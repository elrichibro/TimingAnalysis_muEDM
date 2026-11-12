#include "PreSelection.hpp"

// ROOT DATA FRAME
#include <ROOT/RDataFrame.hxx>

#include <TSystem.h>

#include <algorithm>
#include <cctype>
#include <ctime>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr int kRunsPerPoint = 2;

struct thr_pair {
    double detector1;
    double detector2;
};
}

// ------------------------------------------------------------------------------------------------------------------------------------

int RunFromSampleId(const std::string& id) {
    const size_t ext = id.rfind(".root");
    if (ext == std::string::npos) {
        return -1;
    }

    size_t begin = ext;
    while (begin > 0 && std::isdigit(static_cast<unsigned char>(id[begin - 1]))) {
        begin--;
    }

    if (begin == ext) {
        return -1;
    }

    return std::stoi(id.substr(begin, ext - begin));
}

// ------------------------------------------------------------------------------------------------------------------------------------

// Threshold pair of a run: thr2 cycles fast, thr1 slow, each pair is kept for kRunsPerPoint runs
const thr_pair& ThresholdsOfSample(const std::vector<thr_pair>& table, int run_min, const ROOT::RDF::RSampleInfo& id) {
    const int run = RunFromSampleId(id.AsString());
    const int index = run - run_min;

    if (run < 0 || index < 0 || index >= static_cast<int>(table.size())) {
        throw std::runtime_error("PreSelection: no threshold for sample " + id.AsString());
    }

    return table[index];
}

// ------------------------------------------------------------------------------------------------------------------------------------

int RunPreSelection(const config_struct& cfg) {
    const auto& in = cfg.preSelection.input;
    const auto& out = cfg.preSelection.output;
    const bool verbose = cfg.general.verbose;

    std::cout << "Operation mode: preSelection -> Selected" << std::endl;

    // ------
    // Checks
    // ------

    if (in.dataset_max < in.dataset_min) {
        std::cout << "ERROR: preSelection dataset_max (" << in.dataset_max << ") < dataset_min (" << in.dataset_min << "), exiting." << std::endl;
        return 1;
    }

    if ((in.thr1.empty()) || (in.thr2.empty())) {
        std::cout << "ERROR: preSelection threshold vectors thr1/thr2 must not be empty, exiting." << std::endl;
        return 1;
    }

    if ((in.run_column.empty()) || (in.thr_columns.size() != 2)) {
        std::cout << "ERROR: preSelection needs run_column and exactly 2 thr_columns (detector 1, detector 2), exiting." << std::endl;
        return 1;
    }

    if (out.columns.empty()) {
        std::cout << "ERROR: preSelection output columns list is empty, exiting." << std::endl;
        return 1;
    }

    const int n_runs = in.dataset_max - in.dataset_min + 1;
    const int n_expected = kRunsPerPoint * static_cast<int>(in.thr1.size() * in.thr2.size());

    if (n_runs != n_expected) {
        std::cout << "ERROR: preSelection runs [" << in.dataset_min << ", " << in.dataset_max << "] are " << n_runs
                  << " but " << kRunsPerPoint << " x " << in.thr1.size() << " x " << in.thr2.size() << " = " << n_expected
                  << " are expected from the threshold scan, exiting." << std::endl;
        return 1;
    }


    std::vector<std::string> defined_columns = {in.run_column, in.thr_columns[0], in.thr_columns[1]};

    for (const auto& col : defined_columns) {
        if (std::find(out.columns.begin(), out.columns.end(), col) == out.columns.end()) {
            std::cout << "WARNING: column " << col << " is defined but not listed in preSelection output columns, it will not be saved." << std::endl;
        }
    }

    // ---------------------------
    // Input files and thresholds
    // ---------------------------

    std::vector<std::string> paths;
    std::vector<thr_pair> table;
    std::vector<std::string> missing;

    paths.reserve(n_runs);
    table.reserve(n_runs);

    for (int run = in.dataset_min; run <= in.dataset_max; run++) {
        std::string path = in.input_data + std::to_string(run) + ".root";

        if (gSystem->AccessPathName(path.c_str())) {
            missing.push_back(path);
        }

        const size_t step = static_cast<size_t>((run - in.dataset_min) / kRunsPerPoint);
        const size_t idx1 = step / in.thr2.size();
        const size_t idx2 = step % in.thr2.size();

        paths.push_back(path);
        table.push_back({in.thr1[idx1], in.thr2[idx2]});

        if (verbose) {
            std::cout << "    Run " << run << " -> " << in.thr_columns[0] << " = " << in.thr1[idx1]
                      << ", " << in.thr_columns[1] << " = " << in.thr2[idx2] << std::endl;
        }
    }

    if (!missing.empty()) {
        std::cout << "ERROR: " << missing.size() << " input files not found:" << std::endl;
        for (const auto& path : missing) {
            std::cout << "    " << path << std::endl;
        }
        std::cout << "exiting." << std::endl;
        return 1;
    }

    // -----------
    // Output path
    // -----------

    const std::string out_file = out.output_data;

    // ---------------------
    // Merge + Snapshot
    // ---------------------

    try {
        ROOT::RDataFrame dataset(in.input_tree, paths);

        const int run_min = in.dataset_min;

        ROOT::RDF::RNode node = dataset
            .DefinePerSample(in.run_column, [](unsigned int, const ROOT::RDF::RSampleInfo& id) -> int {
                return RunFromSampleId(id.AsString());
            })
            .DefinePerSample(in.thr_columns[0], [table, run_min](unsigned int, const ROOT::RDF::RSampleInfo& id) -> double {
                return ThresholdsOfSample(table, run_min, id).detector1;
            })
            .DefinePerSample(in.thr_columns[1], [table, run_min](unsigned int, const ROOT::RDF::RSampleInfo& id) -> double {
                return ThresholdsOfSample(table, run_min, id).detector2;
            });

        ROOT::RDF::RSnapshotOptions options;
        options.fMode = "RECREATE";

        std::cout << "Writing " << out_file << " ..." << std::endl;

        std::vector<std::string> snapshot_columns;
        for (const auto& col : out.columns) {
            if (col.find('.') == std::string::npos) {
                snapshot_columns.push_back(col);
                continue;
            }
            std::string copy = col;
            std::replace(copy.begin(), copy.end(), '.', '_');   // Fers5203Pulses.channelID -> Fers5203Pulses_channelID
            node = node.Define(copy, col);
            snapshot_columns.push_back(copy);
        }

        auto snapshot = node.Snapshot(out.output_tree, out_file, snapshot_columns, options);

        std::cout << "preSelection done: " << out_file << " (tree " << out.output_tree << ")" << std::endl;

        std::cout << "    Saved columns:" << std::endl;
        for (const auto& col : snapshot->GetColumnNames()) {
            std::cout << "        " << col << std::endl;
        }

        if (verbose) {
            std::cout << "    Entries: " << *snapshot->Count() << std::endl;
        }

    } catch (const std::exception& except) {
        std::cout << "ERROR: preSelection failed: " << except.what() << std::endl;
        return 1;
    }

    return 0;
}
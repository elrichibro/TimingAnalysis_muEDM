#include "Filter.hpp"

#include <iostream>
#include <memory>
#include <string>

#include <TH1D.h>

// ------------------------------------------------
// FIRST OF EVENT   
// ------------------------------------------------

ROOT::RDF::RNode FirstToA_Node(ROOT::RDF::RNode node, const first_ToA& f_ToA) {   
    node = node
        .Define("Mask_Lead", "FD01_edge == 1")
        .Define("ToA_Lead", "FD01_ToA[Mask_Lead]")
        .Define("Ch_Lead", "FD01_channel[Mask_Lead]");

    std::vector<std::vector<int>> channels = f_ToA.channels;

    auto Calculate_first = [channels](const ROOT::RVec<double>& toa, const ROOT::RVec<unsigned char>& ch) -> std::vector<double> {
        std::vector<std::vector<double>> current_toa(channels.size());
        std::vector<std::vector<bool>> seen(channels.size());
        
        for (size_t k = 0; k < channels.size(); ++k) {
            seen[k].resize(channels[k].size(), false);
        }        
        
        int current_channel = -1;
        
        for(size_t j = 0; j < toa.size(); j++) {
            for(size_t k = 0; k < channels.size(); k++) {
                for (size_t m = 0; m < channels[k].size(); m++) {
                    if (ch[j] == current_channel) {
                        continue;
                    }

                    if (ch[j] == channels[k][m]) {

                        if (!seen[k][m]) {
                            current_toa[k].push_back(toa[j]);
                            seen[k][m] = true;
                        }
                        break;
                    }
                }
            }
        }
    
        std::vector<double> result(channels.size(), -999.0);

        for (size_t k = 0; k < channels.size(); k++) {
            if (!current_toa[k].empty()) {
                result[k] = *std::min_element(current_toa[k].begin(), current_toa[k].end());
            }
        }

        return result;
    };
    

    node = node.Define("Event_ToA_0", Calculate_first, {"ToA_Lead", "Ch_Lead"});

    std::vector<std::string> toa_out = f_ToA.ToA_out;
    
    for (size_t k = 0; k < channels.size(); k++) {
        std::string col_name = toa_out[k];
        
        node = node.Define(col_name, [k](const std::vector<double>& v) -> double {
            return v[k];
        }, {"Event_ToA_0"});
    }

    return node;
}

// -------------------------------------------------------------------------------------------
// TOT
// -------------------------------------------------------------------------------------------

ROOT::RDF::RNode DefineTOT_Node(ROOT::RDF::RNode node, const std::vector<int>& channels) {
    auto calc_TOT = [](const ROOT::RVec<double>& time, const ROOT::RVec<uint8_t>& edge) {
        ROOT::RVec<double> tot;
        const size_t n = time.size();

        if (time.size() < 2) {
            return tot;
        }
        
        for (int i = 0; i < n - 1; i++) {
            if ((edge[i] == 1) && (edge[i + 1] == 0)) {
                tot.push_back(time[i + 1] - time[i]);
            }
        }
        return tot;
    };


    for (size_t i = 0; i < channels.size(); i++) {
        std::string ch = std::to_string(channels[i]);
        
        node = node     
            .Define("Mask_Ch" + ch, "(FD01_channel == " + ch + ")")
            
            .Define("ToA_Ch" + ch, "FD01_ToA[Mask_Ch" + ch + "]")
            .Define("Edge_Ch" + ch, "FD01_edge[Mask_Ch" + ch + "]")
            
            .Define("ToT_Ch" + ch, calc_TOT, {"ToA_Ch" + ch, "Edge_Ch" + ch});
    }
    return node;
}

// -------------------------------------------------------------------------------------------
// MEAN TOA CHANNELS
// -------------------------------------------------------------------------------------------

ROOT::RDF::RNode DefineTOA_mean_Node(ROOT::RDF::RNode node, const std::vector<int>& channels) {   
    auto Calculate_stats = [channels](const ROOT::RVec<double>& toa, const ROOT::RVec<unsigned char>& ch) -> std::vector<double> {
        std::vector<double> result;
        result.assign(2, 0.0);

        std::vector<double> current_toa;
        current_toa.reserve(channels.size());

        int current_channel = -1;
        for(size_t j = 0; j < toa.size(); j++) {
            if (ch[j] != current_channel) {
                for (size_t k = 0; k < channels.size(); ++k) {
                    if (ch[j] == channels[k]) {
                        current_toa.push_back(toa[j]);
                        current_channel = ch[j];
                        break;
                    }
                }
            }
        }

        if (current_toa.empty()) {
            return {-999.0, -999.0};
        }

        double sum_mean = 0;
        double sum_std_dev = 0;

        for(size_t i = 0; i < current_toa.size(); i++) {
            sum_mean += current_toa[i];
        }

        double mean = sum_mean/static_cast<double>(current_toa.size());

        for(size_t i = 0; i < current_toa.size(); i++) {
            sum_std_dev += (mean - current_toa[i]) * (mean - current_toa[i]);
        }

        double std_dev = current_toa.size() <= 1 ? 0.0 : std::sqrt(sum_std_dev/(current_toa.size() - 1));

        result[0] = mean;
        result[1] = std_dev;

        return result;
    };

    node = node
        .Define("Mask_Lead", "FD01_edge == 1")
        .Define("ToA_Lead", "FD01_ToA[Mask_Lead]")
        .Define("Channel_Lead", "FD01_channel[Mask_Lead]")
        
        .Define("TOA_stats", Calculate_stats, {"ToA_Lead", "Channel_Lead"})
        
        .Define("TOA_Mean", "TOA_stats[0]")
        .Define("TOA_StdDev", "TOA_stats[1]");

    return node;
}
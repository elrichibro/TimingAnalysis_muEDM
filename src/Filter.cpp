#include "Filter.hpp"
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TCanvas.h>
#include <TLegend.h>
#include <TH1D.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include <TGraphErrors.h>
#include <TVirtualPad.h>

#include <TH1D.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <cmath>
#include <iostream>

struct dataset {
    std::shared_ptr<ROOT::RDataFrame> dataframe;
    ROOT::RDF::RNode node;
    std::string label;
};

struct DatasetResults {
    std::string label;
    ROOT::RDF::RResultPtr<TH1D> histo;
    ROOT::RDF::RResultPtr<double> mean;
    ROOT::RDF::RResultPtr<double> stddev;
    ROOT::RDF::RResultPtr<ULong64_t> n_events;
};

struct FitResults {
    double noise_yield;
    double noise_yield_err;
    double signal_yield;
    double signal_yield_err;
};

// ------------------------------------------------
// FIRST OF EVENT
// ------------------------------------------------

ROOT::RDF::RNode DefineEventFirst_Node(ROOT::RDF::RNode node, const std::vector<std::vector<int>> channels) {   
    node = node
        .Define("Mask_Lead", "FD01_edge == 1")
        .Define("ToA_Lead", "FD01_ToA[Mask_Lead]")
        .Define("Ch_Lead", "FD01_channel[Mask_Lead]");

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

    for (size_t k = 0; k < channels.size(); k++) {
        std::string col_name = "Event_ToA_0_" + std::to_string(k);
        
        node = node.Define(col_name, [k](const std::vector<double>& v) -> double {
            return v[k];
        }, {"_ToA_Min_Vec"});
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
int PlotTOT(TCanvas* c_all, ROOT::RDF::RNode node, const std::vector<int>& channels, int bins, double min, double max, int run_id) { 
    if (!c_all) return -1;
    
    c_all->cd();
    c_all->Clear();

    const std::vector<int> colors = {
        kBlue + 1, kRed + 1, kGreen + 2, kMagenta + 1, 
        kOrange + 7, kCyan + 2, kViolet - 1, kTeal + 2
    };

    std::vector<ROOT::RDF::RResultPtr<TH1D>> hist_ptrs;
    hist_ptrs.reserve(channels.size());

    for (int ch : channels) {
        std::string col_name = "ToT_Ch" + std::to_string(ch);
        std::string hist_name = "h_ToT_Ch" + std::to_string(ch) + "_run" + std::to_string(run_id);
        std::string hist_title = "ToT;ToT [ns];Events";

        hist_ptrs.push_back(node.Histo1D({hist_name.c_str(), hist_title.c_str(), bins, min, max}, col_name));
    }

    double max_y = 0.0;
    long long total_entries = 0;

    for (size_t i = 0; i < hist_ptrs.size(); i++) {
        double current_max = hist_ptrs[i]->GetMaximum();
        total_entries += hist_ptrs[i]->GetEntries();
        if (current_max > max_y) {
            max_y = current_max;
        }
    }

    if (total_entries == 0) {
        printf("WARNING: no data in [%.2f, %.2f]!\n", min, max);
    }

    auto legend = new TLegend(0.72, 0.68, 0.88, 0.88);
    legend->SetBorderSize(1);
    legend->SetFillColor(0);
    legend->SetTextSize(0.03);
    std::string header_title = "Channels (Run " + std::to_string(run_id) + ")";
    legend->SetHeader(header_title.c_str(), "C");

    for (size_t i = 0; i < hist_ptrs.size(); i++) {
        auto h = hist_ptrs[i].GetPtr();
        int color = colors[i % colors.size()];

        std::string clone_name = std::string(h->GetName()) + "_copy";
        TH1D* h_clone = (TH1D*) h->Clone(clone_name.c_str());
        h_clone->SetDirectory(0);

        h_clone->SetLineColor(color);
        h_clone->SetLineWidth(2);
        h_clone->SetStats(0);

        if (max_y > 0) {
            h_clone->SetMaximum(max_y * 1.2);
        }

        const char* draw_option = (i == 0) ? "HIST" : "HIST SAME";
        h_clone->Draw(draw_option);

        std::string leg_label = "Channel " + std::to_string(channels[i]);
        legend->AddEntry(h_clone, leg_label.c_str(), "l");
    }

    legend->Draw();

    c_all->Modified();
    c_all->Update();

    return 0;
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

int PlotTOA_MEAN(TCanvas* c_all, ROOT::RDF::RNode node, const std::vector<int>& channels, int bins, double min, double max, int run_id) {
    if (!c_all) {
        return -1;
    }

    c_all->cd();
    c_all->Clear();

    const std::vector<int> colors = {
        kBlue + 1, kRed + 1, kGreen + 2, kMagenta + 1, 
        kOrange + 7, kCyan + 2, kViolet - 1, kTeal + 2
    };

    std::vector<ROOT::RDF::RResultPtr<TH1D>> hist_ptrs;
    hist_ptrs.reserve(channels.size());

    std::string hist_title = "ToA;ToA [ns];Events";

    for (int ch : channels) {
        std::string col_name = "FirstToA_Ch" + std::to_string(ch) + "_Lead";
        std::string hist_name = "h_ToA_Ch" + std::to_string(ch) + "_run" + std::to_string(run_id);    

        hist_ptrs.push_back(node.Histo1D({hist_name.c_str(), hist_title.c_str(), bins, min, max}, col_name));
    }

    std::string hist1 = "h_ToA_Mean_run" + std::to_string(run_id);
    hist_ptrs.push_back(node.Histo1D({hist1.c_str(), hist_title.c_str(), bins, min, max}, "TOA_Mean"));

    std::string hist2 = "h_ToA_Mean_run" + std::to_string(run_id);
    hist_ptrs.push_back(node.Histo1D({hist2.c_str(), hist_title.c_str(), bins, min, max}, "Event_ToA_0_0"));

    double max_y = 0.0;
    long long total_entries = 0;

    for (size_t i = 0; i < hist_ptrs.size(); i++) {
        double current_max = hist_ptrs[i]->GetMaximum();
        total_entries += hist_ptrs[i]->GetEntries();
        if (current_max > max_y) {
            max_y = current_max;
        }
    }

    if (total_entries == 0) {
        printf("WARNING: no data in [%.2f, %.2f]!\n", min, max);
    }

    auto legend = new TLegend(0.72, 0.68, 0.88, 0.88);
    legend->SetBorderSize(1);
    legend->SetFillColor(0);
    legend->SetTextSize(0.03);
    std::string header_title = "Channels (Run " + std::to_string(run_id) + ")";
    legend->SetHeader(header_title.c_str(), "C");

    for (size_t i = 0; i < hist_ptrs.size(); i++) {
        auto h = hist_ptrs[i].GetPtr();
        int color = colors[i % colors.size()];

        std::string clone_name = std::string(h->GetName()) + "_copy";
        TH1D* h_clone = (TH1D*) h->Clone(clone_name.c_str());
        h_clone->SetDirectory(0);

        h_clone->SetLineColor(color);
        h_clone->SetLineWidth(2);
        h_clone->SetStats(0);

        if (max_y > 0) {
            h_clone->SetMaximum(max_y * 1.2);
        }

        const char* draw_option = (i == 0) ? "HIST" : "HIST SAME";
        h_clone->Draw(draw_option);

        std::string leg_label = "Channel " + std::to_string(channels[i]);
        legend->AddEntry(h_clone, leg_label.c_str(), "l"); 
    }

    legend->Draw();

    c_all->Modified();
    c_all->Update();

    return 0;
}




// -------------
// MAINS
// -------------


void analisi_TOT(int bins, double min, double max, const std::vector<int> channels) {
    TH1::AddDirectory(kFALSE);

    static int run_id = 0;
    run_id++;

    ROOT::EnableImplicitMT();

    std::string path_name = "./dati/03_11_26/root/Run_Gate_TOF_50mV.root";

    ROOT::RDataFrame data_frame("Event", path_name);
    ROOT::RDF::RNode node = data_frame;

    node = DefineTOT_Node(node, channels);
    
    std::string canvas_name = "c_all_" + std::to_string(run_id); 
    TCanvas* c_all = new TCanvas(canvas_name.c_str(), "ToT", 800, 600);
    
    int ret = PlotTOT(c_all, node, channels, bins, min, max, run_id);

    if (ret != 0) {
        printf("ERROR -> ret: %d\n", ret);
        return;
    }
}


void analisi_TOA_MEAN(int bins ,double min, double max, const std::vector<int> channels) {
    TH1::AddDirectory(kFALSE);

    static int run_id = 0;
    run_id++;
    
    ROOT::EnableImplicitMT();

    std::string path_name = "./dati/03_11_26/root/Run_Gate_TOF_50mV.root";

    ROOT::RDataFrame data_frame("Event", path_name);
    
    ROOT::RDF::RNode node = data_frame;

    node = DefineTOA_mean_Node(node, channels);

    for (size_t i = 0; i < channels.size(); i++) {
        std::string ch = std::to_string(channels[i]);
        node = node     
            .Define("Mask_Ch" + ch, "FD01_channel == " + ch)
            .Define("Mask_Lead", "FD01_edge == 1")

            .Define("ToA_Ch" + ch + "_Lead", "FD01_ToA[Mask_Ch" + ch + " && Mask_Lead]")

            .Define("ToA_Ch" + ch + "_Lead_0", "!ToA_Ch" + ch + "_Lead.empty() ? ToA_Ch" + ch + "_Lead[0] : -999.0");
    }

    std::vector<std::vector<int>> input;
    input.push_back(channels);

    node = DefineEventFirst_Node(node, input);

    std::string canvas_name = "c_all_" + std::to_string(run_id); 
    TCanvas* c_all = new TCanvas(canvas_name.c_str(), "ToA", 800, 600);

    int ret = PlotTOA_MEAN(c_all, node, channels, bins, min, max, run_id);
    
    if(ret != 0) {
        printf("ERROR -> ret: %d\n", ret);
        return;
    }
}
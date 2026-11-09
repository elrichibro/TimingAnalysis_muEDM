#include "Filter.hpp"

#include <iostream>
#include <memory>
#include <string>

#include <TH1D.h>

ROOT::RDF::RNode EventHits(ROOT::RDF::RNode node, const config_struct& cfg, const std::vector<int> channels, const std::string& out_column) {
    auto hits = cfg.efficiency.event.hits;

    auto count_hits = [channels](const ROOT::RVec<unsigned char>& ch) -> int {    
        int hit_count = 0;
        int current_channel = -1;

        for (size_t i = 0; i < ch.size(); i++) {
            if(ch[i] == current_channel) {
                continue;
            }
            current_channel = ch[i];
            
            bool is_in_list = std::find(channels.begin(), channels.end(), ch[i]) != channels.end();

            if (is_in_list) {
                hit_count++;
            }
        }
        return hit_count; 
    };
    
    node = node.Define(out_column, count_hits, {"Ch_Lead"});

    return node;
}






// ------------------------------------------------
// FIRST OF EVENT   
// ------------------------------------------------

ROOT::RDF::RNode FirstToA_Node(ROOT::RDF::RNode node, const config_struct& cfg) {
    std::string ToA_mask =  cfg.io.columns.ToA + "* 3.125/1000";
    std::string edge_mask = cfg.io.columns.edge + "== 1";
    std::string channel_mask = cfg.io.columns.channel + "[Mask_Lead]"; 
    
    node = node
        .Define("ToA_ns", ToA_mask.c_str())
        .Define("Mask_Lead", edge_mask.c_str())
        .Define("ToA_Lead", "ToA_ns[Mask_Lead]")
        .Define("Ch_Lead", channel_mask.c_str());

    std::vector<std::vector<int>> channels = cfg.analysis.f_ToA.channels;

    auto Calculate_first = [channels](const ROOT::RVec<double>& toa, const ROOT::RVec<unsigned char>& ch) -> std::vector<double> {
        std::vector<std::vector<double>> current_toa(channels.size());
        std::vector<std::vector<bool>> seen(channels.size());
        
        for (size_t k = 0; k < channels.size(); ++k) {
            seen[k].resize(channels[k].size(), false);
        }        
                
        for(size_t j = 0; j < toa.size(); j++) {
            for(size_t k = 0; k < channels.size(); k++) {
                for (size_t m = 0; m < channels[k].size(); m++) {
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
    
        std::vector<double> result(channels.size(), -9999.0);

        for (size_t k = 0; k < channels.size(); k++) {
            if (!current_toa[k].empty()) {
                result[k] = *std::min_element(current_toa[k].begin(), current_toa[k].end());
            }
        }

        return result;
    };
    

    node = node.Define("Event_ToA_0", Calculate_first, {"ToA_Lead", "Ch_Lead"});

    std::vector<std::string> toa_out = cfg.analysis.f_ToA.ToA_out;
    
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

ROOT::RDF::RNode DefineTOT_Node(ROOT::RDF::RNode node, const config_struct& cfg) {
    auto ToT = cfg.analysis.ToT;
    auto Column = cfg.io.columns;
    
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


    std::vector<std::vector<int>> channels = cfg.analysis.ToT.channels;
    std::string ToA_column =  cfg.io.columns.ToA + "* 3.125/1000";
    
    node = node.Define("ToA_ns", ToA_column.c_str());

    for (size_t i = 0; i < channels.size(); i++) {
        if(channels[i].size() != ToT.ToT_list[i].size()) {
            printf("Mismatch between dimensions in DefineToT_Node, channels vector is %lu, ToT_list is %lu.\n", channels[i].size(), ToT.ToT_list[i].size());
            return node;
        }

        for(size_t j = 0; j < channels[i].size(); j++) {
            std::string ch = std::to_string(channels[i][j]);
            std::string tot_col = ToT.ToT_list[i][j];

            std::string channel_column = Column.channel + ch;
            std::string edge_column = Column.edge + "[Mask_Ch" + ch + "]";

            node = node
                .Define("Mask_Ch" + ch, channel_column.c_str())
                
                .Define("ToA_Ch" + ch, "ToA_ns[Mask_Ch" + ch + "]")
                .Define("Edge_Ch" + ch, edge_column.c_str())
                
                .Define(tot_col, calc_TOT, {"ToA_Ch" + ch, "Edge_Ch" + ch});
        }
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
#include "Config.hpp"
#include "Filter.hpp"
#include "Visualizer.hpp"
#include "Operations.hpp"
#include "PreSelection.hpp"

#include <ROOT/RDataFrame.hxx>

#include <iostream>
#include <string>

#include <TApplication.h>

/*
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
*/
// -----------------------------------------------------------------------------------------------------------------------------------
// MAIN
// -----------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

    config_struct cfg;
    std::string json_path = "";

    std::vector<double> detector1_hits;
    std::vector<double> detector2_hits;
    
    int verbose = 0;
    bool visualize = false;
    
    // ---------
    // INTERFACE
    // ---------
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg.rfind(".json") != std::string::npos) {
            json_path = arg;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = 1;
        } else if ((arg == "--visualize") || (arg == "-vis")) {
            visualize = true;
        } else {
            std::cout << "ERROR: invalid input command, please try again, exinting.\n" << std::endl;
            return 1;
        }
    }

    // ------------------------------------------------------------------------------------------------------------------------------------
    // JSON CONFIGURATION
    // ------------------------------------------------------------------------------------------------------------------------------------

    if (Configure(cfg, json_path) != 0) {
        std::cout << "Configurations fails, check needed, exiting." << std::endl;
        return 1;
    }

    verbose = cfg.general.verbose;
    visualize = cfg.general.visualize;

    // Verbose JSON configuration
    if (verbose) {
        Verbose_config(cfg);
    }

    // ------------------------------------------------------------------------------------------------------------------------------------
    // OPERATION
    // ------------------------------------------------------------------------------------------------------------------------------------

    ROOT::EnableImplicitMT();// MultiThread option: ON

    // ------------
    // PreSelection
    // ------------

    if (cfg.general.operation_mode == "preSelection") {
        return RunPreSelection(cfg);
    }

    TApplication* app = nullptr;
    if (visualize) {
        std::cout << "TApplication inializating ..." << std::endl;
        app = new TApplication("app", &argc, argv);
    }

    if(cfg.general.data_type == "single") {
        ROOT::RDataFrame data_frame(cfg.io.single_data.input_tree, cfg.io.single_data.input_data);

        // ---------
        // Delta ToA
        // ---------

        if (cfg.general.operation_mode.find("DeltaToA") != std::string::npos) {
            ROOT::RDF::RNode node_DeltaToA = data_frame;

            node_DeltaToA = FirstToA_Node(node_DeltaToA, cfg);
            node_DeltaToA = CalculateDelta_FirstToA(node_DeltaToA, cfg.analysis.f_ToA);
            
            TCanvas* c_DeltaToA = new TCanvas("c_DeltaToA", "DeltaToA", 800, 600);
            int ret = Plot_DeltaFirstToA(c_DeltaToA, node_DeltaToA, cfg.analysis.f_ToA, cfg.canvas.plot_delta_ToA);

            app->Run();
                
            delete app;
            app = nullptr; 
        
        } else if(cfg.general.operation_mode.find("ToT") != std::string::npos) {
            ROOT::RDF::RNode node_ToT = data_frame;
            node_ToT = DefineTOT_Node(node_ToT, cfg);

            TCanvas* c_ToTtrg = new TCanvas("c_ToTtrg", "ToT", 800, 600);
            int ret = Plot_ToT(c_ToTtrg, node_ToT, cfg, cfg.analysis.ToT.ToT_first[2][0]);

            app->Run();
                
            delete app;
            app = nullptr; 

        } else if(cfg.general.operation_mode.find("Efficiency") != std::string::npos) {
            ROOT::RDF::RNode node_Hits = data_frame;

            std::string edge_mask = cfg.io.single_data.columns.edge + "== 1";
            std::string channel_mask = cfg.io.single_data.columns.channel + "[Mask_Lead]";
            
            node_Hits = node_Hits
                .Define("Mask_Lead", edge_mask.c_str())
                .Define("Ch_Lead", channel_mask.c_str())
                .Filter("Any(Ch_Lead == 62)", "Triggered Event");

            if(cfg.efficiency.eff_mode == "hits_ch") {
                std::string col = cfg.efficiency.hits_ch.column_name;
                TCanvas* c_hits = new TCanvas("c_hits", "Hits", 800, 600);

                if(cfg.efficiency.hits_ch.detector == "detector2") {    
                    node_Hits = EventHits(node_Hits, cfg, cfg.efficiency.ch.detector2_channel, col);
                    int ret = Plot_Hits(c_hits, node_Hits, cfg, col, cfg.efficiency.ch.detector2_channel);

                } else if(cfg.efficiency.hits_ch.detector == "detector1") {
                    node_Hits = EventHits(node_Hits, cfg, cfg.efficiency.ch.detector1_channel, col);
                    int ret = Plot_Hits(c_hits, node_Hits, cfg, col, cfg.efficiency.ch.detector1_channel);
                }

                app->Run();
                    
                delete app;
                app = nullptr; 
            }

        } else if(cfg.general.operation_mode.find("efficiency") != std::string::npos) {
            ROOT::RDF::RNode node_Hits = data_frame;

            std::string edge_mask = cfg.io.single_data.columns.edge + "== 1";
            std::string channel_mask = cfg.io.single_data.columns.channel + "[Mask_Lead]";
            
            node_Hits = node_Hits
                .Define("Mask_Lead", edge_mask.c_str())
                .Define("Ch_Lead", channel_mask.c_str())
                .Filter("Any(Ch_Lead == 62)", "Triggered Event");

            auto cut_report = node_Hits.Report();
            
            node_Hits = EventHits(node_Hits, cfg, cfg.efficiency.ch.detector1_channel, cfg.efficiency.event.detector1_column);
            node_Hits = EventHits(node_Hits, cfg, cfg.efficiency.ch.detector2_channel, cfg.efficiency.event.detector2_column);

            auto plot = cfg.canvas.plot_hits;
            auto hist_hits_det1 = node_Hits.Histo1D({"hist_hits_det1", plot.title.c_str(), plot.bins, plot.min, plot.max}, cfg.efficiency.event.detector1_column);
            auto hist_hits_det2 = node_Hits.Histo1D({"hist_hits_det2", plot.title.c_str(), plot.bins, plot.min, plot.max}, cfg.efficiency.event.detector2_column);
            
            auto h_map = node_Hits.Histo2D({"h_map", "Detector 1 vs Detector 2;Detector 1 Hits;Detector 2 Hits", plot.bins, plot.min, plot.max, plot.bins, plot.min, plot.max},
                cfg.efficiency.event.detector1_column, cfg.efficiency.event.detector2_column);

            cut_report->Print();

            TH1D* h_det1 = hist_hits_det1.GetPtr();
            double total_det1_events = h_det1->GetEntries();
            
            std::cout << "-------------------------------------------------" << std::endl;

            for(size_t k = 0; k < cfg.efficiency.ch.detector1_channel.size() + 1; k++) {
                double hit = h_det1->GetBinContent(k + 1);
                detector1_hits.push_back(hit);
                std::cout << "Detector 1 counts for " << k << " hitted channels: " << hit/total_det1_events << std::endl;
            }

            std::cout << "-------------------------------------------------" << std::endl;

            TH1D* h_det2 = hist_hits_det2.GetPtr();
            double total_det2_events = h_det2->GetEntries();
            for(size_t k = 0; k < cfg.efficiency.ch.detector1_channel.size() + 1; k++) {
                double hit = h_det2->GetBinContent(k + 1);
                detector2_hits.push_back(hit);
                std::cout << "Detector 2 counts for " << k << " hitted channels: " << hit/total_det2_events << std::endl;
            }

            std::cout << "-------------------------------------------------" << std::endl;

            if(visualize) {
                TCanvas* c_Det1 = new TCanvas("c_Det1", "Hits Detector 1", 800, 600);
                c_Det1->cd();
                hist_hits_det1->Draw();
                c_Det1->Update();

                TCanvas* c_Det2 = new TCanvas("c_Det2", "Hits Detector 2", 800, 600);
                c_Det2->cd();
                hist_hits_det2->Draw();
                c_Det2->Update();

                TCanvas* c_cov = new TCanvas("c_cov", "Hits Detector 1 vs Detector 2", 800, 600);
                c_cov->cd();
                h_map->Draw("COLZ TEXT");
                c_cov->Update();

                app->Run();

                delete app;
                app = nullptr;
            }
        } else {
            printf("ERROR: invalid operation mode, exiting...");
        }
    }

    return 0;
}
#include "Config.hpp"
#include "Filter.hpp"
#include "Visualizer.hpp"
#include "Operations.hpp"

#include <ROOT/RDataFrame.hxx>

#include <iostream>
#include <string>

#include <TApplication.h>


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

/*
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

    TApplication* app = nullptr;
    if (visualize) {
        std::cout << "TApplication inializating ..." << std::endl;
        app = new TApplication("app", &argc, argv);
    }

    // ---------
    // Delta ToA
    // ---------


    TCanvas* c_DeltaToA = new TCanvas("c_DeltaToA", "DeltaToA", 800, 600);

    if (cfg.general.operation_mode.find("DeltaToA") != std::string::npos) {
        ROOT::RDataFrame data_frame(cfg.io.tree_data_name, cfg.io.in_data_file);
        ROOT::RDF::RNode node_DeltaToA = data_frame;

        node_DeltaToA = FirstToA_Node(node_DeltaToA, cfg.analysis.f_ToA);
        node_DeltaToA = CalculateDelta_FirstToA(node_DeltaToA, cfg.analysis.f_ToA);
        
        
        int ret = Plot_DeltaFirstToA(c_DeltaToA, node_DeltaToA, cfg.analysis.f_ToA, cfg.canvas.plot_delta_ToA);

        app->Run();
            
        delete app;
        app = nullptr; 
    } else {
        printf("ERROR: invalid operation mode, exiting...");
    }

    return 0;
}
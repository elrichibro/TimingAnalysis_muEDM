#include "Config.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>

#include <vector>
#include <string>

// ------------------------------------------------------------------------------------------------------------------------------------

int Configure(config_struct& value, const std::string& json_path) {
    std::ifstream file(json_path);
    
    // Check
    if (!file.is_open()) {
        std::cout << "ERROR: Cannot open: " << json_path << ", exiting.\n" << std::endl;
        return 1;
    }

    try {
        nlohmann::json json_obj;
        file >> json_obj;

        // -------
        // General
        // -------

        if (json_obj.contains("general")) {
            const auto& j = json_obj["general"];

            value.general.operation_mode = j.value("operation_mode", value.general.operation_mode);
            value.general.data_type = j.value("data_type", value.general.data_type);
            value.general.verbose = j.value("verbose", value.general.verbose);
            value.general.visualize = j.value("visualize", value.general.visualize);
        }

        // ---
        // I/O
        // ---

        if (json_obj.contains("io")) {
            const auto& j = json_obj["io"];

            value.io.single_data.input_data = j["single_data"].value("input_data", value.io.single_data.input_data);
            value.io.single_data.input_tree = j["single_data"].value("input_tree", value.io.single_data.input_tree);

            value.io.single_data.columns.channel = j["single_data"]["columns"].value("channel", value.io.single_data.columns.channel);
            value.io.single_data.columns.edge = j["single_data"]["columns"].value("edge", value.io.single_data.columns.edge);
            value.io.single_data.columns.ToA = j["single_data"]["columns"].value("ToA", value.io.single_data.columns.ToA);
            value.io.single_data.columns.ToT = j["single_data"]["columns"].value("ToT", value.io.single_data.columns.ToT);
        }

        // ------------
        // preSelection
        // ------------
        
        if (json_obj.contains("preSelection")) {
            const auto& j = json_obj["preSelection"];

            value.preSelection.input.input_data = j["input"].value("input_data", value.preSelection.input.input_data);
            value.preSelection.input.input_tree = j["input"].value("input_tree", value.preSelection.input.input_tree);
            value.preSelection.input.dataset_min = j["input"].value("dataset_min", value.preSelection.input.dataset_min);
            value.preSelection.input.dataset_max = j["input"].value("dataset_max", value.preSelection.input.dataset_max);
            value.preSelection.input.thr1 = j["input"].value("thr1", value.preSelection.input.thr1);
            value.preSelection.input.thr2 = j["input"].value("thr2", value.preSelection.input.thr2);
            value.preSelection.input.run_column = j["input"].value("run_column", value.preSelection.input.run_column);
            value.preSelection.input.thr_columns = j["input"].value("thr_columns", value.preSelection.input.thr_columns);

            value.preSelection.output.output_data = j["output"].value("output_data", value.preSelection.output.output_data);
            value.preSelection.output.output_tree = j["output"].value("output_tree", value.preSelection.output.output_tree);
            value.preSelection.output.columns = j["output"].value("columns", value.preSelection.output.columns);
        }

        // ----------
        // Efficiency
        // ----------

        if (json_obj.contains("efficiency")) {
            const auto& j = json_obj["efficiency"];

            value.efficiency.eff_mode = j.value("eff_mode", value.efficiency.eff_mode);
            value.efficiency.hits_ch.detector = j["hits_ch"].value("detector", value.efficiency.hits_ch.detector);
            value.efficiency.hits_ch.column_name = j["hits_ch"].value("column_name", value.efficiency.hits_ch.column_name);

            value.efficiency.ch.detector1_channel = j["ch"].value("detector1_channel", value.efficiency.ch.detector1_channel);
            value.efficiency.ch.detector2_channel = j["ch"].value("detector2_channel", value.efficiency.ch.detector2_channel);
            value.efficiency.ch.trigger_channel = j["ch"].value("trigger_channel", value.efficiency.ch.trigger_channel);

            value.efficiency.event.channel_hits = j["event"].value("channel_hits", value.efficiency.event.channel_hits);
            value.efficiency.event.hits = j["event"].value("hits", value.efficiency.event.hits);
            value.efficiency.event.detector1_column = j["event"].value("detector1_column", value.efficiency.event.detector1_column);
            value.efficiency.event.detector2_column = j["event"].value("detector2_column", value.efficiency.event.detector2_column);
        }

        // --------
        // Analysis
        // --------

        if (json_obj.contains("analysis")) {
            const auto& j = json_obj["analysis"];

            value.analysis.analysis_mode = j.value("analysis_mode", value.analysis.analysis_mode);     
            value.analysis.channels = j.value("channels", value.analysis.channels);

            value.analysis.d_ToA.ToA1 = j["d_ToA"].value("ToA1", value.analysis.d_ToA.ToA1);
            value.analysis.d_ToA.ToA2 = j["d_ToA"].value("ToA2", value.analysis.d_ToA.ToA2);

            value.analysis.f_ToA.channels = j["f_ToA"].value("channels", value.analysis.f_ToA.channels);
            value.analysis.f_ToA.ToA_list = j["f_ToA"].value("ToA_list", value.analysis.f_ToA.ToA_list);
            value.analysis.f_ToA.ToA_out = j["f_ToA"].value("ToA_out", value.analysis.f_ToA.ToA_out);

            value.analysis.f_ToA.Delta = j["f_ToA"].value("Delta", value.analysis.f_ToA.Delta);
        
            value.analysis.ToT.channels = j["ToT"].value("channels", value.analysis.ToT.channels);
            value.analysis.ToT.ToT_list = j["ToT"].value("ToT_list", value.analysis.ToT.ToT_list);
            value.analysis.ToT.first = j["ToT"].value("first", value.analysis.ToT.first);
            value.analysis.ToT.ToT_first = j["ToT"].value("ToT_first", value.analysis.ToT.ToT_first);
            value.analysis.ToT.ToT_first_ToA = j["ToT"].value("ToT_first_ToA", value.analysis.ToT.ToT_first_ToA);
        }

        // ------
        // Canvas
        // ------

        if (json_obj.contains("canvas")) {
            const auto& j = json_obj["canvas"];

            value.canvas.plot_delta_ToA.bins = j["plot_delta_ToA"].value("bins", value.canvas.plot_delta_ToA.bins);
            value.canvas.plot_delta_ToA.min = j["plot_delta_ToA"].value("min", value.canvas.plot_delta_ToA.min);
            value.canvas.plot_delta_ToA.max = j["plot_delta_ToA"].value("max", value.canvas.plot_delta_ToA.max);
            value.canvas.plot_delta_ToA.title = j["plot_delta_ToA"].value("title", value.canvas.plot_delta_ToA.title);

            value.canvas.plot_ToT.bins = j["plot_ToT"].value("bins", value.canvas.plot_ToT.bins);
            value.canvas.plot_ToT.min = j["plot_ToT"].value("min", value.canvas.plot_ToT.min);
            value.canvas.plot_ToT.max = j["plot_ToT"].value("max", value.canvas.plot_ToT.max);
            value.canvas.plot_ToT.title = j["plot_ToT"].value("title", value.canvas.plot_ToT.title);

            value.canvas.plot_hits.bins = j["plot_hits"].value("bins", value.canvas.plot_hits.bins);
            value.canvas.plot_hits.min = j["plot_hits"].value("min", value.canvas.plot_hits.min);
            value.canvas.plot_hits.max = j["plot_hits"].value("max", value.canvas.plot_hits.max);
            value.canvas.plot_hits.title = j["plot_hits"].value("title", value.canvas.plot_hits.title);
        }
    } catch (const std::exception& except) {
        std::cout << "ERROR: " << except.what() << std::endl;
        return -1;
    }

    return 0;
}

// ------------------------------------------------------------------------------------------------------------------------------------

void Verbose_config(const config_struct& value) {
    std::cout << "Init Verbose -> Loaded Configuration:" << std::endl;
    
    std::cout << "" << std::endl;
    
    // -------
    // General
    // -------

    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "General settings:" << std::endl;
    std::cout << "    Operation mode: " << value.general.operation_mode << std::endl;
    std::cout << "    Data type: " << value.general.data_type << std::endl;
    std::cout << "    Verbose mode: " << value.general.verbose << std::endl;
    std::cout << "    Visualize flag: " << value.general.visualize << std::endl;

    std::cout << "" << std::endl;    
    std::cout << "--------------------------------------------------------------------" << std::endl;
    
    // ------------
    // Input/Output
    // ------------

    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "I/O settings:" << std::endl;
    std::cout << "    Single data file option: " << std::endl;
    std::cout << "        Input data file: " << value.io.single_data.input_data << std::endl;
    std::cout << "        Data Tree: " << value.io.single_data.input_tree << std::endl;

    std::cout << "    Tree column names: " << std::endl;
    std::cout << "        Channel columns: " << value.io.single_data.columns.channel << std::endl;
    std::cout << "        Edge columns: " << value.io.single_data.columns.edge << std::endl;
    std::cout << "        ToA columns: " << value.io.single_data.columns.ToA << std::endl;
    std::cout << "        ToT columns: " << value.io.single_data.columns.ToT << std::endl;

    std::cout << "" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;

    // ------------
    // PreSelection
    // ------------

    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "PreSelection settings:" << std::endl;
    std::cout << "    Input: " << std::endl;
    std::cout << "        Input data prefix: " << value.preSelection.input.input_data << std::endl;
    std::cout << "        Data Tree: " << value.preSelection.input.input_tree << std::endl;
    std::cout << "        Dataset min: " << value.preSelection.input.dataset_min << std::endl;
    std::cout << "        Dataset max: " << value.preSelection.input.dataset_max << std::endl;
    std::cout << "        Threshold vector 1: {";
    for(size_t i = 0; i < value.preSelection.input.thr1.size(); i++) {
        std::cout << value.preSelection.input.thr1[i];
        if(i != (value.preSelection.input.thr1.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    std::cout << "        Threshold vector 2: {";
    for(size_t i = 0; i < value.preSelection.input.thr2.size(); i++) {
        std::cout << value.preSelection.input.thr2[i];
        if(i != (value.preSelection.input.thr2.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    std::cout << "        Run column: " << value.preSelection.input.run_column << std::endl;
    std::cout << "        Threshold columns: {";
    for(size_t i = 0; i < value.preSelection.input.thr_columns.size(); i++) {
        std::cout << value.preSelection.input.thr_columns[i];
        if(i != (value.preSelection.input.thr_columns.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    std::cout << "    Output: " << std::endl;
    std::cout << "        Output folder: " << value.preSelection.output.output_data << std::endl;
    std::cout << "        Output Tree: " << value.preSelection.output.output_tree << std::endl;
    std::cout << "        Saved columns: {";
    for(size_t i = 0; i < value.preSelection.output.columns.size(); i++) {
        std::cout << value.preSelection.output.columns[i];
        if(i != (value.preSelection.output.columns.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    std::cout << "" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;

    // ----------
    // Efficiency
    // ----------

    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "Efficiency:" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "    Efficiency mode: " << value.efficiency.eff_mode << std::endl;

    std::cout << "    Hits_channel mode: " << std::endl;
    std::cout << "        Detector: " << value.efficiency.hits_ch.detector << std::endl;
    std::cout << "        Column name: " << value.efficiency.hits_ch.column_name << std::endl;
    

    std::cout << "    Channels: " << std::endl;
    std::cout << "        Detector 1 channels: {"; 
    for(size_t i = 0; i < value.efficiency.ch.detector1_channel.size(); i++) {    
        std::cout << value.efficiency.ch.detector1_channel[i];
        if(i != (value.efficiency.ch.detector1_channel.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    std::cout << "        Detector 2 channels: {"; 
    for(size_t i = 0; i < value.efficiency.ch.detector2_channel.size(); i++) {    
        std::cout << value.efficiency.ch.detector2_channel[i];
        if(i != (value.efficiency.ch.detector2_channel.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    std::cout << "        Trigger channel: " << value.efficiency.ch.trigger_channel << std::endl;

    std::cout << "    Event: " << std::endl;
    std::cout << "        Channel hits flag: " << value.efficiency.event.channel_hits << std::endl;
    std::cout << "        Hits: {"; 
    for(size_t i = 0; i < value.efficiency.event.hits.size(); i++) {    
        std::cout << value.efficiency.event.hits[i];
        if(i != (value.efficiency.event.hits.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;
    std::cout << "        Detector 1 column: " << value.efficiency.event.detector1_column << std::endl;
    std::cout << "        Detector 2 column: " << value.efficiency.event.detector2_column << std::endl; 

    std::cout << "" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;

    // --------
    // Analysis
    // --------

    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "Analysis setup:" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "    Analysis Mode: " << value.analysis.analysis_mode << std::endl;
    
    std::cout << "    Analyzed channels: "; 
    for(size_t i = 0; i < value.analysis.channels.size(); i++) {
        std::cout << "{";
        
        for (size_t j = 0; j < value.analysis.channels[i].size(); j++) {
            std::cout << value.analysis.channels[i][j];
            
            if(j != (value.analysis.channels[i].size() - 1)) {
                std::cout << ", ";
            }
        }
        std::cout << "}";
    }
    std::cout << std::endl;

    std::cout << "    Delta ToA:" << std::endl;
    std::cout << "        ToA1: " << value.analysis.d_ToA.ToA1 << std::endl;
    std::cout << "        ToA2: " << value.analysis.d_ToA.ToA2 << std::endl; 

    std::cout << "    First ToA:" << std::endl;
    std::cout << "        Analyzed channels: ";
    for(size_t i = 0; i < value.analysis.f_ToA.channels.size(); i++) {
        std::cout << "{";
        
        for (size_t j = 0; j < value.analysis.f_ToA.channels[i].size(); j++) {
            std::cout << value.analysis.f_ToA.channels[i][j];
            
            if(j != (value.analysis.f_ToA.channels[i].size() - 1)) {
                std::cout << ", ";
            }
        }
        std::cout << "}";
    }
    std::cout << std::endl;
    
    std::cout << "        ToA list: ";
    for(size_t i = 0; i < value.analysis.f_ToA.ToA_list.size(); i++) {
        std::cout << "{";
        
        for (size_t j = 0; j < value.analysis.f_ToA.ToA_list[i].size(); j++) {
            std::cout << value.analysis.f_ToA.ToA_list[i][j];
            
            if(j != (value.analysis.f_ToA.ToA_list[i].size() - 1)) {
                std::cout << ", ";
            }
        }
        std::cout << "}";
    }
    std::cout << std::endl;

    std::cout << "        ToA out: {";
    for(size_t i = 0; i < value.analysis.f_ToA.ToA_out.size(); i++) {
        std::cout << value.analysis.f_ToA.ToA_out[i];
        
        if(i != (value.analysis.f_ToA.ToA_out.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    std::cout << "        Delta first ToA: " << value.analysis.f_ToA.Delta << std::endl;

    std::cout << "    First ToT:" << std::endl;
    std::cout << "        ToT channels: ";
    for(size_t i = 0; i < value.analysis.ToT.channels.size(); i++) {
        std::cout << "{";
        
        for (size_t j = 0; j < value.analysis.ToT.channels[i].size(); j++) {
            std::cout << value.analysis.ToT.channels[i][j];
            
            if(j != (value.analysis.ToT.channels[i].size() - 1)) {
                std::cout << ", ";
            }
        }
        std::cout << "}";
    }
    std::cout << std::endl;
    
    std::cout << "        ToT list: ";
    for(size_t i = 0; i < value.analysis.ToT.ToT_list.size(); i++) {
        std::cout << "{";
        
        for (size_t j = 0; j < value.analysis.ToT.ToT_list[i].size(); j++) {
            std::cout << value.analysis.ToT.ToT_list[i][j];
            
            if(j != (value.analysis.ToT.ToT_list[i].size() - 1)) {
                std::cout << ", ";
            }
        }
        std::cout << "}";
    }
    std::cout << std::endl;

    std::cout << "        ToT first option: " << value.analysis.ToT.first << std::endl;

    std::cout << "        ToT first: ";
    for(size_t i = 0; i < value.analysis.ToT.ToT_first.size(); i++) {
        std::cout << "{";
        
        for (size_t j = 0; j < value.analysis.ToT.ToT_first[i].size(); j++) {
            std::cout << value.analysis.ToT.ToT_first[i][j];
            
            if(j != (value.analysis.ToT.ToT_first[i].size() - 1)) {
                std::cout << ", ";
            }
        }
        std::cout << "}";
    }
    std::cout << std::endl;

    std::cout << "        ToT first ToA: {";
    for(size_t i = 0; i < value.analysis.ToT.ToT_first_ToA.size(); i++) {
        std::cout << value.analysis.ToT.ToT_first_ToA[i];
        
        if(i != (value.analysis.ToT.ToT_first_ToA.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    std::cout << "" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;
    
    // -----
    // Plots
    // -----

    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "Canvas settings:" << std::endl;
    std::cout << "" << std::endl;


    std::cout << "    Plot Delta fisrt ToA: " << std::endl;
    std::cout << "        Plot bin value: " << value.canvas.plot_delta_ToA.bins << std::endl;
    std::cout << "        Plot min bin value: " << value.canvas.plot_delta_ToA.min << std::endl;
    std::cout << "        Plot max bin value: " << value.canvas.plot_delta_ToA.max << std::endl;
    std::cout << "        Plot title: " << value.canvas.plot_delta_ToA.title << std::endl;

    std::cout << "    Plot ToT histogram: " << std::endl;
    std::cout << "        Plot bin value: " << value.canvas.plot_ToT.bins << std::endl;
    std::cout << "        Plot min bin value: " << value.canvas.plot_ToT.min << std::endl;
    std::cout << "        Plot max bin value: " << value.canvas.plot_ToT.max << std::endl;
    std::cout << "        Plot title: " << value.canvas.plot_ToT.title << std::endl;

    std::cout << "    Plot Hits histogram: " << std::endl;
    std::cout << "        Plot bin value: " << value.canvas.plot_hits.bins << std::endl;
    std::cout << "        Plot min bin value: " << value.canvas.plot_hits.min << std::endl;
    std::cout << "        Plot max bin value: " << value.canvas.plot_hits.max << std::endl;
    std::cout << "        Plot title: " << value.canvas.plot_hits.title << std::endl;

    std::cout << "" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;
    
    std::cout << "End of Verbose" << std::endl;
    std::cout << "" << std::endl;
}

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

        if (json_obj.contains("general")) {
            const auto& j = json_obj["general"];

            value.general.operation_mode = j.value("operation_mode", value.general.operation_mode);
            value.general.verbose = j.value("verbose", value.general.verbose);
            value.general.visualize = j.value("visualize", value.general.visualize);
        }

        if (json_obj.contains("io")) {
            const auto& j = json_obj["io"];

            value.io.tree_data_name = j.value("tree_data_name", value.io.tree_data_name);
            value.io.in_data_file = j.value("in_data_file", value.io.in_data_file);

            value.io.columns.channel = j["columns"].value("channel", value.io.columns.channel);
            value.io.columns.edge = j["columns"].value("edge", value.io.columns.edge);
            value.io.columns.ToA = j["columns"].value("ToA", value.io.columns.ToA);
            value.io.columns.ToT = j["columns"].value("ToT", value.io.columns.ToT);
        }

        if (json_obj.contains("efficiency")) {
            const auto& j = json_obj["efficiency"];

            value.efficiency.gate_channel = j.value("gate_channel", value.efficiency.gate_channel);
            value.efficiency.tof_channel = j.value("tof_channel", value.efficiency.tof_channel);
            value.efficiency.trigger_channel = j.value("trigger_channel", value.efficiency.trigger_channel);

            value.efficiency.event.hits = j["event"].value("hits", value.efficiency.event.hits);

            value.efficiency.event.gate_column = j["event"].value("gate_column", value.efficiency.event.gate_column);
            value.efficiency.event.tof_column = j["event"].value("tof_column", value.efficiency.event.tof_column);
        }

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
            value.analysis.ToT.ToT_first = j["ToT"].value("ToT_first", value.analysis.ToT.ToT_first);
        }

        if (json_obj.contains("canvas")) {
            const auto& j = json_obj["canvas"];

            value.canvas.plot_delta_ToA.bins = j["plot_delta_ToA"].value("bins", value.canvas.plot_delta_ToA.bins);
            value.canvas.plot_delta_ToA.min = j["plot_delta_ToA"].value("min", value.canvas.plot_delta_ToA.min);
            value.canvas.plot_delta_ToA.max = j["plot_delta_ToA"].value("max", value.canvas.plot_delta_ToA.max);
            value.canvas.plot_delta_ToA.title = j["plot_delta_ToA"].value("title", value.canvas.plot_delta_ToA.title);

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
    std::cout << "    Input data file: " << value.io.in_data_file << std::endl;
    std::cout << "    Data Tree: " << value.io.tree_data_name << std::endl;
    std::cout << "    Tree column names: " << std::endl;
    std::cout << "        Channel columns: " << value.io.columns.channel << std::endl;
    std::cout << "        Edge columns: " << value.io.columns.edge << std::endl;
    std::cout << "        ToA columns: " << value.io.columns.ToA << std::endl;
    std::cout << "        ToT columns: " << value.io.columns.ToT << std::endl;
    
    std::cout << "" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;

    // ----------
    // Efficiency
    // ----------

    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "Efficiency setup:" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "    Gate channels: {"; 
    for(size_t i = 0; i < value.efficiency.gate_channel.size(); i++) {    
        std::cout << value.efficiency.gate_channel[i];
        if(i != (value.efficiency.gate_channel.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    std::cout << "    ToF channels: {"; 
    for(size_t i = 0; i < value.efficiency.tof_channel.size(); i++) {    
        std::cout << value.efficiency.tof_channel[i];
        if(i != (value.efficiency.tof_channel.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    std::cout << "    Trigger channel: " << value.efficiency.trigger_channel << std::endl;

    std::cout << "    Event: " << std::endl;
    std::cout << "        Hits: {"; 
    for(size_t i = 0; i < value.efficiency.event.hits.size(); i++) {    
        std::cout << value.efficiency.event.hits[i];
        if(i != (value.efficiency.event.hits.size() - 1)) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;
    std::cout << "        Gate column: " << value.efficiency.event.gate_column << std::endl;
    std::cout << "        ToF column: " << value.efficiency.event.tof_column << std::endl; 

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

    std::cout << "        ToT first: {";
    for(size_t i = 0; i < value.analysis.ToT.ToT_first.size(); i++) {
        std::cout << value.analysis.ToT.ToT_first[i];
        
        if(i != (value.analysis.ToT.ToT_first.size() - 1)) {
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

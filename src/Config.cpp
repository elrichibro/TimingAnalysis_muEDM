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
        }

        if (json_obj.contains("analysis")) {
            const auto& j = json_obj["analysis"];

            value.analysis.analysis_mode = j.value("analysis_mode", value.analysis.analysis_mode);     
            value.analysis.channels = j.value("channels", value.analysis.channels);
        }

        if (json_obj.contains("plot")) {
            const auto& j = json_obj["plot"];

            value.plot.bins = j.value("bins", value.plot.bins);
            value.plot.min = j.value("min", value.plot.min);
            value.plot.max = j.value("max", value.plot.max);
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

    std::cout << "" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;

    // -----
    // Plots
    // -----

    std::cout << "--------------------------------------------------------------------" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "Plot settings:" << std::endl;
    std::cout << "" << std::endl;

    std::cout << "    Plot bins: " << value.plot.bins << std::endl;
    std::cout << "    Plot min bin value: " << value.plot.min << std::endl;
    std::cout << "    Plot max bin value: " << value.plot.max << std::endl;

  
    std::cout << "" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;
    
    std::cout << "End of Verbose" << std::endl;
    std::cout << "" << std::endl;
}

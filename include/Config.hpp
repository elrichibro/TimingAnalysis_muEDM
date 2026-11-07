#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>

struct general_config {
    std::string operation_mode = "";// Operation mode
    int verbose = 0;// General verbose flag
    bool visualize = false;// General visualization flag
};

/// @brief Stores input/output file paths
struct io_config {
    std::string tree_data_name = "";// Data tree name
    std::string in_data_file = "";// Input data file path
};

struct analysis_config {
    std::string analysis_mode;
    std::vector<std::vector<int>> channels;
};

struct plot_config {
    int bins;
    double min;
    double max;
};

struct config_struct {
    general_config general;
    io_config io;
    analysis_config analysis;
    plot_config plot;
};

// -----------------------
// Configuration functions
// -----------------------

int Configure(config_struct& value, const std::string& json_path);

// ------------------------------------------------------------------------------------------------------------------------------------

void Verbose_config(const config_struct& value);

#endif
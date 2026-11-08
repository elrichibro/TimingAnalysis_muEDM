#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>

struct general_config {
    std::string operation_mode = "";
    int verbose = 0;
    bool visualize = false;
};

struct columns_config {
    std::string channel = "";
    std::string edge = "";
    std::string ToA = "";
    std::string ToT = "";
};

struct io_config {
    std::string tree_data_name = "";
    std::string in_data_file = "";
    columns_config columns;
};

struct events_config {

};

struct efficiency_config {
    std::vector<int> ref_channel;
    std::vector<int> eff_channel;
};

struct delta_ToA {
    std::string ToA1 = "";
    std::string ToA2 = "";
};

struct first_ToA {
    std::vector<std::vector<int>> channels;
    std::vector<std::vector<std::string>> ToA_list;
    std::vector<std::string> ToA_out;
    std::string Delta;
};

struct first_ToT {
    std::vector<std::vector<int>> channels;
    std::vector<std::vector<std::string>> ToT_list;
    std::vector<std::string> ToT_first;
};

struct analysis_config {
    std::string analysis_mode;
    std::vector<std::vector<int>> channels;
    delta_ToA d_ToA;
    first_ToA f_ToA;
    first_ToT ToT;
};

struct plot_config {
    int bins;
    double min;
    double max;
    std::string title;
};

struct canvas_config {
    plot_config plot_delta_ToA;
};

struct config_struct {
    general_config general;
    io_config io;
    efficiency_config efficiency;
    analysis_config analysis;
    canvas_config canvas;
};

// -----------------------
// Configuration functions
// -----------------------

int Configure(config_struct& value, const std::string& json_path);

// ------------------------------------------------------------------------------------------------------------------------------------

void Verbose_config(const config_struct& value);

#endif
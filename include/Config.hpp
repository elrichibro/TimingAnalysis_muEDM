#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>

// -------
// General
// -------

struct general_config {
    std::string operation_mode = "";
    std::string data_type = "";
    int verbose = 0;
    bool visualize = false;
};

// ---
// I/O
// ---

struct single_data_config {
    std::string tree_data_name = "";
    std::string in_data_file = "";
};

struct all_data_config {
    std::string tree_all_data = "";
    std::string in_all_data = "";
    int dataset_min;
    int dataset_max;
    std::vector<double> thr1;
    std::vector<double> thr2;
};

struct columns_config {
    std::string channel = "";
    std::string edge = "";
    std::string ToA = "";
    std::string ToT = "";
};

struct io_config {
    single_data_config single_data;
    all_data_config all_data;
    columns_config columns;
};

// ----------
// Efficiency
// ----------

struct channels {
    std::vector<int> gate_channel;
    std::vector<int> tof_channel;
    int trigger_channel = -1;
};

struct hits_channel {
    std::string detector = "";
    std::string column_name = "";
};

struct events_config {
    bool channel_hits = false;
    std::vector<int> hits;
    std::string gate_column = " ";
    std::string tof_column = " ";
};

struct efficiency_config {
    std::string eff_mode = "";
    hits_channel hits_ch;
    channels ch;
    events_config event;
};

// --------
// Analysis
// --------

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
    bool first = false;
    std::vector<std::vector<std::string>> ToT_first;
    std::vector<std::string> ToT_first_ToA;
};

struct analysis_config {
    std::string analysis_mode;
    std::vector<std::vector<int>> channels;
    delta_ToA d_ToA;
    first_ToA f_ToA;
    first_ToT ToT;
};

// ------
// Canvas
// ------

struct plot_config {
    int bins;
    double min;
    double max;
    std::string title;
};

struct canvas_config {
    plot_config plot_delta_ToA;
    plot_config plot_ToT;
    plot_config plot_hits;
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

// ------------------------------------------------------------------------------------------------------------------------------------


#endif
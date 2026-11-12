#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>

// -------
// General
// -------

struct general_cfg {
    std::string operation_mode = "";
    std::string data_type = "";
    int verbose = 0;
    bool visualize = false;
};

// ---
// I/O
// ---

struct columns_cfg {
    std::string channel = "";
    std::string edge = "";
    std::string ToA = "";
    std::string ToT = "";
};

struct single_data_cfg {
    std::string input_data = "";
    std::string input_tree = "";
    columns_cfg columns;
};

struct io_cfg {
    single_data_cfg single_data;
};

// ------------
// PreSelection
// ------------

struct preSelection_input_cfg {
    std::string input_data = "";
    std::string input_tree = "";
    int dataset_min = 0;
    int dataset_max = -1;
    std::vector<double> thr1;
    std::vector<double> thr2;
    std::string run_column = "";
    std::vector<std::string> thr_columns;   // {detector 1, detector 2}
};

struct preSelection_output_cfg {
    std::string output_data = "";
    std::string output_tree = "";
    std::vector<std::string> columns;       // columns saved in the snapshot
};

struct preSelection_cfg {
    preSelection_input_cfg input;
    preSelection_output_cfg output;
};

// ----------
// Efficiency
// ----------

struct channels {
    std::vector<int> detector1_channel;
    std::vector<int> detector2_channel;
    int trigger_channel = -1;
};

struct hits_channel {
    std::string detector = "";
    std::string column_name = "";
};

struct events_cfg {
    bool channel_hits = false;
    std::vector<int> hits;
    std::string detector1_column = " ";
    std::string detector2_column = " ";
};

struct efficiency_cfg {
    std::string eff_mode = "";
    hits_channel hits_ch;
    channels ch;
    events_cfg event;
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

struct analysis_cfg {
    std::string analysis_mode;
    std::vector<std::vector<int>> channels;
    delta_ToA d_ToA;
    first_ToA f_ToA;
    first_ToT ToT;
};

// ------
// Canvas
// ------

struct plot_cfg {
    int bins;
    double min;
    double max;
    std::string title;
};

struct canvas_cfg {
    plot_cfg plot_delta_ToA;
    plot_cfg plot_ToT;
    plot_cfg plot_hits;
};

struct config_struct {
    general_cfg general;
    io_cfg io;
    preSelection_cfg preSelection;
    efficiency_cfg efficiency;
    analysis_cfg analysis;
    canvas_cfg canvas;
};

// -----------------------
// Configuration functions
// -----------------------

int Configure(config_struct& value, const std::string& json_path);

// ------------------------------------------------------------------------------------------------------------------------------------

void Verbose_config(const config_struct& value);

// ------------------------------------------------------------------------------------------------------------------------------------


#endif
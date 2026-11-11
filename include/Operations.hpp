#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

#include "Config.hpp"

// ROOT DATA FRAME
#include <ROOT/RDataFrame.hxx>


ROOT::RDF::RNode CalculateDelta_FirstToA(ROOT::RDF::RNode node, const first_ToA& f_ToA);

struct Eff_Container_Multi {
    double thr1;
    double thr2;
    double eff;
};

int ProcessAllData(const config_struct& cfg, std::vector<std::string>& paths, std::unordered_map<std::string, std::pair<double, double>>& file_container);

#endif
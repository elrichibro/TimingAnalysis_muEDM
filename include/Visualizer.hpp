#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include "Config.hpp"

// ROOT DATA FRAME
#include <ROOT/RDataFrame.hxx>

// VISUALIZATION
#include <TCanvas.h>

#include <vector>

int Plot_ToT(TCanvas* c_all, ROOT::RDF::RNode node, const config_struct& cfg, const std::string& column);

int PlotTOA_MEAN(TCanvas* c_all, ROOT::RDF::RNode node, const std::vector<int>& channels, int bins, double min, double max, int run_id);

int Plot_DeltaFirstToA(TCanvas* c_all, ROOT::RDF::RNode node, const first_ToA& f_ToA, const plot_config& plot);

int Plot_Hits(TCanvas* c_all, ROOT::RDF::RNode node, const config_struct& cfg, const std::string& column);


#endif
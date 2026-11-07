#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

// ROOT DATA FRAME
#include <ROOT/RDataFrame.hxx>

// VISUALIZATION
#include <TCanvas.h>

#include <vector>


int PlotTOT(TCanvas* c_all, ROOT::RDF::RNode node, const std::vector<int>& channels, int bins, double min, double max, int run_id);

int PlotTOA_MEAN(TCanvas* c_all, ROOT::RDF::RNode node, const std::vector<int>& channels, int bins, double min, double max, int run_id);


#endif
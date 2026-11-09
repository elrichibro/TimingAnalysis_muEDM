#ifndef FILTER_HPP
#define FILTER_HPP

#include "Config.hpp"

// ROOT DATA FRAME
#include <ROOT/RDataFrame.hxx>

// STANDARD TEMPLATE LIBRARY
#include <vector>

// Efficiency Node

ROOT::RDF::RNode EventHits(ROOT::RDF::RNode node, const config_struct& cfg, const std::vector<int> channels, const std::string& out_column);   



ROOT::RDF::RNode FirstToA_Node(ROOT::RDF::RNode node, const config_struct& cfg);   

ROOT::RDF::RNode DefineTOT_Node(ROOT::RDF::RNode node, const config_struct& cfg);

ROOT::RDF::RNode DefineTOA_mean_Node(ROOT::RDF::RNode node, const std::vector<int>& channels);



#endif 


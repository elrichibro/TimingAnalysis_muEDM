#ifndef FILTER_HPP
#define FILTER_HPP

#include "Config.hpp"

// ROOT DATA FRAME
#include <ROOT/RDataFrame.hxx>

// STANDARD TEMPLATE LIBRARY
#include <vector>

ROOT::RDF::RNode FirstToA_Node(ROOT::RDF::RNode node, const first_ToA& f_ToA);   

ROOT::RDF::RNode DefineTOT_Node(ROOT::RDF::RNode node, const std::vector<int>& channels);

ROOT::RDF::RNode DefineTOA_mean_Node(ROOT::RDF::RNode node, const std::vector<int>& channels);

#endif 


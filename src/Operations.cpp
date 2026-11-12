#include "Operations.hpp"

ROOT::RDF::RNode CalculateDelta_FirstToA(ROOT::RDF::RNode node, const first_ToA& f_ToA) {
    std::string delta = f_ToA.ToA_out[0] + "-" + f_ToA.ToA_out[1];

    return node = node.Define(f_ToA.Delta, delta);
}

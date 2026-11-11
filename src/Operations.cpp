#include "Operations.hpp"

ROOT::RDF::RNode CalculateDelta_FirstToA(ROOT::RDF::RNode node, const first_ToA& f_ToA) {
    std::string delta = f_ToA.ToA_out[0] + "-" + f_ToA.ToA_out[1];  
    
    return node = node.Define(f_ToA.Delta, delta);
}

int ProcessAllData(const config_struct& cfg, std::vector<std::string>& paths, std::unordered_map<std::string, std::pair<double, double>>& file_container) {
    int min = cfg.io.all_data.dataset_min;
    int max = cfg.io.all_data.dataset_max;
    int delta = max - min + 1; 

    const auto& v_thr1 = cfg.io.all_data.thr1;
    const auto& v_thr2 = cfg.io.all_data.thr2;

    paths.reserve(delta);
    file_container.reserve(delta);

    int thr1 = 0;
    int thr2 = 0;

    for (int i = min; i <= max; i++) {
        std::string path = "../../dati/09_11_26/raw00" + std::to_string(i) + ".root";
        paths.push_back(path);
    
        int step = (i - min) / 2;

        size_t idx2 = step % v_thr2.size();
        
        size_t idx1 = step / v_thr2.size();

        if (idx1 >= v_thr1.size()) {
            idx1 = v_thr1.size() - 1;
        }

        std::string root_key = cfg.io.all_data.tree_all_data + "/" + path;

        file_container[root_key] = {v_thr1[idx1], v_thr2[idx2]};
    }

    return 0;
}

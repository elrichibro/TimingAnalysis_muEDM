#include "Config.hpp"
#include "Filter.hpp"

#include <ROOT/RDataFrame.hxx>

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {

    config_struct cfg;
    std::string json_path = "";
    
    int verbose = 0;
    bool visualize = false;
    
    // ---------
    // INTERFACE
    // ---------
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg.rfind(".json") != std::string::npos) {
            json_path = arg;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = 1;
        } else if ((arg == "--visualize") || (arg == "-vis")) {
            visualize = true;
        } else {
            std::cout << "ERROR: invalid input command, please try again, exinting.\n" << std::endl;
            return 1;
        }
    }

    // ------------------------------------------------------------------------------------------------------------------------------------
    // JSON CONFIGURATION
    // ------------------------------------------------------------------------------------------------------------------------------------

    if (Configure(cfg, json_path) != 0) {
        std::cout << "Configurations fails, check needed, exiting." << std::endl;
        return 1;
    }

    verbose = cfg.general.verbose;
    visualize = cfg.general.visualize;

    // Verbose JSON configuration
    if (verbose) {
        Verbose_config(cfg);
    }






    
}
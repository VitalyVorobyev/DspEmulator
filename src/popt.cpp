#include "popt.h"

#include <fstream>
#include <iostream>
#include <cstdlib>  // exit

using std::string;
using std::ifstream;
using std::cout;
using std::endl;
using std::cerr;

POpt::POpt(int argc, char** argv) :
    generalOptions("General"), fileOptions("File"),
    m_verb(false), m_ampThres(100) {
   
    generalOptions.add_options()
        ("help, h", "Help info")
        ("config, cf", po::value<string>()->default_value("config.cfg"), "Config file")
        ("ifile, f",   po::value<string>(), "Input file")
        ("itree, t",   po::value<string>(), "Input tree name")
        ("verb, v", "Verbatim output");

    fileOptions.add_options()
        ("dspPath",        po::value<string>()->required(), "Path to DSP coeffs")
        ("inputPath",      po::value<string>()->required(), "Path to input file")
        ("inputTreeName",  po::value<string>()->required(), "Input Tree")
        ("outputPath",     po::value<string>()->required(), "Path to output file")
        ("outputTreeName", po::value<string>()->required(), "Output tree")
        ("ampThres",   po::value<int>(&m_ampThres), "Low amplitude threshold");

    try {
        po::store(po::parse_command_line(argc, argv, generalOptions), vm);
        notify(vm);
        if (vm.count("help")) {
            helpScreen();
            exit(0);
        }
        if (vm.count("verb")) {
            m_verb = true;
        }
        if (vm.count("config")) {
            ifstream ifs{vm["config"].as<string>()};
            if (ifs) {
                store(po::parse_config_file(ifs, fileOptions), vm);
                notify(vm);
                parseConfig();
            }
        } else {
            throw new po::error("wrong config file");
        }
        if (vm.count("ifile")) {
//            cout <<
            pars.emplace("ifile", vm["ifile"].as<string>());
            pars.emplace("ofile", "refit_" + vm["ifile"].as<string>());
        } else {
            throw new po::error("input file is not specified");
        }
        if (vm.count("itree"))
            pars.emplace("itree", vm["itree"].as<string>());

    } catch (po::error& e) {
        cerr << "ERROR: " << e.what() << endl
             << generalOptions << endl;
    }
}

void POpt::parseConfig() {
    if (vm.count("dspPath"))
        pars.emplace("dspPath", vm["dspPath"].as<string>());
    else
        throw new po::error("DSP path not specified");

    if (vm.count("inputPath"))
        pars.emplace("ipath", vm["inputPath"].as<string>());
    else
        throw new po::error("Input path not specified");

    if (vm.count("inputTreeName"))
        pars.emplace("itree", vm["inputTreeName"].as<string>());
    else
        throw new po::error("Input TTree name not specified");

    if (vm.count("outputPath"))
        pars.emplace("opath", vm["outputPath"].as<string>());
    else
        throw new po::error("Output path not specified");

    if (vm.count("outputTreeName"))
        pars.emplace("otree", vm["outputTreeName"].as<string>());
    else
        throw new po::error("Output TTree name not specified");
}

void POpt::helpScreen() {
    cout << "  === DSP Emulator ===" << endl
         << "Reads ADC data and makes signal shape fit" << endl
         << "Usage: ./adcfit -cf [configFile] -if [inputFile]" << endl;
}

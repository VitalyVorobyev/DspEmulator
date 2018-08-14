#pragma once

#include <boost/program_options.hpp>
#include <string>
#include <map>

namespace po = boost::program_options;

/// Class POpt manages configuration
class POpt {
    po::options_description generalOptions;
    po::options_description fileOptions;
    po::variables_map vm;

    /// Map to store configuration
    std::map<std::string, std::string> pars;
    bool m_verb;
    int m_ampThres;

    /// Parse config file
    void parseConfig();
    /// Show help screen
    static void helpScreen();

 public:
    /// Constructor
    POpt(int argc, char** argv);

    /// Path to DSP coefficients
    inline std::string dspPath() const {return pars.at("dspPath");}
    /// Input file name
    inline std::string ifile() const {return pars.at("ifile");}
    /// Input files path
    inline std::string ipath() const {return pars.at("ipath");}
    /// Input TTree name
    inline std::string itree() const {return pars.at("itree");}
    /// Output file name
    inline std::string ofile() const {return pars.at("ofile");}
    /// Output files path
    inline std::string opath() const {return pars.at("opath");}
    /// Output TTree name
    inline std::string otree() const {return pars.at("otree");}
    /// Verbatim output. False by default
    inline bool verb() const {return m_verb;}
    /// Amplitude threshold. Will skip events with amplitude less then the threshold
    inline int ampThres() const {return m_ampThres;}
};

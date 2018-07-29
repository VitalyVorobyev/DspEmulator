#pragma once

#include <memory>
#include <vector>
#include <string>
#include <tuple>

#include "eclDspEmulator.h"

// forward declarations
class eclFitHit;
class TTree;
class TFile;
class TChain;

class eclSDSPTester {
    // Aliases
    using OTuple = std::tuple<std::unique_ptr<TFile>,
                              std::unique_ptr<TTree>,
                              std::unique_ptr<eclFitHit> >;
    using ITuple = std::tuple<std::unique_ptr<TChain>,
                              std::unique_ptr<eclFitHit> >;

    std::vector<std::unique_ptr<eclDspEmulator> > m_refit;

    void init();

    ITuple getTree(const std::string& ifname, const std::string& itname) const;
    OTuple makeTree(const std::string& ofname, const std::string& otname) const;

 public:
    /** Constructor */
    eclSDSPTester(const std::string& chmap);
    /**  */
    void refit(const std::string& ifname,
               const std::string& itname,
               const std::string& ofname,
               const std::string& otname, bool verb=false) const;
};

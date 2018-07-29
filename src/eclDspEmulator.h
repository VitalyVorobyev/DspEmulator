# pragma once

#include <vector>
#include <memory>
#include <string>

#include "dspPars.h"
#include "dspCoeffs.h"

// Auxiliary class to store fit results
class dspResult {
 public:
    int mi5; // iteration #
    int lcp; // pedestal
    int lar; // amplitude
    int ltr; // time
    int lch3; // chi2
    int lch4; // chi2
    int lq; // validity code

 public:
    dspResult() : mi5(0), lcp(0), lar(0), ltr(0), lch3(0), lch4(0), lq(0) {}
};

class eclDspEmulator {
    // the dsp logic emulator
    void lftda(const int *y, int ttrig2, int ch, int n16=16);
    // store results
    void ou(const int *y);
    // low amplitude
    void lam(const int *y, int ch);
    // final calculations
    void fin(const int *y, int ch, int n16);

    // parameters to be calculated
    dspResult m_dspr;
    std::unique_ptr<dspCoeffs> dspc;
    std::unique_ptr<dspPars> dspp;

    // misterious parameters
    long long A1;
    long long A2;
    long long B1;
    long long C1;
    long long ch1;
    long long int z0; // //initial time index
    int validity_code;
    int z00;
    int T;
    int it;
    int ttrig;

    // static constants
    /// magic constants
    static const std::vector<long long> k_np;

    /// limits
    static constexpr int it_h = 191;
    static constexpr int it_l = 0;

 public:
    eclDspEmulator(const std::string& fname);

    const dspResult& fit(int ichannel, int trigger_time, const std::vector<int>& adc_data);
    const dspResult& result() const {return m_dspr;}
};

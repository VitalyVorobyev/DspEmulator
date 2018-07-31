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

    void print() const;
};

class eclDspEmulator {
    /// the dsp logic emulator
    void lftda(const int *y, int ttrig2, int ch, int n16=16);
    /// store results
    void ou(const int *y, long long ch1, int it=0);
    /// low amplitude
    void lowAmpAlg(const int *y, int ch);
    /// final calculations
    void fin(const int *y, int ch, int n16, int it=0);
    /// Too large amplitude algorithm
    void largeAmpAlg(const int *y);
    /// Iterative algorithm
    void iterAlg(const int *y, int ch, int ttrig2, int n16);

    // parameters to be calculated
    dspResult m_dspr;
    std::unique_ptr<dspCoeffs> dspc;
    std::unique_ptr<dspPars> dspp;

    // misterious parameters
    long long A1;
    long long B1;
    long long C1;
    long long int z0; // //initial time index
    int validity_code;
    int z00;
    int T;
    int ttrig;

    void Dump() const;

    // static methods
    static void itLimit(int& it);
    static void TLimit(int& T);

    // static constants
    /// magic constants
    static const std::vector<long long> k_np;

    /// limits
    static constexpr int it_h = 191;
    static constexpr int it_l = 0;
    static constexpr int T_h = 2047;
    static constexpr int T_l = -2048;
    static constexpr long long tooLargeAmp = 262015;

 public:
    eclDspEmulator(const std::string& fname);

    const dspResult& fit(int ichannel, int trigger_time, const std::vector<int>& adc_data);
    const dspResult& result() const {return m_dspr;}
};

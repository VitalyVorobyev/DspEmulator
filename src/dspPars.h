#pragma once

class dspCoeffs;

class dspPars {
 public:
    int A0;
    int Askip;
    int Ahard;
    const int k_a;
    const int k_b;
    const int k_c;
    const int k_16;
    const int k1_chi;
    const int k2_chi;
    const int kz_s;
    const int chi_thres;

    void check() const;

 public:
    // constructor
    dspPars(const dspCoeffs& dspc);
    void update(const dspCoeffs& dspc, int ch);
    // print to std output
    void Print() const;
};

#include "dspPars.h"

#include <iostream>
#include <exception>
#include <string>

#include "dspCoeffs.h"

using std::cout;
using std::cerr;
using std::endl;
using std::runtime_error;
using std::to_string;
using std::string;

constexpr bool dump = false;

dspPars::dspPars(const dspCoeffs& dspc) :
    A0(0), Askip(0), Ahard(0),
    k_a(static_cast<int>(dspc.id_uch(26))),
    k_b(static_cast<int>(dspc.id_uch(27))),
    k_c(static_cast<int>(dspc.id_uch(28))),
    k_16(static_cast<int>(dspc.id_uch(29))),
    k1_chi(static_cast<int>(dspc.id_uch(32))),
    k2_chi(static_cast<int>(dspc.id_uch(33))),
    kz_s(static_cast<int>(dspc.id_uch(34))),
    chi_thres(static_cast<int>(dspc.id(15))) {
    check();
}

void dspPars::check() const {
    if (dump)
        cout << "ka: " << k_a
             << "  kb: " << k_b
             << "  kc: " << k_c
             << "  k1_chi: " << k1_chi
             << "  k2_chi: " << k2_chi
             << "  k16: " << k_16
             << "  kzs: " << kz_s
             << "  chi_thres: " << chi_thres
             << endl;
    if (k_a < 13 || k_a > 16) {
        const string mess = "ka out of range " + to_string(k_a);
        cerr << mess << endl;
        throw new runtime_error(mess);
    }
    if (k_b < 13 || k_b > 16) {
        cerr << "kb out of range " << k_b<< endl;
        throw new runtime_error("kb out of range " + to_string(k_b));
    }
    if (k_c < 16 || k_c > 19) {
        cerr << "kc out of range " << k_c << endl;
        throw new runtime_error("kc out of range " + to_string(k_c));
    }
    if (k1_chi < 12 || k1_chi > 15) {
        cerr << "k1_chi out of range " << k1_chi << endl;
        throw new runtime_error("k1_chi out of range " + to_string(k1_chi));
    }
    if (k2_chi < 10 || k2_chi > 13) {
        cerr << "k2_chi out of range " << k2_chi << endl;
        throw new runtime_error("k2_chi out of range " + to_string(k2_chi));
    }
}

void dspPars::update(const dspCoeffs& dspc, int ch) {
    A0 = static_cast<int>(dspc.id(128 + ch) - 128);
    Askip = static_cast<int>(dspc.id(192 + ch) - 128);
    Ahard = static_cast<int>(dspc.id(64 + ch));
//    Askip = 40;
}

void dspPars::Print() const {
    cout << "k_a    = " << k_a << endl
         << "  k_b    = " << k_b << endl
         << "  k_c    = " << k_c << endl
         << "  k_16   = " << k_16 << endl
         << "  k1_chi = " << k1_chi << endl
         << "  k2_chi = " << k2_chi << endl
         << "  A0     = " << A0 << endl
         << "  Askip  = " << Askip << endl
         << "  Ahard  = " << Ahard << endl;
}

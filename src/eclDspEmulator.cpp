# include "eclDspEmulator.h"

#include <iostream>
#include <algorithm>  // max
#include <numeric>  // accumulate, inner_product
#include <exception>
#include <iomanip>  // setw

using std::cout;
using std::endl;
using std::make_unique;
using std::max;
using std::accumulate;
using std::inner_product;
using std::runtime_error;
using std::setw;

constexpr bool dump = false;

void dspResult::print() const {
    cout << " dspResult: "
         << " mi5 " << mi5
         << " lcp " << lcp
         << " lar " << lar
         << " ltr " << ltr
         << " lch3 " << lch3
         << " lch4 " << lch4
         << " lq " << lq
         << endl;
}

const std::vector<long long> eclDspEmulator::k_np = {
    65536, 32768, 21845, 16384, 13107, 10923, 9362, 8192,
    7282, 6554, 5958, 5461, 5041, 4681, 4369, 4096
};

eclDspEmulator::eclDspEmulator(const std::string& fname) :
    dspc(make_unique<dspCoeffs>(fname)),
    dspp(make_unique<dspPars>(*dspc)),
    A1(0), B1(0), C1(0), z0(0),
    validity_code(0), z00(0), T(0), ttrig(0) {}

const dspResult& eclDspEmulator::fit(int ichannel, int trigger_time,
                         const std::vector<int>& adc_data) {
    m_data.assign(adc_data.cbegin(), adc_data.cend());
    lftda(trigger_time, ichannel, 16);
    return m_dspr;
}

void eclDspEmulator::itLimit(int& itt) {
    itt = itt > it_h ? it_h : itt;
    itt = itt < it_l ? it_l : itt;
}

void eclDspEmulator::TLimit(int& T) {
    T = T > T_h ? T_h : T;
    T = T < T_l ? T_l : T;
}

void eclDspEmulator::ou(long long ch1, int it) {
    m_dspr.mi5 = it;
    m_dspr.lar = A1;
    m_dspr.ltr = T;
    int ss = static_cast<int>(m_data[20] + m_data[21]);
    if (ss <= dspp->Ahard) validity_code += 4;
    m_dspr.lq = validity_code;
    m_dspr.lch4 = ch1;
}

void eclDspEmulator::lowAmpAlg(int ch) {
    A1 = m_dspr.lch3;
    if (A1 < -128) {
        validity_code = 1;
        A1 = -128;
        ou(0);
        return;
    }
    validity_code = 2;
    B1 = 0;
    C1 = inner_product(m_data.cbegin()+16, m_data.cbegin()+31, dspc->fg43i(ch, 1 + ttrig * 16),
                       dspc->lfg43(ch, ttrig * 16) * z0 + (1 << (dspp->k_c - 1))) >> dspp->k_c;
}

void eclDspEmulator::largeAmpAlg() {
    if (dump) {
        cout << " == largeAmpAlg == " << endl;
        Dump();
    }
    A1 = m_dspr.lch3 >> 3;
    if (A1 > tooLargeAmp) A1 = A1 >> 3;
    validity_code = 1;
    A1 = A1 - 112;
    m_dspr.lch3 = A1;
    ou(0);
}

void eclDspEmulator::fin(int ch, int n16, int it) {
    long long ch2 = z00 - n16 * C1;
    long long ch1 = (ch2 * ch2 * k_np[n16 - 1]) >> 16;
    for (int i = 1; i < 16; i++) {
        ch2 = A1 * dspc->lf(ch, i + it*16) + B1 * dspc->lf1(ch, i + it*16);
        ch2 >>= dspp->k1_chi;
        ch2 = m_data[i + 15] - ch2 - C1;
        ch1 += ch2 * ch2;
    }
    if (validity_code == 2) {
        long long B5 = ch1;
        for (int i = 0; i < 16; i++) {
            if (B5 > 511) {
                B5 = B5 >> 2;
                T = i + 1;
            }
        }
        T = B5 | (T << 9);
        TLimit(T);
    }
    long long B2 = (A1>>1) * (A1>>1);
    B2 >>= dspp->k2_chi - 2;
    B2 += dspp->chi_thres;
    if ((ch1 > B2) && (validity_code != 2)) validity_code = 3;
    if ((C1 < 0) && (validity_code == 0)) validity_code = 3;
    ou(ch1);
}

void eclDspEmulator::iterAlg(int ch, int ttrig2, int n16) {
    validity_code = 0;
    int it0 = 48 + ((23 - ttrig) << 2);
    if (dump) cout << "it0 " << it0 << endl;
    itLimit(it0);

    for (int iter = 1, it = it0; iter < 4; iter++) {
        A1 = inner_product(m_data.cbegin()+16, m_data.cbegin()+31, dspc->fg31i(ch, 1 + it * 16),
                           dspc->lfg31(ch, it * 16) * z0 + (1 << (dspp->k_a - 1))) >> dspp->k_a;
        B1 = inner_product(m_data.cbegin()+16, m_data.cbegin()+31, dspc->fg32i(ch, 1 + it * 16),
                           dspc->lfg32(ch, it * 16) * z0);

        if (dump) {
            cout << " == iter " << iter << ", it " << it << endl;
            Dump();
        }

        if (A1 < -128) {
            validity_code = 1;
            m_dspr.lcp = A1;
            A1 = -128;
            if (m_dspr.lch3 > 0) {
                validity_code = 2;
                A1 = m_dspr.lch3;
            }
            ou(0, it);
            return;
        }

        if (A1 > tooLargeAmp) {
            validity_code = 1;
            A1 = A1 >> 3;
            if (A1 > tooLargeAmp) A1 = A1 >> 3;
            A1 = A1 - 112;
            m_dspr.lcp = A1;
            ou(0, it);
            return;
        }

        if (A1 < dspp->A0) {
            it = it0;
            lowAmpAlg(ch);
            fin(ch, n16, it);
            return;
        }
        long long B2 = 0;
        long long B3 = 0;
        C1 = 0;
        if (iter != 3) {
            B2 = B1 >> (dspp->k_b - 9);
            B1 = B2 >> 9;
            B2 += A1 << 9;
            B3 = B2 / A1;
            it +=((B3 + 1) >> 1) - 256;
            itLimit(it);
            if (dump) Dump();
        } else {
            B2 = B1 >> (dspp->k_b - 13);
            long long B5 = B1 >> (dspp->k_b - 9);
            B1 = B2 >> 13;
            B2 += A1 << 13;
            B3 = B2 / A1;

            if (dump) {
                cout << " B1 " << B1 << ", B2 " << B2
                     << ", B3 " << B3 << ", B5 " << B5
                     << ", it " << it << ", ttrig2 " << ttrig2
                     << ", T " << T << endl;
            }

            T = (it << 3) + (it << 2) + (((B3 >> 1) + B3 + 2) >> 2) - 3072;
            T = ((210 - ttrig2) << 3) - T;
            TLimit(T);

            m_dspr.lcp = T;
            B1 = B5 >> 9;
            B5 += (A1 << 9);
            B3 = B5 / A1;
            it += ((B3 + 1) >> 1 ) - 256;
            itLimit(it);

            if (dump) {
                cout << " B1 " << B1 << ", B2 " << B2
                     << ", B3 " << B3 << ", B5 " << B5
                     << ", it " << it << ", ttrig2 " << ttrig2
                     << ", T " << T << endl;
            }

            C1 = inner_product(m_data.cbegin()+16, m_data.cbegin()+31, dspc->fg33i(ch, it * 16 + 1),
                               dspc->lfg33(ch, it * 16) * z0) + (1 << (dspp->k_c - 1));
            C1 >>= dspp->k_c;
            if (dump) cout << " ==== C1 " << C1 << " ==== " << endl;
            if (dump) Dump();
        }
    }  // for (iter...)
}

void eclDspEmulator::Dump() const {
    cout << "  A1 " << setw(10) << A1
         << "  B1 " << setw(10) << B1
         << "  C1 " << setw(3) << C1
         << "  z0 " << z0
         << "  vc " << validity_code
         << "  z00 " << setw(5) << z00
         << "  T " << setw(4) << T
         << "  ttrig " << ttrig
         << endl;
}

void eclDspEmulator::lftda(int ttrig2, int ch, int n16) {
    dspp->update(*dspc, --ch);
    ttrig2 = ttrig2 - 2 * (ttrig2 / 8);
    ttrig = max(0, ttrig2 / 6);

    try {
    if (dspp->k_16 + n16 != 16)
        throw new runtime_error("disagreement in number of the points");
    } catch (const runtime_error& e) {
        cout << e.what() << endl;
    }
  
    //initial time index
    z00 = accumulate(m_data.cbegin()+dspp->k_16, m_data.cbegin()+dspp->k_16+16, 0);
    z0 = z00 >> dspp->kz_s;

    // first approximation without time correction
    m_dspr.lch3 = inner_product(m_data.cbegin()+16, m_data.cbegin()+31, dspc->fg41i(ch, ttrig * 16 + 1),
                                dspc->lfg41(ch, ttrig * 16) * z0 + (1 << (dspp->k_a - 1))) >> dspp->k_a;
    if (dump) {
//        cout << "f41 " << dspc->lfg41(ch, ttrig * 16) << ", z0 " << z0 << ", asd " << (1 << (dspp->k_a - 1)) << endl;
        cout << "lch3 init " << dspc->lfg41(ch, ttrig * 16) * z0 + (1 << (dspp->k_a - 1)) << endl;
        cout << "First appr " << m_dspr.lch3 << endl;
    }

    // too large amplitude
    if (m_dspr.lch3 > tooLargeAmp) {
        largeAmpAlg();
        return;
    }

    if (m_dspr.lch3 >= dspp->A0)
        iterAlg(ch, ttrig2, n16);
    else
        lowAmpAlg(ch);

    if (dump) Dump();
    fin(ch, n16);
}

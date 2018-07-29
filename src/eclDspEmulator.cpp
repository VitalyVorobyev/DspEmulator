# include "eclDspEmulator.h"

#include <iostream>

using std::cout;
using std::endl;
using std::make_unique;

constexpr bool dump = false;

const std::vector<long long> eclDspEmulator::k_np = {
    65536, 32768, 21845, 16384, 13107, 10923, 9362, 8192,
    7282, 6554, 5958, 5461, 5041, 4681, 4369, 4096
};

eclDspEmulator::eclDspEmulator(const std::string& fname) :
    dspc(make_unique<dspCoeffs>(fname)),
    dspp(make_unique<dspPars>(*dspc)),
    A1(0), A2(0), B1(0), C1(0), ch1(0), z0(0),
    validity_code(0), z00(0), T(0), it(0), ttrig(0) {}

const dspResult& eclDspEmulator::fit(int ichannel, int trigger_time,
                         const std::vector<int>& adc_data) {
    lftda(&adc_data[0], trigger_time, ichannel, 16);
    return m_dspr;
}

void eclDspEmulator::ou(const int *y) {
    m_dspr.mi5 = it;
    m_dspr.lar = A1;
    m_dspr.ltr = T;
    int ss = y[20] + y[21];
    if (ss <= dspp->Ahard) validity_code += 4;
    m_dspr.lq = validity_code;
    m_dspr.lch4 = ch1;
}

void eclDspEmulator::lam(const int *y, int ch) {
    A1 = A2;
    if (A1 < -128) {
        validity_code = 1;
        A1 = -128;
        ou(y);
        return;
    }
    validity_code = 2;
    B1 = 0;
    C1 = dspc->fg43(ch, ttrig * 16) * z0;
    //***************************************
    for (int i = 1; i < 16; i++) {
        C1 += dspc->fg43(ch, i + ttrig * 16) * y[15 + i];
    }
    C1 += (1 << (dspp->k_c - 1));
    C1 >>= dspp->k_c;
}

void eclDspEmulator::fin(const int *y, int ch, int n16) {
    long long ch2 = z00 - n16 * C1;
    ch1 = ch2 * ch2;
    ch1 = ch1 * k_np[n16 - 1];
    ch1 = ch1 >> 16;
    for (int i = 1; i < 16; i++) {
        ch2 = A1 * dspc->f(ch, i + it*16) + B1 * dspc->f1(ch, i + it*16);
        ch2 >>= dspp->k1_chi;
        ch2 = y[i + 15] - ch2 - C1;
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
        T = B5 | (T<<9);
        if (T > 2047)
            T = T - 4096 * ((T + 2048)/4096);
        if (T < -2048)
            T = T + 4096 * ((-T + 2048)/4096);
    }
    long long B2 = (A1>>1) * (A1>>1);
    B2 >>= dspp->k2_chi - 2;
    B2 += dspp->chi_thres;
    if ((ch1 > B2) && (validity_code != 2)) validity_code = 3;
    if ((C1 < 0) && (validity_code == 0)) validity_code = 3;
    ou(y);
}

void eclDspEmulator::lftda(const int *y, int ttrig2, int ch, int n16) {
    dspp->update(*dspc, --ch);
    if (dump) dspp->Print();
    ttrig2 = ttrig2 - 2 * (ttrig2 / 8);
    ttrig = ttrig2 / 6;
//    cout << ttrig <<
    if (ttrig < 0) ttrig = 0;
    if (dspp->k_16 + n16 != 16)
      cout << "disagreement in number of the points "
           << dspp->k_16 << " " << n16 << endl;
  
    int validity_code = 0;
    long long int z00 = 0;
    for (int i = dspp->k_16; i < 16; i++) z00 += y[i];

    //initial time index
    z0 = z00 >> dspp->kz_s;

    //***************************************
    int it0 = 48 + ((23 - ttrig) << 2);
    if (it0 < it_l) it0 = it_l; 
    else if (it0 > it_h) it0 = it_h; 
    it = it0;

    // first approximation without time correction
    int s1 = dspc->fg41(ch, ttrig * 16);
    long long A2 = (s1 * z0);
    //***************************************
    if(dump) cout << " lftda 0 " << z0 << " " << s1 << " " << A2 << endl;

    long long B3 = 0;
    for (int i = 1; i < 16; i++) {
        s1 = dspc->fg41(ch, ttrig * 16 + i);
        B3 = y[15 + i];
        B3 = s1 * B3;
        A2 += B3;
    }
    A2 += (1 << (dspp->k_a - 1));
    A2 >>= dspp->k_a;

    m_dspr.lch3 = A2;
    // too large amplitude
    if (A2 > 262015) {
        A1 = A2 >> 3;
        if (A1 > 262015) A1 = A1 >> 3;
        validity_code = 1;
        A1 = A1 - 112;
        if (dump) cout << A1 << " 2" << endl;
        m_dspr.lch3 = A1;
        ou(y);
        return;
    }

    if (A2 >= dspp->A0) {
        for (int iter = 1, it = it0; iter < 4; iter++) {
            s1 = dspc->fg31(ch, it * 16);
            int s2 = dspc->fg32(ch, it * 16);
            A1 = (s1 * z0);
            B1 = (s2 * z0);
            //***************************************
            if (dump) {
                cout << " lftda 0 " << z0 << " " << s1 << " " << s2 << " "
                     << A1 << " " << B1 << endl;
                cout << "v1 lftda " << iter << endl;
            }
            for (int i = 1; i < 16; i++) {
                s1 = dspc->fg31(ch, i + it * 16);
                s2 = dspc->fg32(ch, i + it * 16);
                A1 += s1 * y[15 + i];
                B3 = y[15 + i];
                B3 = s2 * B3;
                B1 += B3;
            }
            A1 += 1 << (dspp->k_a - 1);
            A1 = A1 >> dspp->k_a;

            if (A1 < -128) {
                validity_code = 1;
                m_dspr.lcp = A1;
                A1 = -128;
                if (m_dspr.lch3 > 0) {
                    validity_code=2;
                    A1 = m_dspr.lch3;
                }
                ou(y);
                return;
            }

            if (A1 > 262015) {
                validity_code = 1;
                A1 = A1 >> 3;
                if (A1 > 262015) A1 = A1 >> 3;
                A1 = A1 - 112;
                m_dspr.lcp = A1;
                if (dump) cout << A1 << " 1" << endl;
                ou(y);
                return;
            }

            if (A1 < dspp->A0) {
                it = it0;
                lam(y, ch);
                fin(y, ch, n16);
                return;
            }
            long long B2 = 0;
            long long C1 = 0;
            if (iter != 3) {
                B2 = B1 >> (dspp->k_b - 9);
                B1 = B2 >> 9;
                B2 += (A1 << 9);
                B3 = (B2 / A1);
                if (dump) cout << "v1 lftda3 " << ch << " " << it << " " << A1
                               << " " << B1 << " " << z0 << " " << ttrig << endl;
                it +=((B3+1)>>1) - 256;
                it = it > it_h ? it_h : it;
                it = it < it_l ? it_l : it;
            } else {
                B2 = B1 >> (dspp->k_b - 13);
                long long B5 = B1 >> (dspp->k_b - 9);
                B1 = B2 >> 13;
                B2 += A1 << 13;
                B3 = B2 / A1;

                T = (it << 3) + (it << 2) + (((B3 >> 1) + B3 + 2) >> 2) - 3072;
                T = ((210 - ttrig2) << 3) - T;

                m_dspr.lcp = T;
                B1 = B5 >> 9;
                B5 += (A1 << 9);
                B3 = B5 / A1;
                it += ((B3 + 1) >>1 ) - 256;

                it = it>it_h ? it_h : it; 
                it = it<it_l ? it_l : it;

                T = T > 2047 ?  2047 : T;

                T = T < -2048 ? -2048 : T;
                C1 = dspc->fg33(ch, it * 16) * z0;
                //***************************************
                for (int i = 1; i < 16; i++)
                    C1 += dspc->fg33(ch, i + it * 16) * y[15 + i];
                C1 += (1 << (dspp->k_c - 1));
                C1 >>= dspp->k_c;
                if (dump)
                    cout << "v1 lftda4 " << ch << " " << it << " " << A1
                         << " " << T << " " << C1 << endl;
            }
        }  // for (iter...)
    } else {  // if(A2 > A0)
        lam(y, ch);
    }
    fin(y, ch, n16);
}

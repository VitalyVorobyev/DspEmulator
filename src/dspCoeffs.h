#pragma once

#include <string>
#include <cstdint>
#include <vector>

//using DspArray = std::vector<std::vector<std::vector<int64_t>>>;
using DspArray = std::vector<std::vector<int64_t>>;
using DspCIter = std::vector<int64_t>::const_iterator;

class dspCoeffs {
    int init(const std::string& fname);

    short m_id[256];
    // coefficient matrices for all channels in the board
    short int m_f[16][192][16];  // very bad name
    short int m_f1[16][192][16]; // very bad name
            
    short int m_fg41[16][24][16];
    short int m_fg43[16][24][16];

    short int m_fg31[16][192][16];
    short int m_fg32[16][192][16];
    short int m_fg33[16][192][16];

    std::vector<int64_t> m_lid;
    // coefficient matrices for all channels in the board
    DspArray m_lf;  // still very bad name
    DspArray m_lf1; // still very bad name

    DspArray m_lfg41;
    DspArray m_lfg43;

    DspArray m_lfg31;
    DspArray m_lfg32;
    DspArray m_lfg33;

    void fillDspArrays() noexcept;

    // eclDspEmulator processes data from single shaperDSP board
    static constexpr int m_numberOfChannels = 16;

 public:
    dspCoeffs(const std::string& fname);

    short int id(int n) const {return *(&m_id[0] + n);}
    int id_uch(int n) const {return (int) * ((unsigned char*)m_id + n);}
    short int f(int ch, int n) const {return *(&m_f[ch][0][0] + n);}
    short int f1(int ch, int n) const {return *(&m_f1[ch][0][0] + n);}
    short int fg41(int ch, int n) const {return *(&m_fg41[ch][0][0] + n);}
    short int fg43(int ch, int n) const {return *(&m_fg43[ch][0][0] + n);}
    short int fg31(int ch, int n) const {return *(&m_fg31[ch][0][0] + n);}
    short int fg32(int ch, int n) const {return *(&m_fg32[ch][0][0] + n);}
    short int fg33(int ch, int n) const {return *(&m_fg33[ch][0][0] + n);}

    int64_t lf(uint16_t ch, uint16_t n) const {return m_lf[ch][n];}
    int64_t lf1(uint16_t ch, uint16_t n) const {return m_lf1[ch][n];}
    int64_t lfg41(uint16_t ch, uint16_t n) const {return m_lfg41[ch][n];}
    int64_t lfg43(uint16_t ch, uint16_t n) const {return m_lfg43[ch][n];}
    int64_t lfg31(uint16_t ch, uint16_t n) const {return m_lfg31[ch][n];}
    int64_t lfg32(uint16_t ch, uint16_t n) const {return m_lfg32[ch][n];}
    int64_t lfg33(uint16_t ch, uint16_t n) const {return m_lfg33[ch][n];}

    DspCIter fi(uint16_t ch, uint16_t n) const {return m_lf[ch].cbegin() + n;}
    DspCIter f1i(uint16_t ch, uint16_t n) const {return m_lf1[ch].cbegin() + n;}
    DspCIter fg31i(uint16_t ch, uint16_t n) const {return m_lfg31[ch].cbegin() + n;}
    DspCIter fg32i(uint16_t ch, uint16_t n) const {return m_lfg32[ch].cbegin() + n;}
    DspCIter fg33i(uint16_t ch, uint16_t n) const {return m_lfg33[ch].cbegin() + n;}
    DspCIter fg41i(uint16_t ch, uint16_t n) const {return m_lfg41[ch].cbegin() + n;}
    DspCIter fg43i(uint16_t ch, uint16_t n) const {return m_lfg43[ch].cbegin() + n;}
};

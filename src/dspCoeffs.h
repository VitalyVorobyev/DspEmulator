#pragma once

#include <string>

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
};

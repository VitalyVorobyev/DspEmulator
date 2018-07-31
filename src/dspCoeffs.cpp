#include "dspCoeffs.h"

#include <iostream>
#include <exception>
#include <cstdio>
#include <iterator>

using std::cout;
using std::endl;
using std::to_string;
using std::begin;
using std::end;
using std::runtime_error;

dspCoeffs::dspCoeffs(const std::string& fname) :
    m_lid(256), m_lf(m_numberOfChannels), m_lf1(m_numberOfChannels),
    m_lfg41(m_numberOfChannels), m_lfg43(m_numberOfChannels),
    m_lfg31(m_numberOfChannels), m_lfg32(m_numberOfChannels),
    m_lfg33(m_numberOfChannels) {
    init(fname);
    fillDspArrays();
}

void dspCoeffs::fillDspArrays() noexcept {
    m_lid.assign(begin(m_id), end(m_id));
    const uint32_t size1 = 192 * 16;
    const uint32_t size2 = 24 * 16;
    for (uint16_t i = 0; i < m_numberOfChannels; i++) {
        m_lf[i].assign(m_f[i][0], m_f[i][0] + size1);
        m_lf1[i].assign(m_f1[i][0], m_f1[i][0] + size1);
        m_lfg31[i].assign(m_fg31[i][0], m_fg31[i][0] + size1);
        m_lfg32[i].assign(m_fg32[i][0], m_fg32[i][0] + size1);
        m_lfg33[i].assign(m_fg33[i][0], m_fg33[i][0] + size1);
        m_lfg41[i].assign(m_fg41[i][0], m_fg41[i][0] + size2);
        m_lfg43[i].assign(m_fg43[i][0], m_fg43[i][0] + size2);
    }
}

int dspCoeffs::init(const std::string& fname) {
    // open file "filename" with dsp data
    FILE *inifile = fopen(fname.c_str(), "rb");

    try {
    if (inifile == NULL)
        throw new runtime_error("eclDspEmulator::ERROR:: can't open initialization file " + fname);

    const int nsiz = 2;  // sizeof short
    int data_size = 256; // size of block with algorithm parameters

    int ret = fread(m_id, nsiz, data_size, inifile);
    if (ret != data_size)
        throw new runtime_error("Error reading id data. Read block size " + to_string(ret));

    for (int i = 0; i < m_numberOfChannels; i++) {
        // fill FG41
        data_size = 384;
        ret = fread(m_fg41[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new runtime_error("Error reading fg41 data. Read block size " + to_string(ret));

        // fill FG31
        data_size = 3072;
        ret = fread(m_fg31[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new runtime_error("Error reading fg31 data. Read block size " + to_string(ret));

        // fill FG32
        ret = fread(m_fg32[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new runtime_error("Error reading fg32 data. Read block size " + to_string(ret));

        // fill FG33
        ret = fread(m_fg33[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new runtime_error("Error reading fg33 data. Read block size " + to_string(ret));

        // fill FG43
        data_size = 384; 
        ret = fread(m_fg43[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new runtime_error("Error reading fg43 data. Read block size " + to_string(ret));

        // fill F
        data_size = 3072;
        ret = fread(m_f[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new runtime_error("Error reading f data. Read block size " + to_string(ret));

        // fill F1
        ret = fread(m_f1[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new runtime_error("Error reading f1 data. Read block size " + to_string(ret));
    }
    } catch (const runtime_error& e) {
        cout << e.what() << endl;
    }
    fclose(inifile);
    return 0;
}

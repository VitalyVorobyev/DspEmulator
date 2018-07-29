#include "dspCoeffs.h"

#include <iostream>
#include <exception>
#include <cstdio>

using std::cout;
using std::endl;
using std::to_string;

dspCoeffs::dspCoeffs(const std::string& fname) {
    init(fname);
}

int dspCoeffs::init(const std::string& fname) {
    // open file "filename" with dsp data
    FILE *inifile = fopen(fname.c_str(), "rb");
    if (inifile == NULL)
        throw new std::runtime_error("eclDspEmulator::ERROR:: can't open initialization file " + fname);

    const int nsiz = 2;  // sizeof short
    int data_size = 256; // size of block with algorithm parameters

    int ret = fread(m_id, nsiz, data_size, inifile);
    if (ret != data_size)
        throw new std::runtime_error("Error reading id data. Read block size " + to_string(ret));

    for (int i = 0; i < m_numberOfChannels; i++) {
        // fill FG41
        data_size = 384;
        ret = fread(m_fg41[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new std::runtime_error("Error reading fg41 data. Read block size " + to_string(ret));

        // fill FG31
        data_size = 3072;
        ret = fread(m_fg31[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new std::runtime_error("Error reading fg31 data. Read block size " + to_string(ret));

        // fill FG32
        ret = fread(m_fg32[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new std::runtime_error("Error reading fg32 data. Read block size " + to_string(ret));

        // fill FG33
        ret = fread(m_fg33[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new std::runtime_error("Error reading fg33 data. Read block size " + to_string(ret));

        // fill FG43
        data_size = 384; 
        ret = fread(m_fg43[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new std::runtime_error("Error reading fg43 data. Read block size " + to_string(ret));

        // fill F
        data_size = 3072;
        ret = fread(m_f[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new std::runtime_error("Error reading f data. Read block size " + to_string(ret));

        // fill F1
        ret = fread(m_f1[i][0], nsiz, data_size, inifile);
        if (ret != data_size)
            throw new std::runtime_error("Error reading f1 data. Read block size " + to_string(ret));
    }
    fclose(inifile);
    return 0;
}

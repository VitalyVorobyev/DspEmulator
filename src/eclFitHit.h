#pragma once

#include <vector>
#include <TObject.h>

/**
 * Class fot SDSP fit info
 * */
class eclFitHit {  // : public TObject {
 public:
    int evt;
    int cid;
    int amp;
    int tim;
    int qua;
    int chi;
    int trg;
    int cra;
    int shp;
    int chn;
    std::vector<int> adc;

    /** Constructor */
    eclFitHit() : evt(0), cid(0), amp(0), tim(0), qua(0), chi(0),
        trg(0), cra(0), shp(0), chn(0), adc(64, 0) {}

    /** Print to stdout */
    void Print() const;
    ClassDef(eclFitHit, 1)  // header
};


#include "eclSDSPTester.h"

#include "eclFitHit.h"

#include <iostream>
#include <stdexcept>
#include <numeric>

#include <TTree.h>
#include <TFile.h>
#include <TChain.h>

constexpr bool dump = false;

using std::string;
using std::to_string;
using std::cout;
using std::endl;
using std::cerr;
using std::make_unique;
using std::unique_ptr;
using std::vector;
using std::tuple;
using std::make_tuple;
using std::move;
using std::get;
using std::accumulate;

constexpr uint16_t copNum = 26;
constexpr uint16_t shpNum = 24;

eclSDSPTester::eclSDSPTester(const std::string& dspPath, int ampThres) :
    m_refit(copNum * shpNum), dspdat_path(dspPath), m_ampThres(ampThres) {
    init();
}

inline uint16_t cidIdx(uint16_t i, uint16_t j) {return i * shpNum + j;}

uint16_t crateIdx(uint16_t j) {
    return j > 11 ? j - 11 : j + 1;
}

uint16_t shaperIdx(uint16_t j, uint16_t i) {
    if (j > 11) {return i > 17 ? i - 18 + 45 : 2 * i + 2;}
                 return i > 17 ? i - 18 + 37 : 2 * i + 1;
}

uint16_t copperIdx(uint16_t cr) {
    if (cr < 36) return cr / 2;
    else if (cr < 44) return cr - 18;
    return cr - 26;
}

string eclSDSPTester::dspdat(uint16_t i, uint16_t j)  const {
    const string jstr = j < 10 ? "0" + to_string(j) : to_string(j);
    return dspdat_path + to_string(i+1) + "/" + to_string(j) + "/dsp"
           + jstr + ".dat";
}

void eclSDSPTester::init() {
    for (uint16_t i = 0; i < copNum; i++) {
        const int sted = i < 18 ? shpNum : 21;
        for (uint16_t j = 0; j < sted; j++) {
            const auto idx = cidIdx(i, j);
            const auto ifile = dspdat(i, j);
            m_refit[idx] = make_unique<eclDspEmulator>(ifile);
        }
    }
}

eclSDSPTester::ITuple eclSDSPTester::getTree(const string& ifname, const string& itname) const {
    auto itree = make_unique<TChain>(itname.c_str());
    itree->Add(ifname.c_str());
    auto data = make_unique<eclFitHit>();
    itree->SetBranchAddress("evt", &data->evt);
    itree->SetBranchAddress("cid", &data->cid);
    itree->SetBranchAddress("amp", &data->amp);
    itree->SetBranchAddress("tim", &data->tim);
    itree->SetBranchAddress("qua", &data->qua);
    itree->SetBranchAddress("chi", &data->chi);
    itree->SetBranchAddress("trg", &data->trg);
    itree->SetBranchAddress("cra", &data->cra);
    itree->SetBranchAddress("shp", &data->shp);
    itree->SetBranchAddress("chn", &data->chn);
    itree->SetBranchAddress("adc", &data->adc[0]);
    return make_tuple(move(itree), move(data));
}

eclSDSPTester::OTuple eclSDSPTester::makeTree(const std::string& ofname,
                               const std::string& otname) const {
    auto ofile = make_unique<TFile>(ofname.c_str(), "RECREATE");
    auto otree = make_unique<TTree>(otname.c_str(), otname.c_str());
    auto data = make_unique<eclFitHit>();
    otree->Branch("hits", data.get());
    return make_tuple(move(ofile), move(otree), move(data));
}

void eclSDSPTester::refit(const std::string& ifname, const std::string& itname,
                          const std::string& ofname, const std::string& otname,
                          bool verb) const {
    auto itup = getTree(ifname, itname);
    auto otup = makeTree(ofname, otname);

    auto itree = move(get<0>(itup));
    auto idata = move(get<1>(itup));

    auto ofile = move(get<0>(otup));
    auto otree = move(get<1>(otup));
    auto odata = move(get<2>(otup));

    int cr, sp, cop, flag;
    const auto nevt = itree->GetEntries();
    int badEvtCnt = 0;

    for (int i = 0; i < nevt; i++) {
        itree->GetEntry(i);
        if (i%100000 == 0) cout << " event " << i << endl;
        if (idata->amp < m_ampThres) continue;
        cr = idata->cra - 1;
        sp = idata->shp - 1;

    	// if the second crate in copper
        if ((cr > 43) || (cr%2 && cr<36)) sp += 12;
        cop = copperIdx(cr);
        if (dump) cout << "cop " << cop << ", sp " << sp << ", ch " << idata->chn << endl;
        auto& fitter = *m_refit[cidIdx(cop, sp)];

        flag = accumulate(idata->adc.cbegin(), idata->adc.cbegin() + 16, 0) / 16;
        if (flag > 0) {
            const auto& fitres = fitter.fit(idata->chn, idata->trg, idata->adc);
            if (idata->amp != fitres.lar || idata->tim != fitres.ltr) {
                badEvtCnt++;
                if (!fitres.lq) cout << "Success" << endl;
                odata->evt = idata->evt;
                odata->cid = idata->cid;
                odata->trg = idata->trg;
                odata->chn = idata->chn;
                odata->cra = idata->cra;
                odata->shp = sp + 1;
                odata->amp = fitres.lar;
                odata->tim = fitres.ltr;
                odata->qua = fitres.lq;
                odata->chi = fitres.lch4;
                if (verb) {
                    cout << " ----- " << endl;
                    idata->Print();
                    odata->Print();
                }
                otree->Fill();
            }
        }
    }
    cout << "End of event loop " << badEvtCnt << "/" << nevt
         << " (" << static_cast<double>(badEvtCnt) / nevt << ")" << endl;
    ofile->cd();
    otree->Write();
}

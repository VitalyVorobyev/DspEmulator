#include "eclFitHit.h"

#include <iomanip>
#include <sstream>

ClassImp(eclFitHit)

using std::setw;
using std::string;
using std::stringstream;

string eclFitHit::toString() const {
    stringstream out;
    out << "evt=" << setw(5) << evt
        << "   cid=" << setw(4) << cid
        << "   amp=" << setw(6) << amp
        << "   tim=" << setw(5) << tim
        << "   cra=" << setw(2) << cra
        << "   shp=" << setw(2) << shp
        << "   chn=" << setw(2) << chn
        << "   trg=" << setw(3) << trg
        << "   qua=" << setw(1) << qua
        << "   chi=" << setw(10) << chi;
    return out.str();
}

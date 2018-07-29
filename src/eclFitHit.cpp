#include "eclFitHit.h"

#include <iostream>
#include <iomanip>

ClassImp(eclFitHit)

using std::cout;
using std::endl;
using std::setw;

void eclFitHit::Print() const {
    cout << "evt=" << setw(5) << evt
         << "   cid=" << setw(4) << cid
         << "   amp=" << setw(6) << amp
         << "   tim=" << setw(5) << tim
         << "   cra=" << setw(2) << cra
         << "   shp=" << setw(2) << shp
         << "   chn=" << setw(2) << chn
         << "   trg=" << setw(3) << trg
         << "   qua=" << setw(1) << qua
         << "   chi=" << setw(10) << chi
         << endl;
}

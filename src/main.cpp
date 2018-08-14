#include "eclSDSPTester.h"
#include "popt.h"

#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    POpt opt(argc, argv);

    eclSDSPTester tester(opt.dspPath());

    tester.refit(opt.ipath() + opt.ifile(), opt.itree(),
                 opt.opath() + opt.ofile(), opt.otree(), opt.verb());

    return 0;
}

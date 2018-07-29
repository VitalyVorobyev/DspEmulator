#include "eclSDSPTester.h"

int main(int argc, char** argv) {
    eclSDSPTester tester("/home/vitaly/ECL/adcfit/share/ecl_map_new.txt.180127");

    tester.refit(
        "/home/vitaly/AdcBits/upack_adc_physics.0003.05017.HLT2.f00000.root",
//        "/home/vitaly/AdcBits/upack_adc_physics.0003.05609.HLT4.f00003.root",
	    "tt",
	    "refit_physics.0003.05609.HLT4.f00003.root",
	    "yy",
        true // verbose
    );

    return 0;
}

#include <iostream>
#include "ossie/ossieSupport.h"

#include "ADSBdecoder_MR.h"
int main(int argc, char* argv[])
{
    ADSBdecoder_MR_i* ADSBdecoder_MR_servant;
    Component::start_component(ADSBdecoder_MR_servant, argc, argv);
    return 0;
}


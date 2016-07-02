#include <iostream>
#include "ClassicNestedSampler.h"
#include "Models/SpikeSlab.h"

int main()
{
    LiNeS::ClassicNestedSampler<SpikeSlab> sampler(100);
    sampler.do_iteration();
    return 0;
}


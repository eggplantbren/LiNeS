#include <iostream>
#include "ClassicNestedSampler.h"
#include "Models/SpikeSlab.h"

int main()
{
    LiNeS::ClassicNestedSampler<SpikeSlab> sampler(100);

    while(sampler.get_depth() < 100.0)
        sampler.do_iteration(1000);

    return 0;
}


#include <iostream>
#include "ClassicNestedSampler.h"
#include "Models/SpikeSlab.h"

int main()
{
    LiNeS::ClassicNestedSampler<SpikeSlab> sampler(100);

    for(int i=0; i<10000; ++i)
        sampler.do_iteration(10000);

    return 0;
}


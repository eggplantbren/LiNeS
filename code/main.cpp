#include <iostream>
#include "ClassicNestedSampler.h"
#include "Models/SpikeSlab.h"

int main()
{
    LiNeS::ClassicNestedSampler<SpikeSlab> sampler(100);
    sampler.initialise_particles();
    return 0;
}


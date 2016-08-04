#include "SimpleExample.h"
#include "DNest4/code/DNest4.h"

using namespace std;
using namespace DNest4;

SimpleExample::SimpleExample()
:params(100)
{

}

void SimpleExample::from_prior(RNG& rng)
{
    for(size_t i=0; i<params.size(); i++)
        params[i] = rng.rand();
}

double SimpleExample::perturb(RNG& rng)
{
    int which = rng.rand_int(params.size());
    params[which] += rng.randh();
    wrap(params[which], 0.0, 1.0);
    return 0.0;
}

std::vector<double> SimpleExample::get_scalars() const
{
    std::vector<double> scalars{0.0, 0.0};

    for(double x: params)
    {
        scalars[0] += -pow(x - 0.5, 2);
        scalars[1] += -std::abs(x);
    }

    return scalars;
}

void SimpleExample::print(std::ostream& out) const
{
    for(double x: params)
        out<<x<<' ';
}

string SimpleExample::description() const
{
    return string("");
}


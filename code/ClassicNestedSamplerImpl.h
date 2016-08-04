#include <stdexcept>
#include <limits>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "DNest4/code/Utils.h"

namespace LiNeS
{

template<class ModelType>
ClassicNestedSampler<ModelType>::
    ClassicNestedSampler(size_t num_particles, unsigned int seed)
:rng()
,num_particles(num_particles)
,iteration(-1)  // -1 = particles not initialised.
,particles(num_particles)
,scalars(num_particles,
                    std::vector<double>(ModelType::get_num_scalars()))
,tiebreakers(num_particles,
                    std::vector<double>(ModelType::get_num_scalars()))
,floor(ModelType::get_num_scalars(), -std::numeric_limits<double>::max())
,tb_floor(ModelType::get_num_scalars(), -std::numeric_limits<double>::max())
,selection_probs(ModelType::get_num_scalars())
,logger(num_particles, ModelType::get_num_scalars())
,verbosity(Verbosity::medium)
{
    if(num_particles == 0)
        throw std::domain_error("ERROR constructing ClassicNestedSampler:\
                                            num_particles can't be zero.");
    rng.set_seed(seed);

    // Generate selection probabilities for the scalars
    for(double& p: selection_probs)
        p = 5*rng.randn();
    // Normalise to max
    double max = *std::max_element(selection_probs.begin(),
                                                selection_probs.end());
    for(double& p: selection_probs)
        p /= max;
}

template<class ModelType>
void ClassicNestedSampler<ModelType>::initialise_particles()
{
    if(verbosity != Verbosity::low)
    {
        std::cout<<"# Generating "<<num_particles;
        std::cout<<" particles from the prior...";
        std::cout<<std::flush;
    }

    for(size_t i=0; i<num_particles; ++i)
    {
        particles[i].from_prior(rng);
        scalars[i] = particles[i].get_scalars();
        for(double& tb: tiebreakers[i])
            tb = rng.rand();
    }

    if(verbosity != Verbosity::low)
        std::cout<<"done."<<std::endl<<std::endl;

    // Set iteration to zero
    iteration = 0;
}

template<class ModelType>
std::tuple<size_t, size_t>
    ClassicNestedSampler<ModelType>::find_worst_particle()
{
    // Choose a scalar
    size_t which_scalar;
    do
    {
        which_scalar = rng.rand_int(ModelType::get_num_scalars());
    }while(rng.rand() > selection_probs[which_scalar]);

    size_t which_particle;

    double s = std::numeric_limits<double>::max();
    double tb = 1.0;
    bool success = false;

    for(size_t i=0; i<num_particles; ++i)
    {
        if(scalars[i][which_scalar] < s ||
                (scalars[i][which_scalar] == s &&
                    tiebreakers[i][which_scalar] < tb))
        {
            s = scalars[i][which_scalar];
            tb = tiebreakers[i][which_scalar];
            which_particle = i;
            success = true;
        }
    }
    if(!success)
        std::cerr<<"# WARNING in ClassicNestedSampler::find_worst_particle"<<
                    std::endl;

    return std::tuple<double, double>{which_scalar, which_particle};
}

template<class ModelType>
double ClassicNestedSampler<ModelType>::get_depth() const
{
    return iteration*(1.0/num_particles);
}

template<class ModelType>
void ClassicNestedSampler<ModelType>::run(double max_depth,
                                                    unsigned int mcmc_steps)
{
    while(get_depth() < max_depth)
        do_iteration(mcmc_steps);
}


template<class ModelType>
void ClassicNestedSampler<ModelType>::do_iteration(unsigned int mcmc_steps)
{
    // Initialise the particles if it hasn't been done already
    if(iteration == -1)
        initialise_particles();
    ++iteration;

    // Find and save worst particle
    auto t = find_worst_particle();
    size_t which_scalar, which_particle;
    std::tie(which_scalar, which_particle) = t;

    logger.log_particle(scalars[which_particle][which_scalar],
                            tiebreakers[which_particle][which_scalar],
                            which_scalar);
    floor[which_scalar] = scalars[which_particle][which_scalar];
    tb_floor[which_scalar] = tiebreakers[which_particle][which_scalar];    

    if(verbosity == Verbosity::high ||
        (verbosity == Verbosity::medium && iteration%num_particles == 0))
    {
        std::cout<<std::setprecision(12);
        std::cout<<"# Classic NS iteration "<<iteration<<". ";
        std::cout<<"log(X) = "<<iteration*(-1.0/num_particles)<<".\n";
        std::cout<<"# Floor = {";
        for(size_t i=0; i<floor.size(); ++i)
        {
            std::cout<<floor[i];
            if(i != (floor.size() - 1))
                std::cout<<", ";
        }
        std::cout<<"}.\n";
    }

    // Clone a survivor
    if(num_particles != 1)
    {
        size_t copy;
        do
        {
            copy = rng.rand_int(num_particles);
        }while(copy == which_particle);
        particles[which_particle] = particles[copy];
        scalars[which_particle] = scalars[copy];
        tiebreakers[which_particle] = tiebreakers[copy];
    }

    // Print messages
    if(verbosity == Verbosity::high ||
        (verbosity == Verbosity::medium && iteration%num_particles == 0))
        std::cout<<"# Generating new particle..."<<std::flush;

    // Do MCMC
    ModelType proposal;
    double logH;
    std::vector<double> s_proposal, tb_proposal;
    unsigned int accepts = 0;
    for(size_t i=0; i<mcmc_steps; ++i)
    {
        proposal = particles[which_particle];
        logH = proposal.perturb(rng);

        if(rng.rand() <= exp(logH))
        {
            s_proposal = proposal.get_scalars();

            tb_proposal = tiebreakers[which_particle];
            double& tb = tb_proposal[rng.rand_int(tb_proposal.size())];
            tb += rng.randh();
            DNest4::wrap(tb, 0.0, 1.0);

            if(is_okay(s_proposal, tb_proposal))
            {
                particles[which_particle] = proposal;
                scalars[which_particle] = s_proposal;
                tiebreakers[which_particle] = tb_proposal;
                ++accepts;
            }
        }
    }

    if(verbosity == Verbosity::high ||
        (verbosity == Verbosity::medium && iteration%num_particles == 0))
    {
        std::cout<<"done. Accepted "<<accepts<<"/"<<mcmc_steps<<'.';
        std::cout<<"\n#"<<std::endl;
    }
}

template<class ModelType>
bool ClassicNestedSampler<ModelType>::is_okay(std::vector<double> s_proposal,
                                   std::vector<double> tb_proposal) const
{
    for(size_t i=0; i<s_proposal.size(); ++i)
    {
        if(s_proposal[i] < floor[i])
            return false;
        if(s_proposal[i] == floor[i] && (tb_proposal[i] < tb_floor[i]))
            return false;
    }

    return true;
}

} // namespace LiNeS


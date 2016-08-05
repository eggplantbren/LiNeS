#include <fstream>

namespace LiNeS
{

template<class ModelType>
LNS<ModelType>::LNS(unsigned int run_id, const ClassicLogger& classic_logger,
                                DNest4::RNG rng)
:run_id(run_id)
,rng(rng)
,iteration(0)
,logX(0.0)
,active(true)
,mcmc_steps_taken(0)
{
    size_t num_particles = classic_logger.get_num_particles();

    // Get stuff from the logger
    auto  s = classic_logger.get_scalars();
    auto tb = classic_logger.get_tiebreakers();

    // Extract every nth to become a level
    for(size_t i=1; i<=(s.size()/num_particles); ++i)
    {
        levels_scalars.push_back(s[num_particles*i-1]);
        levels_tiebreakers.push_back(tb[num_particles*i-1]);
    }
    std::cout<<std::setprecision(8);

    std::fstream fout;
    fout.open("levels_scalars.txt", std::ios::out);
    fout<<std::setprecision(12);
    for(size_t i=0; i<levels_scalars.size(); ++i)
    {
        for(size_t j=0; j<levels_scalars[i].size(); ++j)
            fout<<levels_scalars[i][j]<<' ';
    }
    fout.close();
}

template<class ModelType>
void LNS<ModelType>::run(unsigned int mcmc_steps, unsigned int thin)
{
    for(size_t i=0; i<(levels_scalars.size()+1); ++i)
        do_iteration(mcmc_steps, thin);
}

template<class ModelType>
void LNS<ModelType>::do_iteration(unsigned int mcmc_steps, unsigned int thin)
{
    // Allocate space
    if(stashed_particles.size() != mcmc_steps/thin+1)
    {
        stashed_particles.resize(mcmc_steps/thin+1);
        stashed_scalars.resize(mcmc_steps/thin+1);
        stashed_tiebreakers.resize(mcmc_steps/thin+1);
    }

    // Set the threshold
    std::vector<double> s_threshold, tb_threshold;
    if(iteration == 0)
    {
        s_threshold = std::vector<double>(ModelType::get_num_scalars(), -1E300);
        tb_threshold = std::vector<double>(ModelType::get_num_scalars(), 0.0);
    }
    else
    {
        s_threshold = levels_scalars[iteration-1];
        tb_threshold = levels_tiebreakers[iteration-1];
    }

    // Count number of particles in the stash that are above the threshold    
    size_t count_above = 0;
    std::vector<bool> above(stashed_particles.size(), false);
    if(iteration > 0)
    {
        for(size_t i=0; i<stashed_particles.size(); ++i)
        {
            if(is_okay(stashed_scalars[i], stashed_tiebreakers[i],
                        s_threshold, tb_threshold))
            {
                above[i] = true;
                ++count_above;
            }
        }
        size_t count_below = stashed_particles.size() - count_above;

        for(size_t i=0; i<stashed_particles.size(); ++i)
        {
            if(active && !above[i] && rng.rand() <= 1.0/count_below)
            {
                logger.log_particle_info(run_id, iteration-1,
                                                stashed_scalars[i]);
                logger.save_particle(stashed_particles[i]);
            }
        }

        if(count_above == 0)
        {
            logX = -std::numeric_limits<double>::max();
            active = false;
        }
        else
            logX += log(count_above) - log(stashed_particles.size());

        logger.log_level(logX, s_threshold);
    }

    std::cout<<"# Linked NS run "<<run_id<<", iteration "<<iteration<<". ";
    std::cout<<"(log(X), scalars) = ("<<logX<<", ";
    for(size_t i=0; i<s_threshold.size(); ++i)
    {
        std::cout<<s_threshold[i];
        if(i != (s_threshold.size()-1))
            std::cout<<", ";
    }
    std::cout<<").\n";

    if(!active)
    {
        std::cout<<"#    Skipping MCMC.\n#"<<std::endl;
        ++iteration;
        return;
    }

    std::cout<<"#    Doing MCMC..."<<std::flush;

    unsigned int K;

    if(iteration == 0)
    {
        K = rng.rand_int(stashed_particles.size());
        stashed_particles[K].from_prior(rng);
        stashed_scalars[K] = stashed_particles[K].get_scalars();
        stashed_tiebreakers[K] = rng.rand();
    }
    else
    {
        unsigned int attempts = 0;
        while(true)
        {
            K = rng.rand_int(stashed_particles.size());
            ++attempts;
            if(above[K])
                break;
        }
    }

    ModelType particle = stashed_particles[K];
    std::vector<double> particle_scalars = stashed_scalars[K];
    std::vector<double> particle_tiebreakers = stashed_tiebreakers[K];

    ModelType proposal;
    std::vector<double> proposal_scalars, proposal_tiebreakers;
    double logH;

    unsigned int tries = 0;
    unsigned int accepts = 0;

    // Forwards
    for(unsigned int i=K+1; i<stashed_particles.size(); ++i)
    {
        for(unsigned int j=0; j<thin; ++j)
        {
            ++tries;

            // Do the proposal
            proposal = particle;
            logH = proposal.perturb(rng);
            proposal_tiebreakers = particle_tiebreakers;
            double& junk = proposal_tiebreakers
                                [rng.rand_int(proposal_tiebreakers.size())];
            junk += rng.randh();
            DNest4::wrap(junk, 0.0, 1.0);

            if(rng.rand() <= exp(logH))
            {
                proposal_scalars = proposal.get_scalars();
                if(is_okay(proposal_scalars, proposal_tiebreakers,
                            s_threshold, tb_threshold))
                {
                    particle = proposal;
                    particle_scalars = proposal_scalars;
                    particle_tiebreakers = proposal_tiebreakers;
                    ++accepts;
                }
            }
        }
        stashed_particles[i] = particle;
        stashed_scalars[i] = particle_scalars;
        stashed_tiebreakers[i] = particle_tiebreakers;
        ++mcmc_steps_taken;
    }

    // Backwards
    particle = stashed_particles[K];
    particle_scalars = stashed_scalars[K];
    particle_tiebreakers = stashed_tiebreakers[K];

    for(int i=K-1; i>=0; --i)
    {
        for(unsigned int j=0; j<thin; ++j)
        {
            ++tries;

            // Do the proposal
            proposal = particle;
            logH = proposal.perturb(rng);
            proposal_tiebreakers = particle_tiebreakers;
            double& junk = proposal_tiebreakers
                                [rng.rand_int(proposal_tiebreakers.size())];
            junk += rng.randh();
            DNest4::wrap(junk, 0.0, 1.0);

            if(rng.rand() <= exp(logH))
            {
                proposal_scalars = proposal.get_scalars();
                if(is_okay(proposal_scalars, proposal_tiebreakers,
                            s_threshold, tb_threshold))
                {
                    particle = proposal;
                    particle_scalars = proposal_scalars;
                    particle_tiebreakers = proposal_tiebreakers;
                    ++accepts;
                }
            }
        }
        stashed_particles[i] = particle;
        stashed_scalars[i] = particle_scalars;
        stashed_tiebreakers[i] = particle_tiebreakers;
        ++mcmc_steps_taken;

    }

    std::cout<<"done. Accepted "<<accepts<<'/'<<tries<<".\n#"<<std::endl;

    ++iteration;
}

} // namespace LiNeS


namespace LiNeS
{

template<class ModelType>
ClassicNestedSampler<ModelType>::
    ClassicNestedSampler(std::size_t num_particles, unsigned int seed)
:particles(num_particles)
,rng()
{
    rng.set_seed(seed);
}

} // namespace LiNeS


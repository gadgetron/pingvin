
#pragma once

#include "Node.h"
#include "hoNDArray.h"

#include <random>

namespace Gadgetron
{

template <typename T>
class RandNormGenerator
{
public:

    typedef std::mt19937 RandomGeneratorType;

    RandNormGenerator();
    RandNormGenerator(long long seed, T mean = 0, T sigma = 1);
    ~RandNormGenerator();

    void seed(unsigned long seed);
    void setPara(T mean = 0, T sigma = 1);

    RandomGeneratorType& getRandomer() { return rng_; }
    const RandomGeneratorType& getRandomer() const { return rng_; }

    void gen(hoNDArray<T>& randNum);
    void gen(hoNDArray< std::complex<T> >& randNum);

protected:

    RandomGeneratorType rng_;
    std::normal_distribution<T> dist_norm_;
};

/// add white noise to the kspace data
class WhiteNoiseInjectorGadget : public Core::ChannelGadget<mrd::Acquisition>
{
public:
    typedef Gadgetron::RandNormGenerator<double> RandGenType;

    WhiteNoiseInjectorGadget(const Core::Context& context, const Core::GadgetProperties& props);
    ~WhiteNoiseInjectorGadget() override;

protected:
    NODE_PROPERTY(noise_mean_, float, "Noise mean", 0.0);
    NODE_PROPERTY(noise_std_, float, "Noise standard deviation", 1.0);
    NODE_PROPERTY(add_noise_ref_, bool, "Add noise to reference scans", false);

    void process(Core::InputChannel<mrd::Acquisition>& in, Core::OutputChannel& out) override;

    /// random noise generator
    RandGenType* randn_;

    /// helper memory to store noise
    hoNDArray< std::complex<double> > noise_;
    hoNDArray< std::complex<float> > noise_fl_;

    /// calibration mode and rate
    size_t acceFactorE1_;
    size_t acceFactorE2_;

    bool is_interleaved_;
    bool is_embeded_;
    bool is_seperate_;
    bool is_external_;
    bool is_other_;
    bool is_no_acceleration_;
};

}

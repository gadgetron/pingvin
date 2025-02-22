#include "WhiteNoiseInjectorGadget.h"
#include "hoNDArray_elemwise.h"

#include <array>

namespace Gadgetron
{

template <typename T>
RandNormGenerator<T>::RandNormGenerator()
{
    rng_.seed();
    this->setPara(0, 1);
}

template <typename T>
RandNormGenerator<T>::RandNormGenerator(long long s, T mean, T sigma)
{
    this->seed(s);
    this->setPara(mean, sigma);
}

template <typename T>
RandNormGenerator<T>::~RandNormGenerator()
{
}

template <typename T>
void RandNormGenerator<T>::seed(unsigned long s)
{
    rng_.seed(s);
}

template <typename T>
void RandNormGenerator<T>::setPara(T mean, T sigma)
{
    typename std::normal_distribution<T>::param_type para(mean, sigma);
    dist_norm_.param(para);
}

template <typename T>
inline void RandNormGenerator<T>::gen(hoNDArray<T>& randNum)
{
    try
    {
        size_t N = randNum.get_number_of_elements();
        size_t n;
        for (n = 0; n<N; n++)
        {
            randNum(n) = dist_norm_(rng_);
        }
    }
    catch (...)
    {
        GADGET_THROW("Errors in RandNormGenerator<T>::gen(hoNDArray<T>& randNum) ... ");
    }
}

template <typename T>
inline void RandNormGenerator<T>::gen(hoNDArray< std::complex<T> >& randNum)
{
    try
    {
        size_t N = randNum.get_number_of_elements();
        size_t n;

        T real, imag;
        for (n = 0; n<N; n++)
        {
            real = dist_norm_(rng_);
            imag = dist_norm_(rng_);

            randNum(n) = std::complex<T>(real, imag);
        }
    }
    catch (...)
    {
        GADGET_THROW("Errors in RandNormGenerator<T>::gen(hoNDArray< std::complex<T> >& randNum) ... ");
    }
}

WhiteNoiseInjectorGadget::WhiteNoiseInjectorGadget(const Core::Context& context, const Core::GadgetProperties& props)
    : Core::ChannelGadget<mrd::Acquisition>(context, props)
    , acceFactorE1_(1), acceFactorE2_(1)
    , is_interleaved_(false), is_embeded_(false), is_seperate_(false), is_external_(false), is_other_(false), is_no_acceleration_(false)
{
    GDEBUG_STREAM("noise mean is " << noise_mean_);
    GDEBUG_STREAM("noise std is " << noise_std_);
    GDEBUG_STREAM("add_noise_ref is " << add_noise_ref_);

    randn_ = new RandGenType();
    randn_->setPara(noise_mean_, noise_std_);

    // get the current time and generate a seed
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    long long seed = (long long)(1e10*(timeinfo->tm_year+1900) + 1e8*(timeinfo->tm_mon+1) + 1e6*timeinfo->tm_mday + 1e4*timeinfo->tm_hour + 1e2*timeinfo->tm_min + timeinfo->tm_sec + std::rand());

    std::array<unsigned int, 10> sequence;
    sequence[0] = (unsigned int)(1e10*(timeinfo->tm_year+1900));
    sequence[1] = (unsigned int)(1e8*(timeinfo->tm_mon+1));
    sequence[2] = (unsigned int)(1e6*timeinfo->tm_mday);
    sequence[3] = (unsigned int)(1e4*timeinfo->tm_hour);
    sequence[4] = (unsigned int)(1e2*timeinfo->tm_min);
    sequence[5] = (unsigned int)(timeinfo->tm_sec);

    std::srand( (unsigned int)seed );
    sequence[6] = (unsigned int)(std::rand());
    sequence[7] = (unsigned int)(std::rand());
    sequence[8] = (unsigned int)(std::rand());
    sequence[9] = (unsigned int)(std::rand());

    std::seed_seq seedSeq(sequence.begin(), sequence.end());
    randn_->getRandomer().seed(seedSeq);

    randn_->seed( (unsigned long)seed );

    // ---------------------------------------------------------------------------------------------------------
    auto& header = context.header;
    if( header.encoding.size() != 1)
    {
      GDEBUG("Number of encoding spaces: %d\n", header.encoding.size());
      GADGET_THROW("This simple WhiteNoiseInjectorGadget only supports one encoding space\n");
    }
    if (!header.encoding[0].parallel_imaging) {
      GADGET_THROW("Parallel Imaging section not found in header");
    }

    mrd::ParallelImagingType p_imaging = *header.encoding[0].parallel_imaging;

    acceFactorE1_ = (double)(p_imaging.acceleration_factor.kspace_encoding_step_1);
    acceFactorE2_ = (double)(p_imaging.acceleration_factor.kspace_encoding_step_2);

    GDEBUG_STREAM("acceFactorE1_ is " << acceFactorE1_);
    GDEBUG_STREAM("acceFactorE2_ is " << acceFactorE2_);

    if ( !p_imaging.calibration_mode )
    {
        GADGET_THROW("Parallel Imaging calibrationMode not found in header");
    }

    auto calib = *p_imaging.calibration_mode;
    if ( calib == mrd::CalibrationMode::kInterleaved ) {
      is_interleaved_ = true;
      GDEBUG_STREAM("Calibration mode is interleaved");
    } else if ( calib == mrd::CalibrationMode::kEmbedded ) {
      is_embeded_ = true;
      GDEBUG_STREAM("Calibration mode is embedded");
    } else if ( calib == mrd::CalibrationMode::kSeparate ) {
      is_seperate_ = true;
      GDEBUG_STREAM("Calibration mode is separate");
    } else if ( calib == mrd::CalibrationMode::kExternal ) {
      is_external_ = true;
      GDEBUG_STREAM("Calibration mode is external");
    } else if ( calib == mrd::CalibrationMode::kOther ) {
      is_other_ = true;
      GDEBUG_STREAM("Calibration mode is other");
    } else {
      GADGET_THROW("Failed to process parallel imaging calibration mode");
    }
}


WhiteNoiseInjectorGadget::~WhiteNoiseInjectorGadget()
{
    delete randn_;
}

void WhiteNoiseInjectorGadget::process(Core::InputChannel<mrd::Acquisition>& in, Core::OutputChannel& out)
{
    for (auto m1 : in)
    {

    auto& head = m1.head;
    auto& data = m1.data;

    bool is_noise = head.flags.HasFlags(mrd::AcquisitionFlags::kIsNoiseMeasurement);
    bool is_scc_correction = head.flags.HasFlags(mrd::AcquisitionFlags::kIsSurfacecoilcorrectionscanData);

    bool is_ref = head.flags.HasFlags(mrd::AcquisitionFlags::kIsParallelCalibration);
    bool is_ref_kspace = head.flags.HasFlags(mrd::AcquisitionFlags::kIsParallelCalibrationAndImaging);

    size_t channels = m1.Coils();
    size_t samples = m1.Samples();

    if (!is_noise && !is_scc_correction )
    {
        bool add_noise = true;
        if ( is_ref && !is_ref_kspace && (is_seperate_||is_external_) )
        {
            add_noise = add_noise_ref_;

            if ( !add_noise )
            {
                GDEBUG_STREAM("WhiteNoiseInjectorGadget, noise is not added to the ref acquisitions ... ");
            }
        }

        if ( add_noise )
        {
            if ( !noise_.dimensions_equal(data) )
            {
                noise_.create(data.dimensions());
                noise_fl_.create(data.dimensions());
            }

            try
            {
                randn_->gen(noise_);
            }
            catch(...)
            {
                GADGET_THROW("WhiteNoiseInjectorGadget, randn_->gen(noise_) failed ... ");
            }

            if ( !noise_fl_.copyFrom(noise_) )
            {
                GADGET_THROW("WhiteNoiseInjectorGadget, noise_fl_.copyFrom(noise_) failed ... ");
            }

            try
            {
                Gadgetron::add(data, noise_fl_, data);
            }
            catch(...)
            {
                GADGET_THROW("WhiteNoiseInjectorGadget, Gadgetron::add(*m2->getObjectPtr(), noise_, *m2->getObjectPtr()) failed ... ");
            }
        }
    }

    out.push(std::move(m1));
    }
}

GADGETRON_GADGET_EXPORT(WhiteNoiseInjectorGadget)
}

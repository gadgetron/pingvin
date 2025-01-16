#pragma once

#include "Node.h"
#include "hoNDArray.h"
#include <complex>

namespace Gadgetron {

    // TODO the ignore_segment_ flag is a hack for some EPI sequences
    // should be fixed on the converter side.

    // This gadget fills the mrd::ReconData structures with kspace readouts and sets up the sampling limits
    // For the cartesian sampling, the filled kspace ensures its center (N/2) is aligned with the specified center in
    // the encoding limits For the non-cartesian sampling, this "center alignment" constraint is not applied and kspace
    // lines are filled as their E1 and E2 indexes

    // Since the order of data can be changed from its acquried time order, there is no easy way to resort waveform data
    // Therefore, the waveform data was copied and passed with every buffer

    class BucketToBufferGadget : public Core::ChannelGadget<mrd::AcquisitionBucket> {
    public:

        enum class Dimension { average, contrast, phase, repetition, set, segment, slice, none };

        struct Parameters : public Core::NodeParameters {
            using NodeParameters::NodeParameters;
            Parameters(const std::string& prefix) : NodeParameters(prefix, "Bucket To Buffer Options")
            {
                register_parameter("N-dimensions", &N_dimension, "N-Dimensions");
                register_parameter("S-dimensions", &S_dimension, "S-Dimensions");
                register_flag("split-slices", &split_slices, "Split slices");
                register_flag("ignore-segment", &ignore_segment, "Ignore segment");
                register_flag("verbose", &verbose, "Whether to print more information");
            }
            Dimension N_dimension = Dimension::none;
            Dimension S_dimension = Dimension::none;
            bool split_slices = false;
            bool ignore_segment = false;
            bool verbose = false;
        };

        using Core::ChannelGadget<mrd::AcquisitionBucket>::ChannelGadget;
        BucketToBufferGadget(const Core::MrdContext& context, const Parameters& params)
            : Core::ChannelGadget<mrd::AcquisitionBucket>(Core::Context{.header=context.header}, Core::GadgetProperties{})
            , parameters_(params)
        { }

    struct BufferKey {
        uint32_t average,slice,contrast,phase,repetition,set,segment;
        BufferKey(const BufferKey&) = default;
        BufferKey(const mrd::EncodingCounters& idx)
            : average{idx.average.value_or(0)}
            , slice{idx.slice.value_or(0)}
            , contrast{idx.contrast.value_or(0)}
            , phase{idx.phase.value_or(0)}
            , repetition{idx.repetition.value_or(0)}
            , set{idx.set.value_or(0)}
            , segment{idx.segment.value_or(0)}
        { }
    };

    protected:
        Parameters parameters_;

        void process(Core::InputChannel<mrd::AcquisitionBucket>& in, Core::OutputChannel& out) override;
        BufferKey getKey(const mrd::EncodingCounters& idx) const;

        mrd::ReconBuffer makeDataBuffer(const mrd::Acquisition& acq, mrd::EncodingType encoding,
            const mrd::EncodingLimitsType& stats, bool forref) const;

        mrd::SamplingDescription createSamplingDescription(const mrd::EncodingType& encoding,
            const mrd::EncodingLimitsType& stats, const mrd::Acquisition& acq, bool forref) const;

        void add_acquisition(mrd::ReconBuffer& dataBuffer, const mrd::Acquisition& acq, mrd::EncodingType encoding,
            const mrd::EncodingLimitsType& stats, bool forref);

        uint32_t getNE0(const mrd::Acquisition& acq, const mrd::EncodingType& encoding) const;
        uint32_t getNE1(const mrd::EncodingType& encoding, const mrd::EncodingLimitsType& stats, bool forref) const;
        uint32_t getNE2(const mrd::EncodingType& encoding, const mrd::EncodingLimitsType& stats, bool forref) const;
        uint32_t getNLOC(const mrd::EncodingType& encoding, const mrd::EncodingLimitsType& stats) const;
    };

    void from_string(const std::string&, BucketToBufferGadget::Dimension&);
    std::ostream& operator<<(std::ostream& out, const BucketToBufferGadget::Dimension& param);
    void validate(boost::any& v, const std::vector<std::string>& values, BucketToBufferGadget::Dimension*, int);
}

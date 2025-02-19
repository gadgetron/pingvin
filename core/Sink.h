#include "Node.h"

namespace Pingvin {

struct ISink {
    virtual ~ISink() = default;
    virtual void produce_output(Gadgetron::Core::ChannelPair& output_channel) = 0;
};


/** TODO: Move to MR-specific location! */
struct MrdSink : public ISink {
    MrdSink(std::ostream& output_stream, const Gadgetron::Core::MrdContext& ctx)
        : mrd_writer_(output_stream)
    {
        mrd_writer_.WriteHeader(ctx.header);
    }

    void produce_output(Gadgetron::Core::ChannelPair& output_channel) override
    {
        while (true) {
            try {
                auto message = output_channel.input.pop();

                using namespace Gadgetron::Core;

                if (convertible_to<mrd::Acquisition>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::Acquisition>(std::move(message)));
                } else if (convertible_to<mrd::WaveformUint32>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::WaveformUint32>(std::move(message)));
                } else if (convertible_to<mrd::ImageUint16>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageUint16>(std::move(message)));
                } else if (convertible_to<mrd::ImageInt16>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageInt16>(std::move(message)));
                } else if (convertible_to<mrd::ImageUint32>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageUint32>(std::move(message)));
                } else if (convertible_to<mrd::ImageInt32>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageInt32>(std::move(message)));
                } else if (convertible_to<mrd::ImageFloat>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageFloat>(std::move(message)));
                } else if (convertible_to<mrd::ImageDouble>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageDouble>(std::move(message)));
                } else if (convertible_to<mrd::ImageComplexFloat>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageComplexFloat>(std::move(message)));
                } else if (convertible_to<mrd::ImageComplexDouble>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageComplexDouble>(std::move(message)));
                } else if (convertible_to<mrd::AcquisitionBucket>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::AcquisitionBucket>(std::move(message)));
                } else if (convertible_to<mrd::ImageArray>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageArray>(std::move(message)));
                } else {
                    GADGET_THROW("Unsupported Message type for MrdWriter! Check that the last Gadget emits a valid MRD type.");
                }
            } catch (const Gadgetron::Core::ChannelClosed& exc) {
                break;
            }
        }

        mrd_writer_.EndData();
        mrd_writer_.Close();
    }

private:
    mrd::binary::MrdWriter mrd_writer_;
};


}
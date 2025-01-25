#include "CutXGadget.h"
#include "hoNDFFT.h"
#include "hoNDArray_utils.h"

namespace Gadgetron{

    CutXGadget::CutXGadget(const Core::Context& context, const Core::GadgetProperties& props)
        : ChannelGadget(context, props)
    {
        auto& h = context.header;

        if (h.encoding.empty()) {
            GDEBUG("This Gadget needs an encoding description\n");
            GADGET_THROW("No encoding spaces found in header");
        }

        GDEBUG("Number of encoding spaces = %d\n", h.encoding.size());

        // Get the encoding space and trajectory description
        mrd::EncodingSpaceType e_space = h.encoding[0].encoded_space;
        mrd::EncodingSpaceType r_space = h.encoding[0].recon_space;

        // Primary encoding space is for EPI
        encodeNx_  = e_space.matrix_size.x;
        encodeFOV_ = e_space.field_of_view_mm.x;
        reconNx_   = r_space.matrix_size.x;
        reconFOV_  = r_space.field_of_view_mm.x;

        cutNx_ = encodeNx_;
    }

    void CutXGadget::process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out)
    {
        for (auto acq : input) {
            try {
                auto& data = acq.data;
                size_t RO = data.get_size(0);

                if (RO > cutNx_) {
                    auto& head = acq.head;

                    uint32_t center_sample = head.center_sample.value_or(RO / 2);
                    uint16_t startX = center_sample - cutNx_ / 2;
                    uint16_t endX = startX + cutNx_ - 1;

                    float ratio = RO / static_cast<float>(cutNx_);
                    head.center_sample = static_cast<uint16_t>(center_sample / ratio);

                    hoNDArray<std::complex<float>> data_out(cutNx_, data.get_size(1));

                    for (size_t cha = 0; cha < data.get_size(1); cha++) {
                        memcpy(data_out.begin() + cha * cutNx_,
                               data.begin() + cha * RO + startX,
                               sizeof(std::complex<float>) * cutNx_);
                    }

                    acq.data = std::move(data_out);
                }

                out.push(std::move(acq));
            } catch (...) {
                GERROR("Errors in CutXGadget::process(...) ... ");
                throw;
            }
        }
    }

    GADGETRON_GADGET_EXPORT(CutXGadget)
}

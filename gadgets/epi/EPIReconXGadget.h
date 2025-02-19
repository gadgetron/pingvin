#pragma once

#include "MRNode.h"
#include "EPIReconXObjectTrapezoid.h"
#include "EPIReconXObjectFlat.h"

namespace Gadgetron {

    class EPIReconXGadget : public Core::MRChannelGadget<mrd::Acquisition> {
    public:
        EPIReconXGadget(const Core::MRContext& context, const Core::NodeParameters& parameters);

    protected:
        void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out) override;

        // A set of reconstruction objects
        EPI::EPIReconXObjectTrapezoid<std::complex<float>> reconx;
        EPI::EPIReconXObjectFlat<std::complex<float>> reconx_other;

        // readout oversampling for reconx_other
        float oversamplng_ratio2_;
    };

} // namespace Gadgetron
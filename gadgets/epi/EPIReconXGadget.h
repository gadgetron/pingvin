#pragma once

#include "Node.h"
#include "EPIReconXObjectTrapezoid.h"
#include "EPIReconXObjectFlat.h"

namespace Gadgetron {

    class EPIReconXGadget : public Core::ChannelGadget<mrd::Acquisition> {
    public:
        EPIReconXGadget(const Core::Context& context, const Core::GadgetProperties& props);

    protected:
        NODE_PROPERTY(verbose_mode_, bool, "Verbose output", false);

        void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out) override;

        // A set of reconstruction objects
        EPI::EPIReconXObjectTrapezoid<std::complex<float>> reconx;
        EPI::EPIReconXObjectFlat<std::complex<float>> reconx_other;

        // readout oversampling for reconx_other
        float oversamplng_ratio2_;
    };

} // namespace Gadgetron
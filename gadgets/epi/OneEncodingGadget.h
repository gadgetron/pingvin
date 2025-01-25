/** \file   OneEncodingGadget.h
    \brief  This is the class gadget to make sure EPI Flash Ref lines are in the same encoding space as the imaging lines.
    \author Hui Xue
*/

#pragma once

#include "Node.h"

namespace Gadgetron {

    class OneEncodingGadget : public Core::ChannelGadget<mrd::Acquisition> {
    public:
        OneEncodingGadget(const Core::Context& context, const Core::GadgetProperties& props)
            : ChannelGadget(context, props) {}

    protected:
        void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out) override;
    };

} // namespace Gadgetron

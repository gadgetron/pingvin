/** \file   OneEncodingGadget.h
    \brief  This is the class gadget to make sure EPI Flash Ref lines are in the same encoding space as the imaging lines.
    \author Hui Xue
*/

#pragma once

#include "MRNode.h"

namespace Gadgetron {

    class OneEncodingGadget : public Core::MRChannelGadget<mrd::Acquisition> {
    public:
        using Core::MRChannelGadget<mrd::Acquisition>::MRChannelGadget;

    protected:
        void process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out) override;
    };

} // namespace Gadgetron

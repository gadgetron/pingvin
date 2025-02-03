#pragma once

#include "Node.h"

namespace Gadgetron {

    /**
     * ImageIndexGadget sets the image_index fields for images.
     *
     * ImageIndexGadget will keep track of individual image series, and number images in each series
     * sequentially.
     */
    class ImageIndexGadget : public Core::MRChannelGadget<mrd::AnyImage> {
      public:
        using Core::MRChannelGadget<mrd::AnyImage>::MRChannelGadget;
        void process(Core::InputChannel<mrd::AnyImage> &, Core::OutputChannel &) override;
    };
}

#pragma once

#include "hoNDArray.h"
#include "Types.h"

namespace Gadgetron::Grappa {

    struct ChannelAnnotation {
        uint64_t number_of_combined_channels, number_of_uncombined_channels;
        std::vector<uint64_t> reordering;
    };

    /** TODO Joe: The ChannelReorderer is no longer used, so all Gadgets using this
     *      AnnotatedAcquisition can be updated to just use Core::Acquisition.
     */
    using AnnotatedAcquisition = Core::tuple<Core::Acquisition, Core::optional<ChannelAnnotation>>;
}

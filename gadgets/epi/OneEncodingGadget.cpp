#include "OneEncodingGadget.h"

namespace Gadgetron {

    void OneEncodingGadget::process(Core::InputChannel<mrd::Acquisition>& input, Core::OutputChannel& out) {
        for (auto acq : input) {
            acq.head.encoding_space_ref = 0;
            out.push(std::move(acq));
        }
    }

    GADGETRON_GADGET_EXPORT(OneEncodingGadget)
}

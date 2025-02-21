#include "SliceAccumulator.h"

#include "hoNDArray.h"

namespace {
    using namespace Gadgetron;
    using namespace Gadgetron::Grappa;
}

namespace Gadgetron::Grappa {

    void SliceAccumulator::process(Core::InputChannel<mrd::Acquisition> &in, Core::OutputChannel &out) {

        std::vector<mrd::Acquisition> acquisitions{};

        for (auto acquisition : in) {
            acquisitions.emplace_back(std::move(acquisition));

            if (acquisitions.back().head.flags.HasFlags(mrd::AcquisitionFlags::kLastInSlice)) {
                out.push(std::move(acquisitions));
                acquisitions = std::vector<mrd::Acquisition>{};
            }
        }
    }

}
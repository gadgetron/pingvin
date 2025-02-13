#pragma once

#include <map>
#include <memory>

#include "Branch.h"

#include "Channel.h"

namespace Gadgetron::Core::Parallel {

    /** TODO: Move to MR-specific location! */
    template<class ...ARGS>
    class Fanout : public MRBranch<ARGS...> {
    public:
        using MRBranch<ARGS...>::MRBranch;

        void process(InputChannel<ARGS...>& input, std::map<std::string, OutputChannel> output) override {
            for (auto thing : input) {
                for (auto &pair : output) {
                    auto copy_of_thing = thing;
                    pair.second.push(std::move(copy_of_thing));
                }
            }
        }
    };

    using AcquisitionFanout = Core::Parallel::Fanout<mrd::Acquisition>;
    using WaveformFanout = Core::Parallel::Fanout<mrd::WaveformUint32>;
    using ImageFanout = Core::Parallel::Fanout<mrd::AnyImage>;
}
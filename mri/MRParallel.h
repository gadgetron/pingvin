#pragma once

#include "parallel/Branch.h"
#include "parallel/Merge.h"

#include "MRContext.h"

namespace Gadgetron::Core::Parallel {

    template <class... TYPELIST> class MRBranch : public TypedBranch<TYPELIST...> {
    public:
        using TypedBranch<TYPELIST...>::TypedBranch;

        MRBranch(const MRContext& context, const NodeParameters& parameters) {}
    };

    class MRMerge : public Merge {
    public:
        using Merge::Merge;

        MRMerge(const MRContext& context, const NodeParameters& parameters) {}
    };

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

} // namespace Gadgetron::Core::Parallel

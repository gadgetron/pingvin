#pragma once

#include "Node.h"

#include "Stream.h"

#include "parallel/Branch.h"
#include "parallel/Merge.h"

namespace Gadgetron::Core {

    class ParallelStream : public Node {
        using Branch = Core::Parallel::Branch;
        using Merge  = Core::Parallel::Merge;

    public:
        ParallelStream(std::unique_ptr<Branch> branch, std::unique_ptr<Merge> merge, std::vector<std::shared_ptr<Stream>> streams)
            : branch(std::move(branch)),
              merge(std::move(merge)),
              streams(std::move(streams))
        {}

        void process(
                Core::GenericInputChannel& input,
                Core::OutputChannel& output
        ) override;

    private:
        std::unique_ptr<Branch> branch;
        std::unique_ptr<Merge>  merge;
        std::vector<std::shared_ptr<Stream>> streams;
    };

}
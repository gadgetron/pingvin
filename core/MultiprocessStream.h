#pragma once

#include "PureStream.h"

#include <future>

namespace Gadgetron::Core {
    class MultiprocessStream : public Node {

    public:
        MultiprocessStream(const PureStream& pureStream, size_t workers)
            : workers_(workers), pure_stream_(pureStream) {}

        void process(GenericInputChannel& input, OutputChannel& output) override;

    private:

        using Queue = MPMCChannel<std::future<Message>>;

        void process_input(GenericInputChannel input, Queue &queue);
        void process_output(OutputChannel output, Queue &queue);

        const size_t workers_;
        const PureStream pure_stream_;
    };
}

#pragma once

#include "PureStream.h"
#include "Processable.h"

#include <future>

namespace Gadgetron::Main::Nodes {
    class ParallelProcess : public Processable {

    public:
        ParallelProcess(const PureStream& pureStream, size_t workers)
            : workers_(workers), pure_stream_(pureStream) {}

        void process(Core::GenericInputChannel input, Core::OutputChannel output, ErrorHandler& error_handler) override;
        const std::string& name() override;

    private:

        using Queue = Core::MPMCChannel<std::future<Core::Message>>;

        void process_input(Core::GenericInputChannel input, Queue &queue);
        void process_output(Core::OutputChannel output, Queue &queue);

        const size_t workers_;
        const PureStream pure_stream_;
    };
}

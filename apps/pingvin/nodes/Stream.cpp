#include "Stream.h"

#include "Parallel.h"
#include "ParallelProcess.h"
#include "Processable.h"

#include "Node.h"

namespace Gadgetron::Main::Nodes {

    using namespace Gadgetron::Core;

    void Stream::process(GenericInputChannel input,
            OutputChannel output,
            ErrorHandler &error_handler
    ) {
        if (empty()) return;

        std::vector<GenericInputChannel> input_channels{};
        input_channels.emplace_back(std::move(input));
        std::vector<OutputChannel> output_channels{};

        for (auto i = 0; i < nodes.size()-1; i++) {
            auto channel = make_channel<MessageChannel>();
            input_channels.emplace_back(std::move(channel.input));
            output_channels.emplace_back(std::move(channel.output));
        }

        output_channels.emplace_back(std::move(output));

        ErrorHandler nested_handler{error_handler, name()};

        std::vector<std::thread> threads(nodes.size());
        for (auto i = 0; i < nodes.size(); i++) {
            threads[i] = Processable::process_async(
                nodes[i],
                std::move(input_channels[i]),
                std::move(output_channels[i]),
                nested_handler
            );
        }

        for (auto &thread : threads) {
            thread.join();
        }
    }

    bool Stream::empty() const { return nodes.empty(); }
}

const std::string &Gadgetron::Main::Nodes::Stream::name() {
    static const std::string name = "Stream";
    return key.empty() ? name : key;
}

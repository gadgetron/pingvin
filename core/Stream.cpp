#include "Stream.h"

#include <thread>


namespace Gadgetron::Core {

    void Stream::process(GenericInputChannel& input, OutputChannel& output) {
        if (empty()) return;

        std::vector<GenericInputChannel> input_channels{};
        input_channels.emplace_back(std::move(input));
        std::vector<OutputChannel> output_channels{};

        for (size_t i = 0; i < nodes.size()-1; i++) {
            auto channel = make_channel<MessageChannel>();
            input_channels.emplace_back(std::move(channel.input));
            output_channels.emplace_back(std::move(channel.output));
        }

        output_channels.emplace_back(std::move(output));

        std::vector<std::thread> threads(nodes.size());
        for (size_t i = 0; i < nodes.size(); i++) {
            threads[i] = std::thread(
                [](auto node, auto input_channel, auto output_channel) {
                    try {
                        node->process(input_channel, output_channel);
                    } catch (const Core::ChannelClosed &e) {
                        // Ignored
                    }
                },
                nodes[i],
                std::move(input_channels[i]),
                std::move(output_channels[i])
            );
        }

        for (auto &thread : threads) {
            thread.join();
        }
    }

    bool Stream::empty() const { return nodes.empty(); }
}
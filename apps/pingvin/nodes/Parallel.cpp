#include "Parallel.h"

#include <map>
#include <memory>

#include "Channel.h"
#include "Context.h"

namespace {
    using namespace Gadgetron::Core;
    using namespace Gadgetron::Core::Parallel;
    using namespace Gadgetron::Main;
    using namespace Gadgetron::Main::Nodes;

    using DecoratedBranch = Gadgetron::Main::Nodes::Parallel::DecoratedBranch;
    using DecoratedMerge  = Gadgetron::Main::Nodes::Parallel::DecoratedMerge;

    template<class KEY, class VALUE, class F>
    auto transform_map(std::map<KEY, VALUE>& input, F f) {
        using TRANSFORMED = std::remove_reference_t<decltype(f(input.begin()->second))>;
        auto result = std::map<KEY, TRANSFORMED>();

        for (auto &key_val : input) result.emplace(key_val.first, f(key_val.second));

        return result;
    }
}

namespace {
    using namespace Gadgetron::Core;
    using namespace Gadgetron::Core::Parallel;
    using namespace Gadgetron::Main;
    using namespace Gadgetron::Main::Nodes;

    ChannelPair split(const ChannelPair &in) {
        return ChannelPair{ split(in.input), split(in.output) };
    }

    void emplace_channels(
            const Stream &stream,
            std::map<std::string, ChannelPair> &input,
            std::map<std::string, ChannelPair> &output
    ) {
        ChannelPair in_pair = make_channel<MessageChannel>();
        ChannelPair out_pair = make_channel<MessageChannel>();

        if (stream.empty()) out_pair = split(in_pair);

        input.emplace(stream.key, std::move(in_pair));
        output.emplace(stream.key, std::move(out_pair));
    }
}

namespace Gadgetron::Main::Nodes {

    void Parallel::process(
            InputChannel input,
            OutputChannel output,
            ErrorHandler &error_handler
    ) {
        ErrorHandler nested_handler{error_handler, branch->key};

        std::vector<std::thread> threads;
        std::map<std::string, ChannelPair> input_channels;
        std::map<std::string, ChannelPair> output_channels;

        for (auto &stream : streams) {
            emplace_channels(*stream, input_channels, output_channels);
        }

        threads.emplace_back(nested_handler.run(
                [&](auto input, auto output, auto bypass) {
                    branch->process(std::move(input), std::move(output), std::move(bypass));
                },
                std::move(input),
                transform_map(input_channels, [](auto& val) { return std::move(val.output); }),
                split(output)
        ));

        threads.emplace_back(nested_handler.run(
                [&](auto input, auto output) {
                    merge->process(std::move(input), std::move(output));
                },
                transform_map(output_channels, [](auto &val) { return std::move(val.input); }),
                std::move(output)
        ));

        for (auto &stream : streams) {
            threads.emplace_back(
                    Processable::process_async(
                            stream,
                            std::move(input_channels.at(stream->key).input),
                            std::move(output_channels.at(stream->key).output),
                            nested_handler
                    )
            );
        }

        for (auto &thread : threads) { thread.join(); }
    }


    const std::string &Parallel::name() {
        static const std::string n = "Parallel";
        return n;
    }

    void Parallel::DecoratedBranch::process(
            InputChannel input,
            std::map<std::string, OutputChannel> output,
            OutputChannel bypass
    ) {
         branch->process(std::move(input), std::move(output), std::move(bypass));
    }

    Parallel::DecoratedBranch::DecoratedBranch(
            std::unique_ptr<Branch> branch,
            std::string key
    ) : branch(std::move(branch)), key(std::move(key)) {}

    void Parallel::DecoratedMerge::process(
            std::map<std::string, InputChannel> input,
            OutputChannel output
    ) {
         merge->process(std::move(input), std::move(output));
    }

    Parallel::DecoratedMerge::DecoratedMerge(
            std::unique_ptr<Merge> merge,
            std::string key
    ) : merge(std::move(merge)), key(std::move(key)) {}
}
#pragma once

#include <future>
#include <thread>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "mrd/binary/protocols.h"

#include "nodes/Stream.h"
#include "Channel.h"
#include "ErrorHandler.h"

#include "Node.h"


namespace pingvin {


struct ISource {
    virtual ~ISource() = default;
    virtual void consume_input(Gadgetron::Core::ChannelPair& input_channel) = 0;
};

template <typename CTX>
struct Source : public ISource {
    virtual void setContext(CTX&) = 0;
};

struct ISink {
    virtual ~ISink() = default;
    virtual void produce_output(Gadgetron::Core::ChannelPair& output_channel) = 0;
};



///// BEGIN MRD /////

#include "mrd/types.h"

using MrdContext = Gadgetron::Core::MrdContext;

struct MrdSource : public Source<MrdContext> {
    MrdSource(std::istream& input_stream): mrd_reader_(input_stream) {}

    void setContext(MrdContext& ctx) override {
        std::optional<mrd::Header> hdr;
        mrd_reader_.ReadHeader(hdr);
        if (!hdr.has_value()) {
            GADGET_THROW("Failed to read MRD header");
        }

        ctx.header = hdr.value();
    }

    void consume_input(Gadgetron::Core::ChannelPair& input_channel) override
    {
        mrd::StreamItem stream_item;
        while (mrd_reader_.ReadData(stream_item)) {
            std::visit([&](auto&& arg) {
                Gadgetron::Core::Message msg(std::move(arg));
                input_channel.output.push_message(std::move(msg));
            }, stream_item);
        }
        mrd_reader_.Close();
        auto destruct_me = std::move(input_channel.output);
    }

private:
    mrd::binary::MrdReader mrd_reader_;
};

struct MrdSink : public ISink {
    MrdSink(std::ostream& output_stream, const MrdContext& ctx)
        : mrd_writer_(output_stream)
    {
        mrd_writer_.WriteHeader(ctx.header);
    }

    void produce_output(Gadgetron::Core::ChannelPair& output_channel) override
    {
        while (true) {
            try {
                auto message = output_channel.input.pop();

                using namespace Gadgetron::Core;

                if (convertible_to<mrd::Acquisition>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::Acquisition>(std::move(message)));
                } else if (convertible_to<mrd::WaveformUint32>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::WaveformUint32>(std::move(message)));
                } else if (convertible_to<mrd::ImageUint16>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageUint16>(std::move(message)));
                } else if (convertible_to<mrd::ImageInt16>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageInt16>(std::move(message)));
                } else if (convertible_to<mrd::ImageUint32>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageUint32>(std::move(message)));
                } else if (convertible_to<mrd::ImageInt32>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageInt32>(std::move(message)));
                } else if (convertible_to<mrd::ImageFloat>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageFloat>(std::move(message)));
                } else if (convertible_to<mrd::ImageDouble>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageDouble>(std::move(message)));
                } else if (convertible_to<mrd::ImageComplexFloat>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageComplexFloat>(std::move(message)));
                } else if (convertible_to<mrd::ImageComplexDouble>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageComplexDouble>(std::move(message)));
                } else if (convertible_to<mrd::AcquisitionBucket>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::AcquisitionBucket>(std::move(message)));
                } else if (convertible_to<mrd::ImageArray>(message) ) {
                    mrd_writer_.WriteData(force_unpack<mrd::ImageArray>(std::move(message)));
                } else {
                    GADGET_THROW("Unsupported Message type for MrdWriter! Check that the last Gadget emits a valid MRD type.");
                }
            } catch (const Gadgetron::Core::ChannelClosed& exc) {
                break;
            }
        }

        mrd_writer_.EndData();
        mrd_writer_.Close();
    }

private:
    mrd::binary::MrdWriter mrd_writer_;
};

///// END MRD /////

template <typename C>
struct INodeBuilder {
    virtual ~INodeBuilder() = default;
    virtual std::shared_ptr<Gadgetron::Core::Node> build(const C& ctx) const = 0;
    virtual Gadgetron::Core::NodeParameters get_parameters(void) = 0;
};

template <typename N, typename CTX>
class NodeBuilder : public INodeBuilder<CTX> {
public:
    NodeBuilder(const std::string& label): label_(label), parameters_(label) {}

    virtual std::shared_ptr<Gadgetron::Core::Node> build(const CTX& ctx) const override {
        return std::make_shared<N>(ctx, this->parameters_);
    }

    virtual Gadgetron::Core::NodeParameters get_parameters(void) override {
        return parameters_;
    }

private:
    std::string label_;
    typename N::Parameters parameters_;
};


class ErrorThrower : public Gadgetron::Main::ErrorReporter
{
  public:
    void operator()(const std::string& location, const std::string& message) override {
        throw std::runtime_error(("[" + location + "] ERROR: " + message));
    }
};

class NewNodeProcessable : public Gadgetron::Main::Processable {
public:
    NewNodeProcessable(const std::shared_ptr<Gadgetron::Core::Node>& node, std::string name) : node_(node), name_(std::move(name)) {}

    void process(Gadgetron::Core::GenericInputChannel input,
            Gadgetron::Core::OutputChannel output,
            Gadgetron::Main::ErrorHandler &
    ) override {
        node_->process(input, output);
    }

    const std::string& name() override {
        return name_;
    }

private:
    std::shared_ptr<Gadgetron::Core::Node> node_;
    const std::string name_;
};

class Pipeline {
  public:

    struct IBuilder {
        IBuilder(const std::string& name, const std::string& description): name(name), description(description) { }
        virtual Pipeline build(std::istream& input_stream, std::ostream& output_stream) = 0;
        virtual po::options_description collect_options(void) = 0;
        virtual ~IBuilder() = default;

        const std::string name;
        const std::string description;
    };

    template <typename CTX>
    struct Builder : public IBuilder{
        Builder(const std::string& pipeline_name, const std::string& pipeline_description)
            : IBuilder(pipeline_name, pipeline_description) {}

        template <typename N>
        Builder& withSource(void) {
            source_builder_ = [](std::istream& is) { return std::make_shared<N>(is); };
            return *this;
        }

        template <typename N>
        Builder& withSink(void) {
            sink_builder_ = [](std::ostream& os, const CTX& ctx) { return std::make_shared<N>(os, ctx); };
            return *this;
        }

        template <typename N>
        Builder& withNode(const std::string& label) {
            auto nb = std::make_shared<NodeBuilder<N, CTX>>(label);
            builders_.emplace_back(nb);
            return *this;
        }

        po::options_description collect_options(void) override {
            po::options_description pipeline_desc(this->description);

            for (auto& nb: builders_) {
                Gadgetron::Core::NodeParameters node_params = nb->get_parameters();
                po::options_description node_desc(node_params.description());

                if (node_params.parameters().size() > 0) {
                    for (auto& p: node_params.parameters()) {
                        node_desc.add(p->as_boost_option());
                    }

                    pipeline_desc.add(node_desc);
                }
            }
            return pipeline_desc;
        }

        Pipeline build(std::istream& input_stream, std::ostream& output_stream) override {
            std::cerr << "Building pipeline " << pipeline_name << std::endl;

            if (!source_builder_) {
                throw std::runtime_error("No source specified");
            }
            if (!sink_builder_) {
                throw std::runtime_error("No sink specified");
            }
            auto source = source_builder_(input_stream);

            CTX ctx;
            source->setContext(ctx);

            auto sink = sink_builder_(output_stream, ctx);

            std::vector<std::shared_ptr<Gadgetron::Core::Node>> nodes;
            for (auto& builder : builders_) {
                nodes.emplace_back(builder->build(ctx));
            }

            Pipeline pipeline;
            pipeline.source_ = source;
            pipeline.sink_ = sink;
            pipeline.nodes_ = std::move(nodes);

            return std::move(pipeline);
        }

        std::function<std::shared_ptr<Source<CTX>>(std::istream&)> source_builder_;
        std::function<std::shared_ptr<ISink>(std::ostream&, const CTX&)> sink_builder_;

        std::vector<std::shared_ptr<INodeBuilder<CTX>>> builders_;

        const std::string pipeline_name;
    };

    virtual ~Pipeline() = default;

    void run(void) {
        /** TODO: This is the only thing not yet "ported" to the new Pipeline::Builder technique:
         *
         * The Context object previously held a `std::string gadgetron_home`, that could be configured
         * from the CLI.
         *
         * However, this Path is only used in TWO Gadgets, and it is only used to load Python files.
         * These Gadgets can be updated to read the Python file paths from its own CLI parameters...
         *
         * Otherwise, the GADGETRON_HOME path can be queried from the `static default_gadgetron_home()` function.
         */
        // Context::Paths paths{
        //     (vm.count("home"))
        //         ? vm["home"].as<std::filesystem::path>().string()
        //         : "/opt/pingvin"
        // };
        // auto context = StreamContext(hdr, paths, vm);

        std::vector<std::shared_ptr<Gadgetron::Main::Processable>> processables;
        for (auto& node: nodes_) {
            processables.emplace_back(std::make_shared<NewNodeProcessable>(node, "TODO"));
        }

        auto stream = std::make_unique<Gadgetron::Main::Nodes::Stream>(processables);

        auto input_channel = Gadgetron::Core::make_channel<Gadgetron::Core::MessageChannel>();
        auto output_channel = Gadgetron::Core::make_channel<Gadgetron::Core::MessageChannel>();
        std::atomic<bool> processing = true;

        auto process_future = std::async(std::launch::async, [&]() {
            try
            {
                ErrorThrower error_thrower;
                Gadgetron::Main::ErrorHandler error_handler(error_thrower, std::string(__FILE__));
                stream->process(std::move(input_channel.input), std::move(output_channel.output), error_handler);
                processing = false;
            }
            catch (const std::exception& exc)
            {
                {
                    // Induce a ChannelClosed exception upon readers of the channel.
                    auto destruct_me = std::move(output_channel.output);
                }
                processing = false;
                throw;
            }
        });

        auto input_future = std::async(std::launch::async, [&]() {
            try
            {
                source_->consume_input(input_channel);
            }
            catch(std::ios_base::failure& exc)
            {
                // Induce a ChannelClosed exception upon readers of the channel.
                auto destruct_me = std::move(input_channel.output);
            }
        });

        auto output_future = std::async(std::launch::async, [&]()
        {
            sink_->produce_output(output_channel);
        });

        // Clean up and propagate exceptions if they occurred
        input_future.get();
        output_future.get();
        process_future.get();

        GDEBUG_STREAM("Finished consuming stream");
    }

    std::string name(void) const { return name_; }
    std::string description(void) const { return description_; }

  protected:

    std::string name_;
    std::string description_;

    std::shared_ptr<ISource> source_;
    std::shared_ptr<ISink> sink_;

    std::vector<std::shared_ptr<Gadgetron::Core::Node>> nodes_;
};

} // namespace pingvin
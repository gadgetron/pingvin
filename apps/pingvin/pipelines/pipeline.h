#pragma once

#include <future>
#include <thread>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "mrd/binary/protocols.h"

#include "system_info.h"

#include "nodes/Stream.h"
#include "nodes/NodeProcessable.h"
#include "nodes/Parallel.h"
#include "nodes/ParallelProcess.h"
#include "Channel.h"

#include "Node.h"
#include "parallel/Branch.h"
#include "parallel/Merge.h"


namespace pingvin {


struct ISource {
    virtual ~ISource() = default;
    virtual void consume_input(Gadgetron::Core::ChannelPair& input_channel) = 0;
};

template <typename CTX>
struct Source : public ISource {
    virtual void initContext(CTX&) = 0;
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

    void initContext(MrdContext& ctx) override {
        std::optional<mrd::Header> hdr;
        mrd_reader_.ReadHeader(hdr);
        if (!hdr.has_value()) {
            GADGET_THROW("Failed to read MRD header");
        }

        ctx.header = hdr.value();

        for (char** raw = environ; *raw; ++raw) {
            std::string s(*raw);
            auto pos = s.find('=');
            if (pos != std::string::npos) {
                ctx.env[s.substr(0, pos)] = s.substr(pos + 1);
            }
        }

        ctx.paths.pingvin_home = Gadgetron::Main::Info::get_pingvin_home();
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
    virtual std::shared_ptr<Gadgetron::Main::Processable> build(const C& ctx) const = 0;
    virtual void collect_options(po::options_description& parent) = 0;
    virtual void dump_config(std::ostream& os, bool only_modified=false) = 0;
};

template <typename N, typename CTX>
class NodeBuilder : public INodeBuilder<CTX> {
public:
    NodeBuilder(const std::string& label): label_(label), parameters_(label) {}

    std::shared_ptr<Gadgetron::Main::Processable> build(const CTX& ctx) const override {
        auto node = std::make_shared<N>(ctx, this->parameters_);
        return std::make_shared<NodeProcessable>(node, label_);
    }

    void collect_options(po::options_description& parent) override {
        po::options_description desc(parameters_.description());

        if (parameters_.parameters().size() > 0) {
            for (auto& p: parameters_.parameters()) {
                desc.add(p->as_boost_option());
            }

            parent.add(desc);
        }
    }

    void dump_config(std::ostream& os, bool only_modified) override {
        std::stringstream ss;
        ss.imbue(std::locale::classic());
        for (auto& p: parameters_.parameters()) {
            if (!p->modified() && only_modified) {
                continue;
            }
            p->print(ss);
            ss << std::endl;
        }
        auto s = ss.str();
        if (!s.empty()) {
            os << "[" << parameters_.prefix() << "]" << std::endl;
            os << s << std::endl;
        }
    }

private:
    std::string label_;
    typename N::Parameters parameters_;
};

template <typename CTX>
class StreamBuilder : public INodeBuilder<CTX> {
public:
    StreamBuilder() {}
    StreamBuilder(const std::string& key): key_(key) {}

    void collect_options(po::options_description& stream_desc) override {
        for (auto& nb: builders_) {
            nb->collect_options(stream_desc);
        }
    }

    void dump_config(std::ostream& os, bool only_modified) {
        for (auto& nb: builders_) {
            nb->dump_config(os, only_modified);
        }
    }

    std::shared_ptr<Gadgetron::Main::Processable> build(const CTX& ctx) const override {
        std::vector<std::shared_ptr<Gadgetron::Main::Processable>> processables;
        for (auto& builder : builders_) {
            processables.emplace_back(
                builder->build(ctx)
            );
        }
        return std::make_shared<Gadgetron::Main::Nodes::Stream>(processables, key_);
    }

    void append(std::shared_ptr<INodeBuilder<CTX>> builder) {
        builders_.emplace_back(builder);
    }

private:
    std::string key_;
    std::vector<std::shared_ptr<INodeBuilder<CTX>>> builders_;
};

template <typename CTX>
struct IBranchBuilder {
    virtual ~IBranchBuilder() = default;
    virtual std::unique_ptr<Gadgetron::Core::Parallel::Branch> build(const CTX& ctx) const = 0;
    virtual void collect_options(po::options_description& parent) = 0;
    virtual void dump_config(std::ostream& os, bool only_modified=false) = 0;
};

template <typename N, typename CTX>
class BranchBuilder : public IBranchBuilder<CTX> {
public:
    BranchBuilder(const std::string& label): label_(label), parameters_(label) {}

    std::unique_ptr<Gadgetron::Core::Parallel::Branch> build(const CTX& ctx) const override {
        return std::move(std::make_unique<N>(ctx, this->parameters_));
    }

    void collect_options(po::options_description& parent) override {
        po::options_description desc(parameters_.description());

        if (parameters_.parameters().size() > 0) {
            for (auto& p: parameters_.parameters()) {
                desc.add(p->as_boost_option());
            }

            parent.add(desc);
        }
    }

    void dump_config(std::ostream& os, bool only_modified) override {
        os << "[" << parameters_.prefix() << "]" << std::endl;
        for (auto& p: parameters_.parameters()) {
            if (!p->modified() && only_modified) {
                continue;
            }
            p->print(os);
            os << std::endl;
        }
        os << std::endl;
    }

private:
    std::string label_;
    typename N::Parameters parameters_;
};

template <typename CTX>
struct IMergeBuilder {
    virtual ~IMergeBuilder() = default;
    virtual std::unique_ptr<Gadgetron::Core::Parallel::Merge> build(const CTX& ctx) const = 0;
    virtual void collect_options(po::options_description& parent) = 0;
    virtual void dump_config(std::ostream& os, bool only_modified=false) = 0;
};

template <typename N, typename CTX>
class MergeBuilder : public IMergeBuilder<CTX> {
public:
    MergeBuilder(const std::string& label): label_(label), parameters_(label) {}

    std::unique_ptr<Gadgetron::Core::Parallel::Merge> build(const CTX& ctx) const override {
        return std::move(std::make_unique<N>(ctx, this->parameters_));
    }

    void collect_options(po::options_description& parent) override {
        po::options_description desc(parameters_.description());

        if (parameters_.parameters().size() > 0) {
            for (auto& p: parameters_.parameters()) {
                desc.add(p->as_boost_option());
            }

            parent.add(desc);
        }
    }

    void dump_config(std::ostream& os, bool only_modified) override {
        os << "[" << parameters_.prefix() << "]" << std::endl;
        for (auto& p: parameters_.parameters()) {
            if (!p->modified() && only_modified) {
                continue;
            }
            p->print(os);
            os << std::endl;
        }
        os << std::endl;
    }

private:
    std::string label_;
    typename N::Parameters parameters_;
};

template <typename CTX>
struct PipelineBuilder;

template <typename CTX>
class ParallelBuilder : public INodeBuilder<CTX> {
public:
    ParallelBuilder(PipelineBuilder<CTX>& parent)
        : parent_(parent)
    {}

    template <typename BRANCH>
    ParallelBuilder& withBranch(const std::string& label) {
        branch_builder_ = std::make_unique<BranchBuilder<BRANCH, CTX>>(label);
        return *this;
    }

    template <typename MERGE>
    PipelineBuilder<CTX>& withMerge(const std::string& label) {
        merge_builder_ = std::make_unique<MergeBuilder<MERGE, CTX>>(label);
        return parent_;
    }

    ParallelBuilder& withStream(const std::string& key) {
        stream_builders_.emplace_back(key);
        return *this;
    }

    template <typename N>
    ParallelBuilder& withNode(const std::string& label) {
        auto nb = std::make_shared<NodeBuilder<N, CTX>>(label);
        if (stream_builders_.empty()) {
            stream_builders_.emplace_back();
        }
        stream_builders_.back().append(nb);
        return *this;
    }

    virtual std::shared_ptr<Gadgetron::Main::Processable> build(const CTX& ctx) const override {
        if (!branch_builder_) {
            throw std::runtime_error("No branch specified");
        }
        if (!merge_builder_) {
            throw std::runtime_error("No merge specified");
        }
        auto branch = branch_builder_->build(ctx);
        std::vector<std::shared_ptr<Gadgetron::Main::Nodes::Stream>> streams;
        for (auto& stream_builder_: stream_builders_) {
            auto stream = std::dynamic_pointer_cast<Gadgetron::Main::Nodes::Stream>(stream_builder_.build(ctx));
            streams.push_back(stream);
        }
        auto merge = merge_builder_->build(ctx);

        return std::make_shared<Gadgetron::Main::Nodes::Parallel>(std::move(branch), std::move(merge), streams);
    }

    void collect_options(po::options_description& parent) override {
        if (!branch_builder_) {
            throw std::runtime_error("No branch specified");
        }
        if (!merge_builder_) {
            throw std::runtime_error("No merge specified");
        }
        branch_builder_->collect_options(parent);
        for (auto& stream_builder_: stream_builders_) {
            stream_builder_.collect_options(parent);
        }
        merge_builder_->collect_options(parent);
    }

    void dump_config(std::ostream& os, bool only_modified) override {
        branch_builder_->dump_config(os, only_modified);
        for (auto& stream_builder_: stream_builders_) {
            stream_builder_.dump_config(os, only_modified);
        }
        merge_builder_->dump_config(os, only_modified);
    }

private:
    PipelineBuilder<CTX>& parent_;

    std::unique_ptr<IBranchBuilder<CTX>> branch_builder_;
    std::unique_ptr<IMergeBuilder<CTX>> merge_builder_;
    std::vector<StreamBuilder<CTX>> stream_builders_;
};

template <typename CTX>
struct IPureNodeBuilder {
    virtual ~IPureNodeBuilder() = default;
    virtual std::unique_ptr<Gadgetron::Core::GenericPureGadget> build(const CTX& ctx) const = 0;
    virtual void collect_options(po::options_description& parent) = 0;
    virtual void dump_config(std::ostream& os, bool only_modified=false) = 0;
};

template <typename N, typename CTX>
class PureNodeBuilder : public IPureNodeBuilder<CTX> {
public:
    PureNodeBuilder(const std::string& label): label_(label), parameters_(label) {}

    std::unique_ptr<Gadgetron::Core::GenericPureGadget> build(const CTX& ctx) const override {
        return std::move(std::make_unique<N>(ctx, this->parameters_));
    }

    void collect_options(po::options_description& parent) override {
        po::options_description desc(parameters_.description());

        if (parameters_.parameters().size() > 0) {
            for (auto& p: parameters_.parameters()) {
                desc.add(p->as_boost_option());
            }

            parent.add(desc);
        }
    }

    void dump_config(std::ostream& os, bool only_modified) override {
        os << "[" << parameters_.prefix() << "]" << std::endl;
        for (auto& p: parameters_.parameters()) {
            if (!p->modified() && only_modified) {
                continue;
            }
            p->print(os);
            os << std::endl;
        }
        os << std::endl;
    }

private:
    std::string label_;
    typename N::Parameters parameters_;
};

template <typename CTX>
class ParallelProcessBuilder : public INodeBuilder<CTX> {
public:
    ParallelProcessBuilder(PipelineBuilder<CTX>& parent)
        : parent_(parent)
    {}

    template <typename N>
    ParallelProcessBuilder& withPureNode(const std::string& label) {
        auto nb = std::make_shared<PureNodeBuilder<N, CTX>>(label);
        builders_.emplace_back(nb);
        return *this;
    }

    PipelineBuilder<CTX>& withWorkers(size_t workers) {
        workers_ = workers;
        return parent_;
    }

    virtual std::shared_ptr<Gadgetron::Main::Processable> build(const CTX& ctx) const override {
        std::vector<std::shared_ptr<Gadgetron::Core::GenericPureGadget>> pure_gadgets;
        for (auto& builder : builders_) {
            auto node = builder->build(ctx);
            pure_gadgets.emplace_back(std::move(node));
        }

        Gadgetron::Main::Nodes::PureStream pure_stream(pure_gadgets);

        return std::make_shared<Gadgetron::Main::Nodes::ParallelProcess>(pure_stream, workers_);
    }

    void collect_options(po::options_description& parent) override {
        for (auto& builder: builders_) {
            builder->collect_options(parent);
        }
    }

    void dump_config(std::ostream& os, bool only_modified) override {
        for (auto& builder: builders_) {
            builder->dump_config(os, only_modified);
        }
    }

private:
    PipelineBuilder<CTX>& parent_;
    size_t workers_;

    std::vector<std::shared_ptr<IPureNodeBuilder<CTX>>> builders_;
};

// Forward declaration
class Pipeline;

struct IPipelineBuilder {
    IPipelineBuilder(const std::string& name, const std::string& description): name_(name), description_(description) { }
    virtual Pipeline build(std::istream& input_stream, std::ostream& output_stream) = 0;
    virtual po::options_description collect_options(void) = 0;
    virtual void dump_config(std::ostream& os, bool only_modified = false) = 0;
    virtual ~IPipelineBuilder() = default;

    std::string name(void) const { return name_; }
    std::string description(void) const { return description_; }

protected:
    std::string name_;
    std::string description_;
};


class Pipeline {
  public:
    Pipeline(const std::shared_ptr<ISource>& source_, const std::shared_ptr<ISink>& sink_, const std::shared_ptr<Gadgetron::Main::Processable>& stream_)
        : source_(source_), sink_(sink_), stream_(stream_)
    {}

    virtual ~Pipeline() = default;

    void run(void) {
        auto input_channel = Gadgetron::Core::make_channel<Gadgetron::Core::MessageChannel>();
        auto output_channel = Gadgetron::Core::make_channel<Gadgetron::Core::MessageChannel>();

        auto process_future = std::async(std::launch::async, [&]() {
            try {
                stream_->process(std::move(input_channel.input), std::move(output_channel.output));
            } catch (const std::exception& exc) {
                GERROR_STREAM("Exception in process_future: " << exc.what());
                // Induce a ChannelClosed exception upon readers of the channel.
                auto destruct_me = std::move(output_channel.output);
                throw;
            }
        });

        auto input_future = std::async(std::launch::async, [&]() {
            try {
                source_->consume_input(input_channel);
            } catch(const std::exception& exc) {
                GERROR_STREAM("Exception in input_future: " << exc.what());
                // Induce a ChannelClosed exception upon readers of the channel.
                auto destruct_me = std::move(input_channel.output);
                throw;
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

    std::shared_ptr<Gadgetron::Main::Processable> stream_;

    std::vector<std::shared_ptr<Gadgetron::Main::Processable>> processables_;
};

template <typename CTX>
struct PipelineBuilder : public IPipelineBuilder{
    PipelineBuilder(const std::string& pipeline_name, const std::string& pipeline_description)
        : IPipelineBuilder(pipeline_name, pipeline_description) {}

    PipelineBuilder duplicate(const std::string& pipeline_name, const std::string& pipeline_description) const {
        auto pb = *this;
        pb.name_ = pipeline_name;
        pb.description_ = pipeline_description;
        return std::move(pb);
    }

    template <typename N>
    PipelineBuilder& withSource(void) {
        source_builder_ = [](std::istream& is) { return std::make_shared<N>(is); };
        return *this;
    }

    template <typename N>
    PipelineBuilder& withSink(void) {
        sink_builder_ = [](std::ostream& os, const CTX& ctx) { return std::make_shared<N>(os, ctx); };
        return *this;
    }

    template <typename N>
    PipelineBuilder& withNode(const std::string& label) {
        auto nb = std::make_shared<NodeBuilder<N, CTX>>(label);
        streambuilder_.append(nb);
        return *this;
    }

    template <typename N>
    ParallelBuilder<CTX>& withBranch(const std::string& label) {
        auto pb = std::make_shared<ParallelBuilder<CTX>>(*this);
        streambuilder_.append(pb);
        return pb->template withBranch<N>(label);
    }

    ParallelProcessBuilder<CTX>& withParallelProcessStream(void) {
        auto pb = std::make_shared<ParallelProcessBuilder<CTX>>(*this);
        streambuilder_.append(pb);
        return *pb;
    } 

    po::options_description collect_options(void) override {
        po::options_description pipeline_desc(this->description_);

        streambuilder_.collect_options(pipeline_desc);
        return pipeline_desc;
    }

    void dump_config(std::ostream& os, bool only_modified) override {
        streambuilder_.dump_config(os, only_modified);
    }

    Pipeline build(std::istream& input_stream, std::ostream& output_stream) override {
        if (!source_builder_) {
            throw std::runtime_error("No source specified");
        }
        if (!sink_builder_) {
            throw std::runtime_error("No sink specified");
        }
        auto source = source_builder_(input_stream);

        CTX ctx;
        source->initContext(ctx);

        auto sink = sink_builder_(output_stream, ctx);

        Pipeline pipeline(source, sink, streambuilder_.build(ctx));
        return std::move(pipeline);
    }

    std::function<std::shared_ptr<Source<CTX>>(std::istream&)> source_builder_;
    std::function<std::shared_ptr<ISink>(std::ostream&, const CTX&)> sink_builder_;

    StreamBuilder<CTX> streambuilder_;

    const std::string pipeline_name;
};


} // namespace pingvin
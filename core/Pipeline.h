#pragma once

#include <future>
#include <thread>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "system_info.h"

#include "Node.h"
#include "Source.h"
#include "Sink.h"
#include "Stream.h"
#include "ParallelStream.h"
#include "parallel/Branch.h"
#include "parallel/Merge.h"
#include "MultiprocessStream.h"


namespace Pingvin {

template <typename CLS>
struct NodeParametersBuilder {
    NodeParametersBuilder(const std::string& label): label_(label), parameters_(label) {}

    void collect_options(po::options_description& parent) {
        po::options_description desc(parameters_.description());

        if (parameters_.parameters().size() > 0) {
            for (auto& p: parameters_.parameters()) {
                desc.add(p->as_boost_option());
            }

            parent.add(desc);
        }
    }

    void dump_config(std::ostream& os, bool only_modified) {
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

    const std::string label_;
    typename CLS::Parameters parameters_;
};

template <typename C>
struct INodeBuilder {
    virtual ~INodeBuilder() = default;
    virtual std::shared_ptr<Gadgetron::Core::Node> build(const C& ctx) const = 0;
    virtual void collect_options(po::options_description& parent) = 0;
    virtual void dump_config(std::ostream& os, bool only_modified=false) = 0;
};

template <typename N, typename CTX>
class NodeBuilder : public INodeBuilder<CTX>, NodeParametersBuilder<N> {
public:
    NodeBuilder(const std::string& label): NodeParametersBuilder<N>(label) {}

    std::shared_ptr<Gadgetron::Core::Node> build(const CTX& ctx) const override {
        return std::make_shared<N>(ctx, this->parameters_);
    }

    void collect_options(po::options_description& parent) override {
        NodeParametersBuilder<N>::collect_options(parent);
    }

    void dump_config(std::ostream& os, bool only_modified) override {
        NodeParametersBuilder<N>::dump_config(os, only_modified);
    }
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

    std::shared_ptr<Gadgetron::Core::Node> build(const CTX& ctx) const override {
        std::vector<std::shared_ptr<Gadgetron::Core::Node>> nodes;
        for (auto& builder : builders_) {
            nodes.emplace_back(
                builder->build(ctx)
            );
        }
        return std::make_shared<Gadgetron::Core::Stream>(nodes, key_);
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
class BranchBuilder : public IBranchBuilder<CTX>, NodeParametersBuilder<N> {
public:
    BranchBuilder(const std::string& label): NodeParametersBuilder<N>(label) {}

    std::unique_ptr<Gadgetron::Core::Parallel::Branch> build(const CTX& ctx) const override {
        return std::move(std::make_unique<N>(ctx, this->parameters_));
    }

    void collect_options(po::options_description& parent) override {
        NodeParametersBuilder<N>::collect_options(parent);
    }

    void dump_config(std::ostream& os, bool only_modified) override {
        NodeParametersBuilder<N>::dump_config(os, only_modified);
    }
};

template <typename CTX>
struct IMergeBuilder {
    virtual ~IMergeBuilder() = default;
    virtual std::unique_ptr<Gadgetron::Core::Parallel::Merge> build(const CTX& ctx) const = 0;
    virtual void collect_options(po::options_description& parent) = 0;
    virtual void dump_config(std::ostream& os, bool only_modified=false) = 0;
};

template <typename N, typename CTX>
class MergeBuilder : public IMergeBuilder<CTX>, NodeParametersBuilder<N> {
public:
    MergeBuilder(const std::string& label): NodeParametersBuilder<N>(label) {}

    std::unique_ptr<Gadgetron::Core::Parallel::Merge> build(const CTX& ctx) const override {
        return std::move(std::make_unique<N>(ctx, this->parameters_));
    }

    void collect_options(po::options_description& parent) override {
        NodeParametersBuilder<N>::collect_options(parent);
    }

    void dump_config(std::ostream& os, bool only_modified) override {
        NodeParametersBuilder<N>::dump_config(os, only_modified);
    }
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

    virtual std::shared_ptr<Gadgetron::Core::Node> build(const CTX& ctx) const override {
        if (!branch_builder_) {
            throw std::runtime_error("No branch specified");
        }
        if (!merge_builder_) {
            throw std::runtime_error("No merge specified");
        }
        auto branch = branch_builder_->build(ctx);
        std::vector<std::shared_ptr<Gadgetron::Core::Stream>> streams;
        for (auto& stream_builder_: stream_builders_) {
            auto stream = std::dynamic_pointer_cast<Gadgetron::Core::Stream>(stream_builder_.build(ctx));
            streams.push_back(stream);
        }
        auto merge = merge_builder_->build(ctx);

        return std::make_shared<Gadgetron::Core::ParallelStream>(std::move(branch), std::move(merge), std::move(streams));
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
class PureNodeBuilder : public IPureNodeBuilder<CTX>, NodeParametersBuilder<N> {
public:
    PureNodeBuilder(const std::string& label): NodeParametersBuilder<N>(label) {}

    std::unique_ptr<Gadgetron::Core::GenericPureGadget> build(const CTX& ctx) const override {
        return std::move(std::make_unique<N>(ctx, this->parameters_));
    }

    void collect_options(po::options_description& parent) override {
        NodeParametersBuilder<N>::collect_options(parent);
    }

    void dump_config(std::ostream& os, bool only_modified) override {
        NodeParametersBuilder<N>::dump_config(os, only_modified);
    }
};

template <typename CTX>
class MultiprocessStreamBuilder : public INodeBuilder<CTX> {
public:
    MultiprocessStreamBuilder(PipelineBuilder<CTX>& parent)
        : parent_(parent)
    {}

    template <typename N>
    MultiprocessStreamBuilder& withPureNode(const std::string& label) {
        auto nb = std::make_shared<PureNodeBuilder<N, CTX>>(label);
        builders_.emplace_back(nb);
        return *this;
    }

    PipelineBuilder<CTX>& withWorkers(size_t workers) {
        workers_ = workers;
        return parent_;
    }

    virtual std::shared_ptr<Gadgetron::Core::Node> build(const CTX& ctx) const override {
        std::vector<std::shared_ptr<Gadgetron::Core::GenericPureGadget>> pure_gadgets;
        for (auto& builder : builders_) {
            auto node = builder->build(ctx);
            pure_gadgets.emplace_back(std::move(node));
        }

        Gadgetron::Core::PureStream pure_stream(pure_gadgets);

        return std::make_shared<Gadgetron::Core::MultiprocessStream>(pure_stream, workers_);
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
    Pipeline(const std::shared_ptr<ISource>& source_, const std::shared_ptr<ISink>& sink_, const std::shared_ptr<Gadgetron::Core::Node>& stream_)
        : source_(source_), sink_(sink_), stream_(stream_)
    {}

    virtual ~Pipeline() = default;

    void run(void) {
        auto input_channel = Gadgetron::Core::make_channel<Gadgetron::Core::MessageChannel>();
        auto output_channel = Gadgetron::Core::make_channel<Gadgetron::Core::MessageChannel>();

        auto process_future = std::async(std::launch::async, [&]() {
            try {
                // stream_->process(std::move(input_channel.input), std::move(output_channel.output));
                stream_->process(input_channel.input, output_channel.output);
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

    std::shared_ptr<Gadgetron::Core::Node> stream_;
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

    MultiprocessStreamBuilder<CTX>& withMultiprocessStream(void) {
        auto pb = std::make_shared<MultiprocessStreamBuilder<CTX>>(*this);
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

private:
    std::function<std::shared_ptr<Source<CTX>>(std::istream&)> source_builder_;
    std::function<std::shared_ptr<ISink>(std::ostream&, const CTX&)> sink_builder_;

    StreamBuilder<CTX> streambuilder_;

    const std::string pipeline_name;
};


} // namespace Pingvin
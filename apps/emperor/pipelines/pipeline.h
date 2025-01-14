#pragma once

#include <tuple>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "StreamConsumer.h"

namespace pingvin {

class Pipeline {
  public:
    Pipeline(std::string name, std::string description) : name_(name), description_(description) {}
    virtual ~Pipeline() = default;

    // using TempGadget = NewChannelGadget<mrd::Acquisition>;

    // class Builder {
    // public:
    //     Builder() {}

    //     template <typename T>
    //     Builder& append(void) {
    //         auto n = std::make_shared<T>();
    //         nodes_.push_back(n);
    //         return *this;
    //     }

    //     std::vector<std::shared_ptr<TempGadget>> build(void) {
    //         return nodes_;
    //     }

    //     template <typename T>
    //     T build_pipeline(std::string name, std::string description) {
    //         // T p(name, description);
    //         T p;
    //         p.gadgets_ = nodes_;
    //         return std::move(p);
    //     }

    // private:
    //     std::vector<std::shared_ptr<TempGadget>> nodes_;
    // };

    virtual void build(void) = 0;

    void run(po::variables_map& vm) {
        std::cerr << "Running pipeline " << name_ << std::endl;

        std::cerr << "OK, now running Pingvin" << std::endl;

        std::unique_ptr<std::istream> input_file;
        if (vm.count("input")) {
            input_file = std::make_unique<std::ifstream>(vm["input"].as<std::string>());
            if (!input_file->good()) {
                GERROR_STREAM("Could not open input file: " << vm["input"].as<std::string>());
            }
        }

        std::unique_ptr<std::ostream> output_file;
        if (vm.count("output")) {
            output_file = std::make_unique<std::ofstream>(vm["output"].as<std::string>());
            if (!output_file->good()) {
                GERROR_STREAM("Could not open output file: " << vm["output"].as<std::string>());
            }
        }

        std::istream& input_stream = input_file ? *input_file : std::cin;
        std::ostream& output_stream = output_file ? *output_file : std::cout;

        Context::Paths paths{
            (vm.count("home"))
                ? vm["home"].as<std::filesystem::path>().string()
                : "/opt/pingvin"
        };

        mrd::binary::MrdReader mrd_reader(input_stream);
        mrd::binary::MrdWriter mrd_writer(output_stream);

        StreamConsumer consumer;
        mrd::Header hdr = consumer.consume_mrd_header(mrd_reader, mrd_writer);

        auto context = StreamContext(hdr, paths, vm);

        GDEBUG_STREAM("Initializing Gadgets");
        for (auto& g : gadgets_) {
            g->initialize(context, GadgetProperties());
        }
        GDEBUG_STREAM("Initialized Gadgets");

        auto stream = std::make_unique<Nodes::Stream>(processables_);
        consumer.consume_stream(mrd_reader, mrd_writer, stream);

        std::flush(output_stream);

        GDEBUG_STREAM("Finished consuming stream");
    }

    void install_cli(po::options_description& desc) {
        desc.add_options()
            ("help,h", "Show help message")
            ("config,c", po::value<std::string>(), "Pipeline configuration file")
            ("input,i", po::value<std::string>(), "Input stream")
            ("output,o", po::value<std::string>(), "Output stream")
            ;

        po::options_description gadgets("Pipeline Nodes");

        for (auto& g: gadgets_) {
            po::options_description group(g->name());
            g->install_cli(group);
            if (!group.options().empty()) {
                gadgets.add(group);
            }
        }

        desc.add(gadgets);
    }

    std::string name(void) const { return name_; }
    std::string description(void) const { return description_; }

  protected:

    template <typename T>
    void append(void) {
        auto n = std::make_shared<T>();
        gadgets_.emplace_back(n);
        processables_.emplace_back(std::make_shared<NewNodeProcessable>(n, "TODO"));
    }

    // template <typename T>
    // void appendParallel()

    std::string name_;
    std::string description_;

    std::vector<std::shared_ptr<GenericChannelGadget>> gadgets_;
    std::vector<std::shared_ptr<Processable>> processables_;
};

} // namespace pingvin
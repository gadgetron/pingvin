#pragma once

#include "pipeline.h"

namespace pingvin {

  using namespace Gadgetron;

  struct TextContext {

  };

  class TextSource: public Source<TextContext> {
    public:
        TextSource(std::istream& input_stream) : input_stream_(input_stream) {}

        void setContext(TextContext& ctx) override { }

        void consume_input(Gadgetron::Core::ChannelPair& input_channel) override
        {
          std::string line;
          while (std::getline(input_stream_, line)) {
              Gadgetron::Core::Message msg(std::move(line));
              input_channel.output.push_message(std::move(msg));
          }
          auto destruct_me = std::move(input_channel.output);
        }

    private:
        std::istream& input_stream_;
  };

  class TextSink: public ISink {
    public:
        TextSink(std::ostream& output_stream, const TextContext& ctx) : output_stream_(output_stream) {}

        void produce_output(Gadgetron::Core::ChannelPair& output_channel) override
        {
            while (true) {
                try {
                    auto message = output_channel.input.pop();
                    auto text = Gadgetron::Core::force_unpack<std::string>(std::move(message));
                    output_stream_ << text << std::endl;
                } catch (const Gadgetron::Core::ChannelClosed& exc) {
                    break;
                }
            }
        }

    private:
        std::ostream& output_stream_;
  };

  class TextualSearch : public Core::ChannelGadget<std::string> {
    public:
      struct Parameters : public Core::NodeParameters {
          using NodeParameters::NodeParameters;
          Parameters(const std::string& prefix) : NodeParameters(prefix, "Textual Search Options") {
            register_parameter("target", &target, "Text to search for");
          }
          std::string target;
        };

      TextualSearch(const TextContext& context, const Parameters& params)
            : Core::ChannelGadget<std::string>(Core::Context{}, Core::GadgetProperties{})
            , parameters_(params)
        { }

        void process(Core::InputChannel<std::string>& input, Core::OutputChannel& out) override {
            for (auto line : input) {
                // out.push(line);

                if (line.find(parameters_.target) != std::string::npos) {
                    out.push(line);
                }
            }
        }

    protected:
      const Parameters parameters_;
  };

  static auto file_search = PipelineBuilder<TextContext>("text-search", "Search for strings in text")
        .withSource<TextSource>()
        .withSink<TextSink>()
        .withNode<TextualSearch>("search")
        ;

} // namespace pingvin
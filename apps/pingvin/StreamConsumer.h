#include <filesystem>
#include <future>
#include <memory>
#include <sstream>
#include <thread>

#include <mrd/binary/protocols.h>

#include "nodes/Stream.h"
#include "Channel.h"
#include "ErrorHandler.h"

#include "Node.h"

#include "pingvin_config.h"

using namespace Gadgetron::Core;
using namespace Gadgetron::Main;

namespace
{
class ErrorThrower : public ErrorReporter
{
  public:
    void operator()(const std::string& location, const std::string& message) override {
        throw std::runtime_error(("[" + location + "] ERROR: " + message));
    }
};

std::filesystem::path find_config_path(const std::string& home_dir, const std::string& config_xml)
{
    auto config_path = std::filesystem::path(home_dir) / std::filesystem::path(PINGVIN_CONFIG_PATH) /
                       std::filesystem::path(config_xml);

    if (!std::filesystem::is_regular_file(config_path)) {
        throw std::runtime_error("Failed to find Pingvin configuration at the expected path: " +
                                 config_path.string());
    }

    return config_path;
}

    class NewNodeProcessable : public Processable {
    public:
        NewNodeProcessable(const std::shared_ptr<Gadgetron::Core::Node>& node, std::string name) : node_(node), name_(std::move(name)) {}

        void process(GenericInputChannel input,
                OutputChannel output,
                ErrorHandler &
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

} // namespace

class StreamConsumer
{
public:
    StreamConsumer() {}
    ~StreamConsumer() {}

    void consume_stream(mrd::binary::MrdReader& mrd_reader, mrd::binary::MrdWriter& mrd_writer, const std::unique_ptr<Nodes::Stream>& stream)
    {
        auto input_channel = make_channel<MessageChannel>();
        auto output_channel = make_channel<MessageChannel>();
        std::atomic<bool> processing = true;

        auto process_future = std::async(std::launch::async, [&]() {
            try
            {
                ErrorThrower error_thrower;
                ErrorHandler error_handler(error_thrower, std::string(__FILE__));
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
                consume_input_stream(mrd_reader, input_channel);
            }
            catch(std::ios_base::failure& exc)
            {
                // Induce a ChannelClosed exception upon readers of the channel.
                auto destruct_me = std::move(input_channel.output);
            }
        });

        auto output_future = std::async(std::launch::async, [&]()
        {
            process_output_stream(output_channel, mrd_writer);
        });

        // Clean up and propagate exceptions if they occurred
        input_future.get();
        output_future.get();
        process_future.get();
    }

    // mrd::Header consume_mrd_header(mrd::binary::MrdReader& mrd_reader, mrd::binary::MrdWriter& mrd_writer)
    // {
    //     std::optional<mrd::Header> hdr;

    //     mrd_reader.ReadHeader(hdr);
    //     mrd_writer.WriteHeader(hdr);

    //     if (!hdr.has_value()) {
    //         GADGET_THROW("Failed to read ISMRMRD header");
    //     }
    //     return hdr.value();
    // }

  private:

    void consume_input_stream(mrd::binary::MrdReader& mrd_reader, ChannelPair& input_channel)
    {
        mrd::StreamItem stream_item;
        while (mrd_reader.ReadData(stream_item)) {
            std::visit([&](auto&& arg) {
                Message msg(std::move(arg));
                input_channel.output.push_message(std::move(msg));
            }, stream_item);
        }

        mrd_reader.Close();
        auto destruct_me = std::move(input_channel.output);
    }

    void process_output_stream(ChannelPair& output_channel, mrd::binary::MrdWriter& mrd_writer)
    {
        while (true) {
            try {
                auto message = output_channel.input.pop();

                if (convertible_to<mrd::Acquisition>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::Acquisition>(std::move(message)));
                } else if (convertible_to<mrd::WaveformUint32>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::WaveformUint32>(std::move(message)));
                } else if (convertible_to<mrd::ImageUint16>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::ImageUint16>(std::move(message)));
                } else if (convertible_to<mrd::ImageInt16>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::ImageInt16>(std::move(message)));
                } else if (convertible_to<mrd::ImageUint32>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::ImageUint32>(std::move(message)));
                } else if (convertible_to<mrd::ImageInt32>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::ImageInt32>(std::move(message)));
                } else if (convertible_to<mrd::ImageFloat>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::ImageFloat>(std::move(message)));
                } else if (convertible_to<mrd::ImageDouble>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::ImageDouble>(std::move(message)));
                } else if (convertible_to<mrd::ImageComplexFloat>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::ImageComplexFloat>(std::move(message)));
                } else if (convertible_to<mrd::ImageComplexDouble>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::ImageComplexDouble>(std::move(message)));
                } else if (convertible_to<mrd::AcquisitionBucket>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::AcquisitionBucket>(std::move(message)));
                } else if (convertible_to<mrd::ImageArray>(message) ) {
                    mrd_writer.WriteData(force_unpack<mrd::ImageArray>(std::move(message)));
                } else {
                    GADGET_THROW("Unsupported Message type for MrdWriter! Check that the last Gadget emits a valid MRD type.");
                }
            } catch (const ChannelClosed& exc) {
                break;
            }
        }

        mrd_writer.EndData();
        mrd_writer.Close();
    }
};

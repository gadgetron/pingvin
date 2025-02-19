#include "Node.h"

namespace Pingvin {

struct ISource {
    virtual ~ISource() = default;
    virtual void consume_input(Gadgetron::Core::ChannelPair& input_channel) = 0;
};

template <typename CTX>
struct Source : public ISource {
    virtual void initContext(CTX&) = 0;
};


/** TODO: Move to MR-specific location! */
struct MrdSource : public Source<Gadgetron::Core::MrdContext> {
    MrdSource(std::istream& input_stream): mrd_reader_(input_stream) {}

    void initContext(Gadgetron::Core::MrdContext& ctx) override {
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

        ctx.paths.pingvin_home = Pingvin::Main::get_pingvin_home();
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


}
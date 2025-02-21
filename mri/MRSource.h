#pragma once

#include "Source.h"

#include "MRContext.h"

#include "mrd/binary/protocols.h"

namespace Pingvin {

struct MRSource : public Source<Gadgetron::Core::MRContext> {
    MRSource(std::istream& input_stream): mrd_reader_(input_stream) {}

    void initContext(Gadgetron::Core::MRContext& ctx) override {
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
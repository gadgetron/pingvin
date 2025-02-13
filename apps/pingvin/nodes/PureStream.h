#pragma once
#include "Message.h"
#include "PureGadget.h"

namespace Gadgetron::Main::Nodes {
    class PureStream {
    public:
        PureStream(const std::vector<std::shared_ptr<Core::GenericPureGadget>>& pure_gadgets)
            : pure_gadgets_(std::move(pure_gadgets)) {}

        Core::Message process_function(Core::Message) const;

    private:
        const std::vector<std::shared_ptr<Core::GenericPureGadget>> pure_gadgets_;
    };
}

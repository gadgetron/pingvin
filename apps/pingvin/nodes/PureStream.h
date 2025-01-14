#pragma once
#include "Message.h"
#include "PureGadget.h"

namespace Gadgetron::Main::Nodes {
    class PureStream {
    public:
        Core::Message process_function(Core::Message) const;

    private:
        const std::vector<std::unique_ptr<Core::GenericPureGadget>> pure_gadgets;
    };
}

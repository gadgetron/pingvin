
#pragma once

#include <memory>

#include "Node.h"

#include "Channel.h"
#include "Context.h"

namespace Gadgetron::Core {

    class Stream : public Node {
    public:
        const std::string key;

        Stream(std::vector<std::shared_ptr<Node>> nodes, const std::string key=""): nodes(std::move(nodes)), key(key) {}

        void process(
                Core::GenericInputChannel& input,
                Core::OutputChannel& output
        ) override;

        bool empty() const;

    private:
        std::vector<std::shared_ptr<Node>> nodes;
    };
}

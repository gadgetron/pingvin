
#pragma once

#include <memory>

#include "Processable.h"

#include "Channel.h"
#include "Context.h"

namespace Gadgetron::Main::Nodes {

    class Stream : public Processable {
    public:
        const std::string key;

        Stream(std::vector<std::shared_ptr<Processable>> nodes, const std::string key=""): nodes(std::move(nodes)), key(key) {}

        void process(
                Core::GenericInputChannel input,
                Core::OutputChannel output,
                ErrorHandler &
        ) override;

        bool empty() const;
        const std::string &name() override;

    private:
        std::vector<std::shared_ptr<Processable>> nodes;
    };
}

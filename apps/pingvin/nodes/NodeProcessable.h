#pragma once

#include "Processable.h"

#include "Node.h"

class NodeProcessable : public Gadgetron::Main::Processable {
public:
    NodeProcessable(const std::shared_ptr<Gadgetron::Core::Node>& node, std::string name) : node_(node), name_(std::move(name)) {}

    void process(Gadgetron::Core::GenericInputChannel input,
            Gadgetron::Core::OutputChannel output
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

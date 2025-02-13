#pragma once

#include "Channel.h"
#include "Context.h"

#include <memory>
#include <thread>

namespace Gadgetron::Main {

    class Processable {
    public:
        virtual ~Processable() = default;

        virtual void process(
                Core::GenericInputChannel input,
                Core::OutputChannel output
        ) = 0;

        virtual const std::string& name() = 0;

        static std::thread process_async(
            std::shared_ptr<Processable> processable,
            Core::GenericInputChannel input,
            Core::OutputChannel output
        );
    };
}
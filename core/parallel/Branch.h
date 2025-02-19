#pragma once

#include <map>

#include "Channel.h"
#include "Parameters.h"

namespace Gadgetron::Core::Parallel {

    class Branch {
    public:
        struct Parameters : NodeParameters {
            using NodeParameters::NodeParameters;
        };

        virtual ~Branch() = default;
        virtual void process(
            GenericInputChannel input,
                std::map<std::string, OutputChannel> output,
                OutputChannel bypass
        ) = 0;

    private:
        template<class...> friend class TypedBranch;
    };

    template<class ...ARGS>
    class TypedBranch : public Branch {
    public:
        void process(GenericInputChannel input,
                std::map<std::string, OutputChannel> output, OutputChannel bypass) final
        {
            auto typed_input = InputChannel<ARGS...>(input, bypass);
            process(typed_input, std::move(output));
        }

        virtual void process(InputChannel<ARGS...> &, std::map<std::string, OutputChannel>) = 0;
    };

}
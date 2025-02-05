#pragma once

#include <map>
#include <memory>
#include <boost/dll.hpp>

#include "Channel.h"
#include "Context.h"
#include "Parameters.h"
#include "PropertyMixin.h"

namespace Gadgetron::Core::Parallel {

    class Branch : public PropertyMixin {
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
        explicit Branch(const GadgetProperties &props);

        template<class...> friend class TypedBranch;
    };

    template<class ...ARGS>
    class TypedBranch : public Branch {
    public:
        explicit TypedBranch(const GadgetProperties &props);

        void process(
            GenericInputChannel input,
                std::map<std::string, OutputChannel> output,
                OutputChannel bypass
        ) final;

        virtual void process(InputChannel<ARGS...> &, std::map<std::string, OutputChannel>) = 0;
    };

    /** TODO: Move to MR-specific location! */
    template <class... TYPELIST> class MRBranch : public TypedBranch<TYPELIST...> {
    public:
        using TypedBranch<TYPELIST...>::TypedBranch;

        MRBranch(const MrdContext& context, const NodeParameters& parameters)
            : TypedBranch<TYPELIST...>(Core::GadgetProperties{}) {}
    };

}

#include "Branch.hpp"

/** TODO: Delete everywhere */
#define GADGETRON_BRANCH_EXPORT(BranchClass)
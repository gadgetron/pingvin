#pragma once

#include "Parameters.h"
#include "Channel.h"
#include "PropertyMixin.h"
#include "Context.h"
#include <boost/dll/alias.hpp>

namespace po = boost::program_options;
namespace Gadgetron::Core {

    /**
     * Node is the base class for everything in a Gadgetron chain, including Gadgets and TypedChannelGadgets
     */
    class Node {

    public:
        virtual ~Node() = default;

        /**
         * The function which processes the data coming from the InputChannel. Conceptually a coroutine.
         * @param in Channel from which messages are received from upstream
         * @param out Channel in which messages are sent on downstream
         */
        virtual void process(GenericInputChannel& in, OutputChannel& out) = 0;
    };

    class GenericChannelGadget : public Node, public PropertyMixin {
    public:
        struct Parameters : NodeParameters {
            using NodeParameters::NodeParameters;
        };

        GenericChannelGadget(const Context& context, const GadgetProperties& properties) : PropertyMixin(properties) {}
    };

    /**
     * A Node providing typed access to input data. Messages not matching the TYPELIST are simply passed to the next
     * Node in the chain.
     * Should be the first choice for writing new Gadgets.
     * @tparam TYPELIST The type(s) of the messages to be received
     */
    template <class... TYPELIST> class ChannelGadget : public GenericChannelGadget {
    public:
        using GenericChannelGadget::GenericChannelGadget;

        void process(GenericInputChannel& in, OutputChannel& out) override final {
            auto typed_input = InputChannel<TYPELIST...>(in, out);
            this->process(typed_input, out);
        }

        /**
         * The process function to be implemented when inheriting from this class.
         * @param in A channel of the types specified in TYPELIST
         * @param out Channel of output
         */
        virtual void process(InputChannel<TYPELIST...>& in, OutputChannel& out) = 0;
    };

    /** TODO: Move to MR-specific location! */
    template <class... TYPELIST> class MRChannelGadget : public ChannelGadget<TYPELIST...> {
    public:
        using ChannelGadget<TYPELIST...>::ChannelGadget;

        MRChannelGadget(const MrdContext& context, const NodeParameters& parameters)
            : ChannelGadget<TYPELIST...>(Core::Context{}, Core::GadgetProperties{}) {}
    };
}

/** TODO: Delete everywhere */
#define GADGETRON_GADGET_EXPORT(GadgetClass)
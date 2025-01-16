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

        GenericChannelGadget(const Context& context, const GadgetProperties& properties) : PropertyMixin(properties), header{context.header} {}

    protected:
        const mrd::Header header ={};
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

        ///
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
}

/** TODO: Delete everywhere */
/*
#define GADGETRON_GADGET_EXPORT(GadgetClass)                                    \
    std::unique_ptr<Gadgetron::Core::Node> gadget_factory_##GadgetClass(        \
            const Gadgetron::Core::Context& context,                            \
            const std::string& name,                                            \
            const Gadgetron::Core::GadgetProperties& props) {                   \
        return std::make_unique<GadgetClass>(context, props);                   \
    }                                                                           \
BOOST_DLL_ALIAS(gadget_factory_##GadgetClass, gadget_factory_export_##GadgetClass)
*/

#define GADGETRON_GADGET_EXPORT(GadgetClass)
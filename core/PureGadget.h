#pragma once
#include "Node.h"

namespace Gadgetron::Core {

class GenericPureGadget : public Node {
public:
    void process(GenericInputChannel&in, OutputChannel &out) final {
        for (auto message : in)
            out.push(this->process_function(std::move(message)));
    }

    /***
     * Takes in a single Message, and produces another message as output
     * @return The processed Message
     */
    virtual Message process_function(Message) const = 0;
};

template <class RETURN, class INPUT>
class PureGadget : public GenericPureGadget {
public:
    Message process_function(Message message) const override{
        if (!convertible_to<INPUT>(message)) return message;
        return  Message(process_function(force_unpack<INPUT>(std::move(message))));
    }
    /***
     * Takes in an input of type INPUT and returns an output message with type RETURN
     * @param args Input argument
     * @return The processed message
     */
    virtual RETURN process_function(INPUT args) const = 0;
};


/** TODO: Move to MR-specific location! */
template <class RETURN, class INPUT>
class MRPureGadget : public PureGadget<RETURN, INPUT> {
public:
    MRPureGadget(const MrdContext& context, const NodeParameters& parameters) {}
};

}

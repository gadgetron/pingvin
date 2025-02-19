#include "PureStream.h"

namespace Gadgetron::Core {

Message PureStream::process_function(Message message) const
{
    return std::accumulate(
            pure_gadgets_.begin(),
            pure_gadgets_.end(),
            std::move(message),
            [](auto&& message, auto&& gadget) {
                return gadget->process_function(std::move(message));
            }
    );
}

}
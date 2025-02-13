#include "Processable.h"


std::thread Gadgetron::Main::Processable::process_async(
    std::shared_ptr<Processable> processable,
    Core::GenericInputChannel input,
    Core::OutputChannel output
) {
    // return processable->process(std::move(input), std::move(output));
    //     std::move(input),
    //     std::move(output),
    //     nested_handler
    // );
    return std::thread(
        [&processable](auto input, auto output) {
            processable->process(std::move(input), std::move(output));
        },
        std::move(input),
        std::move(output)
    );
}

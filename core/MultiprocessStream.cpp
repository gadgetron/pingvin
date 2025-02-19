#include "MultiprocessStream.h"

#include "ThreadPool.h"

namespace Gadgetron::Core {

    void MultiprocessStream::process_input(GenericInputChannel input, Queue &queue) {

        ThreadPool pool(workers_ ? workers_ : std::thread::hardware_concurrency());

        for (auto message : input) {
            queue.push(
                pool.async(
                        [&](auto message) { return pure_stream_.process_function(std::move(message)); },
                        std::move(message)
                )
            );
        }

        pool.join(); queue.close();
    }

    void MultiprocessStream::process_output(OutputChannel output, Queue &queue) {
        while(true) output.push_message(queue.pop().get());
    }

    void MultiprocessStream::process(GenericInputChannel& input, OutputChannel& output)
    {
        Queue queue;

        std::thread input_thread(
                [&](auto input) {
                    try {
                        this->process_input(std::move(input), queue);
                    } catch (const Core::ChannelClosed& e) {
                        // Ignored
                    }
                },
                std::move(input)
        );

        std::thread output_thread(
                [&](auto output) {
                    try {
                        this->process_output(std::move(output), queue); 
                    } catch (const Core::ChannelClosed& e) {
                        // Ignored
                    }
                },
                std::move(output)
        );

        input_thread.join();
        output_thread.join();
    }
}



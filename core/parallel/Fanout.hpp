#pragma once

namespace Gadgetron::Core::Parallel {

    template<class... ARGS>
    void Fanout<ARGS...>::process(InputChannel<ARGS...> &input, std::map<std::string, OutputChannel> output) {
        for (auto thing : input) {
            for (auto &pair : output) {
                auto copy_of_thing = thing;
                pair.second.push(std::move(copy_of_thing));
            }
        }
    }
}

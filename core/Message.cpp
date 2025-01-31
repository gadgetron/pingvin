#include "Message.h"

Gadgetron::Core::Message::Message(std::vector<std::unique_ptr<Gadgetron::Core::MessageChunk>> message_vector)
        : messages_(std::move(message_vector)) {

}

const std::vector<std::unique_ptr<Gadgetron::Core::MessageChunk>> &Gadgetron::Core::Message::messages() const {
    return messages_;
}

std::vector<std::unique_ptr<Gadgetron::Core::MessageChunk>> Gadgetron::Core::Message::take_messages() {
    return std::move(messages_);
}

Gadgetron::Core::Message Gadgetron::Core::Message::clone(){
    std::vector<std::unique_ptr<MessageChunk>> cloned_messages;
    for (const auto& chunk : messages_)
        cloned_messages.emplace_back(chunk->clone());

    return Message(std::move(cloned_messages));
}
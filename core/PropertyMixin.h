#pragma once

#include <unordered_map>
#include "io/from_string.h"

namespace Gadgetron::Core {

    using GadgetProperties = std::unordered_map<std::string,std::string>;

    class PropertyMixin {

    protected:

        template<class KEY_VALUE_STORAGE>
        explicit PropertyMixin(const KEY_VALUE_STORAGE &pairs) : properties(pairs.begin(), pairs.end()) {}

        template<class T>
        inline T get_property(const std::string &name, T default_value, const std::string &) {
            if (!properties.count(name)) return default_value;
            return IO::from_string<T>(properties.at(name));
        }

    /** TODO: These are no longer private or const, so they can be initialized *after* the Node has been constructed */
    // private:
    //     const GadgetProperties properties;
        GadgetProperties properties;
    };

    template<>
    inline std::string PropertyMixin::get_property<std::string>(const std::string &name, std::string default_value, const std::string &) {
        if (!properties.count(name)) return default_value;
        return properties.at(name);
    }
}

#define NODE_PROPERTY(NAME, TYPE, DESCRIPTION, DEFAULT) const TYPE NAME = this->template get_property<TYPE>(#NAME, DEFAULT, DESCRIPTION)
#define NODE_PROPERTY_NON_CONST(NAME, TYPE, DESCRIPTION, DEFAULT) TYPE NAME = this->template get_property<TYPE>(#NAME, DEFAULT, DESCRIPTION)
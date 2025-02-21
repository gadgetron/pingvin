#pragma once

#include <filesystem>

#include "mrd/types.h"

namespace Gadgetron::Core {

    struct MRContext  {

        /** TODO: This Path is only used in TWO Gadgets, and it is only used to load Python files.
         * These Gadgets can be updated to read the Python file paths from its own CLI parameters...
         *
         * Otherwise, the PINGVIN_HOME path can be queried using the `static get_gadgetron_home()` function.
         */
        struct Paths {
            std::filesystem::path pingvin_home;
        };

        mrd::Header header;
        std::map<std::string, std::string> env;
        Paths paths;
    };

} // namespace Gadgetron::Core
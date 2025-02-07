#pragma once

#include <mrd/types.h>

#include <filesystem>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>


namespace Gadgetron::Core {

    /** TODO: Move to MR-specific location */
    struct MrdContext  {

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


    /** TODO: Delete */
    struct Context {
        using Header = mrd::Header;

        struct Paths {
            boost::filesystem::path gadgetron_home;
        };

        Header header;
        Paths  paths;
        std::map<std::string, std::string> parameters;
    };

    struct StreamContext : Context {
        using Args = boost::program_options::variables_map;

        StreamContext(
            mrd::Header header,
            const Paths paths,
            const Args args
        ) : Context{
                std::move(header),
                paths,
                GetParameters(args)
            },
            args{args} {}


        Args args;

        private:
        static std::map<std::string, std::string> GetParameters(const boost::program_options::variables_map& args) {
            std::map<std::string, std::string> parameters;
            if (args.count("parameter")) {
                auto params = args["parameter"].as<std::vector<std::pair<std::string, std::string>>>();
                for (auto &arg : params) {
                    parameters[arg.first] = arg.second;
                }
            }
            return parameters;
        }

    };
}

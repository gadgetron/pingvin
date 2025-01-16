#include "pipelines/noise.h"
// #include "pipelines/cartesian_grappa.h"
#include "pipelines/default.h"

#include "log.h"
#include "system_info.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace pingvin;

namespace po = boost::program_options;
namespace fs = std::filesystem;

namespace {

std::string envvar_to_node_parameter(const std::string& env_var)
{
    static const std::string prefix("PINGVIN_");

    if (env_var.compare(0, prefix.size(), prefix) != 0) {
        return std::string();
    }

    std::string option(env_var.substr(prefix.size()));

    option.replace(option.find("_"), 1, ".");
    std::replace(option.begin(), option.end(), '_', '-');
    std::transform(option.begin(), option.end(), option.begin(),
        [](unsigned char c){ return std::tolower(c); });
    return option;
}

} // namespace


int main(int argc, char** argv)
{
    po::options_description global("Global options");

    std::filesystem::path pingvin_home("/tmp/pingvin");
    global.add_options()
        ("help,h", "Prints this help message.")
        ("info", "Prints build info about Pingvin.")
        ("home",
            po::value<std::filesystem::path>()->default_value(pingvin_home),
            "Set the Pingvin home directory.")
        ("config,c", po::value<std::string>(), "Configuration file.")
        ("input,i", po::value<std::string>(), "Input stream (default=stdin)")
        ("output,o", po::value<std::string>(), "Output stream (default=stdout)")
        ;

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("pipeline", po::value<std::string>(), "Pipeline to run.")
        ("subargs", po::value<std::vector<std::string>>(), "Arguments for the pipeline.")
        ;

    po::options_description help_options;
    help_options.add(global);

    po::options_description allowed_options;
    allowed_options.add(global).add(hidden);

    po::positional_options_description pos;
    pos.add("pipeline", 1)
        .add("subargs", -1);

    po::variables_map vm;
    std::vector<std::string> unrecognized;
    try {
        po::parsed_options parsed = po::basic_command_line_parser(argc, argv)
                                        .options(allowed_options)
                                        .positional(pos)
                                        .allow_unregistered()
                                        .run();
        po::store(parsed, vm);
        po::notify(vm);

        // Collect all the unrecognized options from the first pass. This will include the
        // (positional) command name, so we need to erase that.
        unrecognized = po::collect_unrecognized(parsed.options, po::include_positional);
        // Actually, it's not necessary...
        // unrecognized.erase(unrecognized.begin());
    } catch (po::error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (vm.count("info")) {
        std::cerr << "Pingvin Info: ..." << std::endl;
        return 0;
    }

    // "Choose" a Pipeline
    std::vector<Pipeline::IBuilder*> builders{&default_mr, &noise_dependency};
    std::map<std::string, Pipeline::IBuilder*> builder_map;
    for (auto& builder : builders) {
        builder_map[builder->name] = builder;
    }

    if (!vm.count("pipeline")) {
        if (vm.count("help")) {
            fs::path progpath(argv[0]);
            std::cerr << "Usage: " << progpath.filename().string() << " [global options] <PIPELINE> [pipeline options]"
                        << std::endl;
            std::cerr << help_options << std::endl;
            std::cerr << "Pipelines:" << std::endl;
            for (auto& builder : builders) {
                // std::cerr << "┌ " << builder->name << std::endl << "└──── " << builder->description << std::endl;
                std::cerr << "- " << builder->name << std::endl << "  └── " << builder->description << std::endl;
            }
            return 0;
        } else {
            std::cerr << "No pipeline specified" << std::endl;
            return 1;
        }
    }

    std::string subcmd = vm["pipeline"].as<std::string>();
    if (!builder_map.count(subcmd)) {
        std::cerr << "Unknown pipeline: " << subcmd << std::endl;
        return 1;
    }
    auto& builder = builder_map[subcmd];

    po::options_description pipeline_options = builder->collect_options();

    try {
        // Parse again for Pipeline
        po::parsed_options parsed = po::basic_command_line_parser(unrecognized).options(pipeline_options).run();
        po::store(parsed, vm);

        if (vm.count("help")) {
            std::cerr << help_options << std::endl;
            std::cerr << "Pipeline:" << std::endl;
            std::cerr << pipeline_options << std::endl;
            return 0;
        }

        po::store(po::parse_environment(pipeline_options, envvar_to_node_parameter), vm);

        if (vm.count("config")) {
            auto config_filename = vm["config"].as<std::string>();
            std::ifstream config_file(config_filename);
            if (!config_file) {
                std::cerr << "Could not open config file: " << config_filename << std::endl;
                return 1;
            }
            po::store(po::parse_config_file(config_file, pipeline_options), vm);
        }

        po::notify(vm);
    }
    catch (po::error &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::unique_ptr<std::istream> input_file;
    if (vm.count("input")) {
        input_file = std::make_unique<std::ifstream>(vm["input"].as<std::string>());
        if (!input_file->good()) {
            std::cerr << "Could not open input file: " << vm["input"].as<std::string>() << std::endl;
        }
    }

    std::unique_ptr<std::ostream> output_file;
    if (vm.count("output")) {
        output_file = std::make_unique<std::ofstream>(vm["output"].as<std::string>());
        if (!output_file->good()) {
            std::cerr << "Could not open output file: " << vm["output"].as<std::string>() << std::endl;
        }
    }

    std::istream& input_stream = input_file ? *input_file : std::cin;
    std::ostream& output_stream = output_file ? *output_file : std::cout;

    std::cerr << "Pingvin Home: " << vm["home"].as<std::filesystem::path>() << std::endl;

    auto pipeline = builder->build(input_stream, output_stream);

    pipeline.run();

    std::flush(output_stream);

    std::cout << "Pingvin finished successfully" << std::endl;

    return 0;
}
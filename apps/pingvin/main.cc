#include "log.h"
#include "system_info.h"
#include "pingvin_config.h"
#include "pipelines/registry.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <filesystem>

namespace po = boost::program_options;

using namespace Pingvin;

namespace {

void set_locale() {
    try {
        std::locale::global(std::locale(""));
    } catch (...) {
        std::locale::global(std::locale::classic());
    }
}

/** TODO: Is this still relevant and necessary in 2025? */
void check_environment_variables() {

    auto get_policy = []() -> std::string {
        auto raw = std::getenv("OMP_WAIT_POLICY");
        return boost::algorithm::to_lower_copy(raw ? std::string(raw) : std::string());
    };

    if ("passive" != get_policy()) {
        GWARN_STREAM("Environment variable 'OMP_WAIT_POLICY' not set to 'PASSIVE'.");
        GWARN_STREAM("Pingvin may experience serious performance issues under heavy load " <<
                        "(multiple simultaneous reconstructions, etc.)")
    }
}

/// Parses a "PINGVIN_*" environment variable into a CLI parameter
std::string envvar_to_node_parameter(const std::string& env_var)
{
    static const std::string prefix("PINGVIN_");

    // Only match envvars starting with "PINGVIN_"
    if (env_var.compare(0, prefix.size(), prefix) != 0) {
        return std::string();
    }

    std::string option(env_var.substr(prefix.size()));

    // Only match envvars with at least one underscore, which separates the
    // node name from the parameter name (e.g. "PINGVIN_FOO_BAR" => "foo.bar")
    size_t first_underscore_loc;
    if ((first_underscore_loc = option.find("_")) == std::string::npos) {
        return std::string();
    } else {
        option.replace(first_underscore_loc, 1, ".");
    }

    std::replace(option.begin(), option.end(), '_', '-');
    std::transform(option.begin(), option.end(), option.begin(),
        [](unsigned char c){ return std::tolower(c); });
    return option;
}

} // namespace


int main(int argc, char** argv)
{
    set_locale();

    po::options_description global("Global options");

    global.add_options()
        ("help,h", "Prints this help message")
        ("info", "Prints build info about Pingvin")
        ("list,l", "List available pipelines")
        ("genconf,g", "Generate a full configuration file for the given pipeline")
        ("dumpconf,d", "Dump the current configuration for the given pipeline")
        ;

    po::options_description pipeline_opts("Pipeline options");
    pipeline_opts.add_options()
        ("config,c", po::value<std::string>(), "Pipeline configuration file")
        ("input,i", po::value<std::string>(), "Input stream (default=stdin)")
        ("output,o", po::value<std::string>(), "Output stream (default=stdout)")
        ("home",
            po::value<std::filesystem::path>()->default_value(Pingvin::Main::get_pingvin_home()),
            "(Deprecated) Set the Pingvin home directory")
        ;

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("pipeline", po::value<std::string>(), "Pipeline to run.")
        ("subargs", po::value<std::vector<std::string>>(), "Arguments for the pipeline.")
        ;

    po::options_description help_options("Pingvin usage");
    help_options.add(global);

    po::options_description allowed_options;
    allowed_options.add(global).add(pipeline_opts).add(hidden);

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
        std::stringstream str;
        Pingvin::Main::print_system_information(str);
        GINFO_STREAM(str.str());
        return 0;
    }

    if (vm.count("home") && !vm["home"].defaulted()) {
        Pingvin::Main::set_pingvin_home(vm["home"].as<std::filesystem::path>());
        GINFO_STREAM("Set Pingvin home to: " << Pingvin::Main::get_pingvin_home());
    }

    PipelineRegistry registry;

    if (vm.count("list")) {
        std::cout << "Pipeline / Description" << std::endl;
        for (auto& builder : registry.builders()) {
            std::cout << builder->name() << " / " << builder->description() << std::endl;
        }
        return 0;
    }

    if (!vm.count("pipeline")) {
        if (vm.count("help")) {
            std::filesystem::path progpath(argv[0]);
            std::cout << "Usage: "
                    << progpath.filename().string() << " [global options] <PIPELINE> [pipeline options]" << std::endl
                    << help_options << std::endl;
            return 0;
        } else {
            std::cerr << "No pipeline specified" << std::endl;
            return 1;
        }
    }

    // "Choose" a Pipeline
    std::string subcmd = vm["pipeline"].as<std::string>();
    auto builder = registry.get(subcmd);
    if (!builder) {
        std::cerr << "Unknown pipeline: " << subcmd << std::endl;
        return 1;
    }

    po::options_description pipeline_options = builder->collect_options();

    try {
        // Parse again for Pipeline
        po::parsed_options parsed = po::basic_command_line_parser(unrecognized).options(pipeline_options).run();
        po::store(parsed, vm);

        if (vm.count("help")) {
            std::filesystem::path progpath(argv[0]);
            std::cout << "Usage: "
                    << progpath.filename().string() << " " << subcmd << " [pipeline options]" << std::endl << std::endl
                    << pipeline_opts << std::endl
                    << subcmd << " - " << pipeline_options << std::endl;
            return 0;
        }

        po::store(po::parse_environment(pipeline_options,
            // This ceremony enables defining PINGVIN_ env vars that don't map directly to CLI options
            // Otherwise, boost::program_options will complain about unrecognized PINGVIN_ env vars
            [&pipeline_options](const std::string& var) {
                auto parsed_option = envvar_to_node_parameter(var);
                return std::any_of(
                  pipeline_options.options().cbegin(),
                  pipeline_options.options().cend(),
                  [parsed_option](auto opt) { return parsed_option == opt->long_name(); }) ? parsed_option : "";
            }), vm);

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

    if (vm.count("dumpconf")) {
        builder->dump_config(std::cout, true);
        return 0;
    }
    if (vm.count("genconf")) {
        builder->dump_config(std::cout, false);
        return 0;
    }

    check_environment_variables();

    GINFO_STREAM("Pingvin " << PINGVIN_VERSION_STRING << " [" << PINGVIN_GIT_SHA1_HASH << "]");

    std::unique_ptr<std::istream> input_file;
    if (vm.count("input")) {
        input_file = std::make_unique<std::ifstream>(vm["input"].as<std::string>());
        if (!input_file->good()) {
            std::cerr << "Could not open input file: " << vm["input"].as<std::string>() << std::endl;
            return 1;
        }
    }

    std::unique_ptr<std::ostream> output_file;
    if (vm.count("output")) {
        output_file = std::make_unique<std::ofstream>(vm["output"].as<std::string>());
        if (!output_file->good()) {
            std::cerr << "Could not open output file: " << vm["output"].as<std::string>() << std::endl;
            return 1;
        }
    }

    std::istream& input_stream = input_file ? *input_file : std::cin;
    std::ostream& output_stream = output_file ? *output_file : std::cout;

    auto pipeline = builder->build(input_stream, output_stream);

    pipeline.run();

    std::flush(output_stream);

    return 0;
}
#include "pipelines/noise.h"
#include "pipelines/grappa.h"
#include "pipelines/default.h"
#include "pipelines/epi.h"
#include "pipelines/cartesian_grappa.h"
#include "pipelines/cartesian_spirit.h"
#include "pipelines/grappa_epi.h"
#include "pipelines/cmr.h"
#include "pipelines/parallel_bypass.h"
#include "pipelines/streams.h"
#include "pipelines/denoise.h"

#include "pipelines/file_search.h"

#include "log.h"
#include "initialization.h"
#include "system_info.h"
#include "pingvin_config.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <filesystem>

namespace po = boost::program_options;

using namespace pingvin;

namespace {

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
    po::options_description global("Global options");

    global.add_options()
        ("help,h", "Prints this help message")
        ("info", "Prints build info about Pingvin")
        ("home",
            po::value<std::filesystem::path>()->default_value(Gadgetron::Main::Info::get_pingvin_home()),
            "Set the Pingvin home directory")
        ("genconf,g", "Generate a full configuration file for the given pipeline")
        ("dumpconf,d", "Dump the current configuration for the given pipeline")
        ("config,c", po::value<std::string>(), "Pipeline configuration file")
        ("input,i", po::value<std::string>(), "Input stream (default=stdin)")
        ("output,o", po::value<std::string>(), "Output stream (default=stdout)")
        ;

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("pipeline", po::value<std::string>(), "Pipeline to run.")
        ("subargs", po::value<std::vector<std::string>>(), "Arguments for the pipeline.")
        ;

    po::options_description help_options("Pingvin usage");
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

    Gadgetron::Main::check_environment_variables();
    Gadgetron::Main::configure_blas_libraries();
    Gadgetron::Main::set_locale();

    GINFO_STREAM("Pingvin " << PINGVIN_VERSION_STRING << " [" << PINGVIN_GIT_SHA1_HASH << "]");

    if (vm.count("info")) {
        std::stringstream str;
        Gadgetron::Main::Info::print_system_information(str);
        GINFO_STREAM(str.str());
        return 0;
    }

    if (vm.count("home") && !vm["home"].defaulted()) {
        Gadgetron::Main::Info::set_pingvin_home(vm["home"].as<std::filesystem::path>());
        GINFO_STREAM("Set Pingvin home to: " << Gadgetron::Main::Info::get_pingvin_home());
    }

    // "Choose" a Pipeline
    std::vector<IPipelineBuilder*> builders{
        &file_search,

        &noise_dependency,
        &default_mr,
        &default_mr_optimized,

        &epi_2d,

        &grappa,
        &grappa_cpu,

        &cartesian_grappa,
        &cartesian_grappa_snr,
        &grappa_denoise,

        &cartesian_spirit,
        &cartesian_spirit_nonlinear,

        &grappa_epi,

        &cmr_cine_binning,
        &cmr_mapping_t1_sr,
        &cmr_rtcine_lax_ai,

        &example_parallel_bypass,

        &stream_cartesian_grappa_imagearray,
        &stream_cartesian_grappa,
        &stream_image_array_scaling,
        &stream_image_array_split,
        &stream_complex_to_float,
        &stream_float_to_fixed_point,
    };
    std::map<std::string, IPipelineBuilder*> builder_map;
    for (auto& builder : builders) {
        builder_map[builder->name()] = builder;
    }

    if (!vm.count("pipeline")) {
        if (vm.count("help")) {
            std::filesystem::path progpath(argv[0]);
            std::cerr << "Usage: " << progpath.filename().string() << " [global options] <PIPELINE> [pipeline options]"
                        << std::endl;
            std::cerr << help_options << std::endl;
            std::cerr << "Pipelines:" << std::endl;
            for (auto& builder : builders) {
                // std::cerr << "┌ " << builder->name << std::endl << "└──── " << builder->description << std::endl;
                std::cerr << "- " << builder->name() << std::endl << "  └── " << builder->description() << std::endl;
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
            std::cerr << "--- " << subcmd << " ---" << std::endl;
            std::cerr << pipeline_options << std::endl;
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
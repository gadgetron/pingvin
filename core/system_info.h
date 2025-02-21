#pragma once

#include <ostream>
#include <filesystem>

namespace Pingvin::Main {

    void print_system_information(std::ostream &);

    std::string pingvin_version();
    std::string pingvin_build();

    size_t system_memory();

    void set_pingvin_home(const std::filesystem::path&);
    const std::filesystem::path get_pingvin_home();

    namespace CUDA {
        bool cuda_support();
        int cuda_device_count();
        std::string cuda_driver_version();
        std::string cuda_runtime_version();
        std::string cuda_device_name(int);
        size_t cuda_device_memory(int);
        std::string cuda_device_capabilities(int);
    }
}
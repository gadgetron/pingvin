configure_file(pingvin_config.in pingvin_config.h)

add_executable(pingvin
        main.cc

        initialization.cpp
        system_info.cpp

        ErrorHandler.h

        nodes/Processable.h
        nodes/Processable.cpp

        nodes/Stream.cpp
        nodes/Stream.h
        nodes/NodeProcessable.h
        nodes/Parallel.cpp
        nodes/Parallel.h
        nodes/ParallelProcess.cpp
        nodes/ParallelProcess.h
        nodes/PureStream.cpp
        nodes/PureStream.h
        )

target_link_libraries(pingvin
        pingvin_core
        pingvin_core_parallel
        pingvin_mricore
        pingvin_epi
        pingvin_grappa
        pingvin_cmr
        pingvin_examples
        Boost::system
        Boost::program_options
        GTBLAS)

target_include_directories(pingvin
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        )


if (REQUIRE_SIGNED_CONFIG)
    target_link_libraries(pingvin GTBabylon)
endif()

if (CUDA_FOUND)
    target_link_libraries(pingvin ${CUDA_LIBRARIES})
endif ()

if (GPERFTOOLS_PROFILER)
    message("Adding gperftools cpu profiler to Pingvin link assemblage.")
    target_link_libraries(pingvin ${GPERFTOOLS_PROFILER} ${GPERFTOOLS_TCMALLOC})
endif ()

install(TARGETS pingvin DESTINATION bin COMPONENT main)

file(GLOB_RECURSE PIPELINE_CONFIG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/pipelines/config/*.conf)
install(FILES ${PIPELINE_CONFIG_FILES} DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH} COMPONENT main)
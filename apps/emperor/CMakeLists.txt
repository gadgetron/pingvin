configure_file(../pingvin/pingvin_config.in pingvin_config.h)

add_executable(emperor
    main.cc

        ../pingvin/system_info.cpp

        ../pingvin/ErrorHandler.h

        ../pingvin/nodes/Processable.h
        ../pingvin/nodes/Processable.cpp

        ../pingvin/nodes/Stream.cpp
        ../pingvin/nodes/Stream.h
        ../pingvin/nodes/Parallel.cpp
        ../pingvin/nodes/Parallel.h
        ../pingvin/nodes/ParallelProcess.cpp
        ../pingvin/nodes/ParallelProcess.h
        ../pingvin/nodes/PureStream.cpp
        ../pingvin/nodes/PureStream.h
        )

target_link_libraries(emperor
        pingvin_core
        pingvin_core_parallel
        pingvin_mricore
        pingvin_epi
        pingvin_grappa
        Boost::system
        Boost::program_options
        GTBLAS)

target_include_directories(emperor
        PRIVATE
        ${CMAKE_SOURCE_DIR}/apps/pingvin
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        )

install(TARGETS emperor DESTINATION bin COMPONENT main)
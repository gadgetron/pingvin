add_compile_options(-Wall -Werror)
add_subdirectory(parallel)

configure_file(pingvin_config.in ${CMAKE_BINARY_DIR}/pingvin_config.h)

add_library(pingvin_core SHARED
        Channel.cpp
        Message.cpp
        Process.cpp
        Stream.cpp
        ParallelStream.cpp
        MultiprocessStream.cpp
        PureStream.cpp
        system_info.cpp
        io/from_string.cpp)

set_target_properties(pingvin_core PROPERTIES
        VERSION ${PINGVIN_VERSION_STRING}
        SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_core
        mrd::mrd
        pingvin_toolbox_cpucore
        Boost::boost
        Boost::filesystem
        )

target_include_directories(pingvin_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_BINARY_DIR}
        )

install(TARGETS pingvin_core
        EXPORT pingvin-export
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        COMPONENT main
        )


install(FILES
        Channel.h
        Channel.hpp
        ChannelIterator.h
        Message.h
        Message.hpp
        MPMCChannel.h
        Context.h
        Source.h
        Sink.h
        Node.h
        PureGadget.h
        ChannelAlgorithms.h
        Process.h

        Pipeline.h
        Stream.h
        ParallelStream.h
        MultiprocessStream.h
        PureStream.h

        system_info.h

        DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)

install(FILES
        io/from_string.h
        io/primitives.h
        io/primitives.hpp
        DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH}/io COMPONENT main)
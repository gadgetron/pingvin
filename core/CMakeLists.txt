add_compile_options(-Wall -Werror)
add_subdirectory(parallel)

add_library(gadgetron_core SHARED
        Channel.cpp
        Gadget.cpp
        Message.cpp
        Process.cpp
        io/from_string.cpp)

set_target_properties(gadgetron_core PROPERTIES
        VERSION ${GADGETRON_VERSION_STRING}
        SOVERSION ${GADGETRON_SOVERSION})

target_link_libraries(gadgetron_core
        mrd::mrd
        gadgetron_toolbox_cpucore
        Boost::boost
        Boost::filesystem
        )

target_include_directories(gadgetron_core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        )

install(TARGETS gadgetron_core
        EXPORT gadgetron-export
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
        Gadget.h
        Context.h
        Node.h
        PureGadget.h
        PropertyMixin.h
        GadgetContainerMessage.h
        ChannelAlgorithms.h
        Process.h
        DESTINATION ${GADGETRON_INSTALL_INCLUDE_PATH} COMPONENT main)

install(FILES
        io/from_string.h
        io/primitives.h
        io/primitives.hpp
        DESTINATION ${GADGETRON_INSTALL_INCLUDE_PATH}/io COMPONENT main)

set(GADGETRON_BUILD_RPATH "${CMAKE_CURRENT_BINARY_DIR};${GADGETRON_BUILD_RPATH}" PARENT_SCOPE)

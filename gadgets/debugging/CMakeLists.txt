include_directories(${HDF5_INCLUDE_DIRS})

set(gadgetron_debugging_header_files
        PseudoReplicatorGadget.h
        RateLimitGadget.h
        WhiteNoiseInjectorGadget.h
        )

set(gadgetron_debugging_src_files
        PseudoReplicatorGadget.cpp
        RateLimitGadget.cpp
        WhiteNoiseInjectorGadget.cpp
        )

add_library(gadgetron_debugging SHARED
    ${gadgetron_debugging_header_files}
    ${gadgetron_debugging_src_files}
    )

set_target_properties(gadgetron_debugging PROPERTIES VERSION ${GADGETRON_VERSION_STRING} SOVERSION ${GADGETRON_SOVERSION})

target_link_libraries(gadgetron_debugging gadgetron_core)

target_include_directories(gadgetron_debugging
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

install(FILES
        ${gadgetron_debugging_header_files}
        DESTINATION ${GADGETRON_INSTALL_INCLUDE_PATH} COMPONENT main)

install(TARGETS gadgetron_debugging
        EXPORT gadgetron-export
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        COMPONENT main
)
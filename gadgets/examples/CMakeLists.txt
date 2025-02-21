
set(EXAMPLE_SOURCES
        ImageLayerer.cpp
        ImageLayerer.h
        ImageInverter.cpp
        ImageInverter.h
        AcquisitionWaveformBranch.cpp
        AcquisitionWaveformBranch.h)

add_library(pingvin_examples SHARED ${EXAMPLE_SOURCES})
set_target_properties(pingvin_examples PROPERTIES
        VERSION ${PINGVIN_VERSION_STRING}
        SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_examples
        pingvin_mri
        pingvin_toolbox_cpucore)

install(TARGETS pingvin_examples
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        COMPONENT main
)
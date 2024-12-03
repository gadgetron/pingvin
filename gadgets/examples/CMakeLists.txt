
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
        pingvin_core
        pingvin_core_parallel
        pingvin_mricore
        pingvin_toolbox_cpucore)

install(TARGETS pingvin_examples
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        COMPONENT main
)


install(FILES
        config/external_connect_example.xml
        config/external_equivalent_example.xml
        config/external_example.xml
        config/external_julia_acquisition_example.xml
        config/external_matlab_acquisition_example.xml
        config/external_matlab_bucket_example.xml
        config/external_matlab_buffer_example.xml
        config/external_matlab_tiny_example.xml
        config/external_python_acquisition_example.xml
        config/external_python_bucket_example.xml
        config/external_python_buffer_example.xml
        config/parallel_bypass_example.xml
        config/stream_complex_to_float.xml
        config/stream_float_to_short.xml
        config/stream_image_array_split.xml
        config/stream_image_array_scaling.xml
        DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH} COMPONENT main)

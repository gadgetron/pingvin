set(gadgetron_toolbox_python_header_files python_converters.h
        python_hoNDArray_converter.h
        python_mrd_converters.h
        python_numpy_wrappers.h
        python_toolbox.h
        python_tuple_converter.h
        python_optional_converter.h
        python_vector_converter.h)

add_library(gadgetron_toolbox_python SHARED
        python_toolbox.cpp
        ${gadgetron_toolbox_python_header_files})

target_link_libraries(gadgetron_toolbox_python
        gadgetron_core
        gadgetron_toolbox_mri_core
        gadgetron_toolbox_cpucore
        Boost::system
        python
        gadgetron_toolbox_log)

target_include_directories(gadgetron_toolbox_python
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)
set_target_properties(gadgetron_toolbox_python PROPERTIES VERSION ${GADGETRON_VERSION_STRING} SOVERSION ${GADGETRON_SOVERSION})

install(TARGETS gadgetron_toolbox_python
	EXPORT gadgetron-export
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)


install(FILES
        ${gadgetron_toolbox_python_header_files}
        DESTINATION ${GADGETRON_INSTALL_INCLUDE_PATH} COMPONENT main)
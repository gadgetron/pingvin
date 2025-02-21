if (MKL_FOUND)
    add_definitions(-DHAVE_MKL)
    # These are needed to get the linking to work properly when
    # MKL is installed, but Armadillo is NOT using it.
    list(APPEND EXTRA_MKL_LIBRARIES mkl_core)
    list(APPEND EXTRA_MKL_LIBRARIES mkl_intel_thread)
    link_directories( ${MKL_LIB_DIR} ${MKL_COMPILER_LIB_DIR} )
endif ()

set( pingvin_cmr_header_files
    CmrCartesianKSpaceBinningCineGadget.h
    CmrParametricMappingGadget.h
    CmrParametricT1SRMappingGadget.h
    CmrParametricT2MappingGadget.h
    PureCmrCartesianKSpaceBinningCineGadget.h)

set( pingvin_cmr_src_files
    CmrCartesianKSpaceBinningCineGadget.cpp
    CmrParametricMappingGadget.cpp
    CmrParametricT1SRMappingGadget.cpp
    CmrParametricT2MappingGadget.cpp
    PureCmrCartesianKSpaceBinningCineGadget.cpp)

if (BUILD_PYTHON_SUPPORT)

    message("Build python cmr gadgets ... ")

    include_directories(${CMAKE_SOURCE_DIR}/toolboxes/python)

    set( pingvin_cmr_header_files ${pingvin_cmr_header_files} CmrRealTimeLAXCineAIAnalysisGadget.h)
    set( pingvin_cmr_src_files ${pingvin_cmr_src_files} CmrRealTimeLAXCineAIAnalysisGadget.cpp)

    set( python_LandmarkDetection_files
        config/LandmarkDetection/gadgetron_cmr_landmark_detection.py
        config/LandmarkDetection/gadgetron_cmr_landmark_detection_util.py
        )

    install(FILES ${python_LandmarkDetection_files} DESTINATION ${PINGVIN_INSTALL_PYTHON_MODULE_PATH}/cmr_lax_landmark_detection COMPONENT main)
endif ()

add_library(pingvin_cmr SHARED
    ${pingvin_cmr_header_files}
    ${pingvin_cmr_src_files}
    )

set_target_properties(pingvin_cmr PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_cmr
    pingvin_mri
    pingvin_mricore
    pingvin_toolbox_log
    pingvin_toolbox_cpucore
    pingvin_toolbox_cpufft
    pingvin_toolbox_cpuklt
    pingvin_toolbox_mri_core
    pingvin_toolbox_cpuoperator
    pingvin_toolbox_image_analyze_io
    pingvin_toolbox_cmr
)

if (BUILD_PYTHON_SUPPORT)
    target_link_libraries(pingvin_cmr pingvin_toolbox_python)
endif ()

install(FILES ${pingvin_cmr_header_files} DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)

install(TARGETS pingvin_cmr
        EXPORT pingvin-export
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)
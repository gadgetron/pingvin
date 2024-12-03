# DCMTK-necessary preprocessor flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_CONFIG_H -D_REENTRANT -D_OSF_SOURCE")

set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost COMPONENTS date_time REQUIRED)

set(GT_DICOM_LIBRARIES
        dcmdata
        oflog
        ofstd
    z m pthread)
if (APPLE)
    list(APPEND GT_DICOM_LIBRARIES iconv)
endif ()

# sanity check:
message("DCMTK ${DCMTK_HOME}")
message("Include: ${DCMTK_INCLUDE_DIRS}")
message("Libraries: ${GT_DICOM_LIBRARIES}")

set(pingvin_dicom_header_files
    DicomFinishGadget.h
    DicomImageWriter.h
    dicom_ismrmrd_utility.h )

set(pingvin_dicom_src_files
    DicomFinishGadget.cpp
    DicomImageWriter.cpp
    dicom_ismrmrd_utility.cpp )

set(pingvin_dicom_config_files dicom.xml )

add_library(pingvin_dicom SHARED
            ${pingvin_dicom_header_files}
            ${pingvin_dicom_src_files}
            ${pingvin_dicom_config_files} )

set_target_properties(pingvin_dicom PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

target_include_directories(pingvin_dicom ${DCMTK_INCLUDE_DIRS})

target_link_libraries(pingvin_dicom
    pingvin_core
    pingvin_mricore
    pingvin_toolbox_log
    pingvin_toolbox_cpucore
    ${GT_DICOM_LIBRARIES} )

if(ARMADILLO_FOUND)
    target_link_libraries(pingvin_dicom pingvin_toolbox_cpucore_math )
endif()

install(
    FILES ${pingvin_dicom_header_files}
    DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)

install(TARGETS pingvin_dicom
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)

install(FILES ${pingvin_dicom_config_files}  DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH} COMPONENT main)
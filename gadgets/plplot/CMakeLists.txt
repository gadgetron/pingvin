include_directories(${PLPLOT_INCLUDE_DIR})
link_directories(${PLPLOT_LIB_DIR})


set( header_files NoiseCovariancePlottingGadget.h )
set( src_files NoiseCovariancePlottingGadget.cpp )
set( config_files Generic_Cartesian_Grappa_SNR_CoilQA.xml )

source_group(config FILES ${config_files})

add_library(pingvin_plplot SHARED
    ${header_files}
    ${src_files}
    ${config_files}
)

set_target_properties(pingvin_plplot PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_plplot
    pingvin_core
    pingvin_mricore
    pingvin_toolbox_log
    pingvin_toolbox_cpucore
    pingvin_toolbox_plplot
)

install(FILES
    ${header_files}
    DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)

install(TARGETS pingvin_plplot
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)

install (FILES  ${config_files}
        DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH} COMPONENT main)
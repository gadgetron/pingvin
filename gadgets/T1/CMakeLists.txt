
set(pingvin_t1_config_files config/MOLLI_T1_Moco.xml)

source_group(config FILES ${pingvin_t1_config_files})

add_library(pingvin_t1 SHARED
        T1MocoGadget.cpp)

set_target_properties(pingvin_t1 PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_t1
    pingvin_core
    pingvin_toolbox_log
    pingvin_toolbox_cpucore
    pingvin_toolbox_cpufft
    pingvin_toolbox_demons
    pingvin_toolbox_t1
    pingvin_toolbox_mri_core
)

install(TARGETS pingvin_t1
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)

install(FILES ${pingvin_t1_config_files} DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH} COMPONENT main)

set( pingvin_mri_noncartesian_header_files
	CPUGriddingReconGadget.h NonCartesianTools.h GriddingReconGadgetBase.h GriddingReconGadgetBase.hpp)

set( pingvin_mri_noncartesian_src_files
	CPUGriddingReconGadget.cpp NonCartesianTools.cpp)

set( pingvin_mri_noncartesian_config_files
	config/Generic_CPU_Gridding_Recon.xml
		config/Generic_Spiral.xml
		config/Generic_Spiral_SNR.xml
		config/Generic_Spiral_Flag.xml
 )


if(CUDA_FOUND)
	list(APPEND pingvin_mri_noncartesian_header_files GriddingReconGadget.h)
	list(APPEND pingvin_mri_noncartesian_src_files GriddingReconGadget.cpp)

	list(APPEND pingvin_mri_noncartesian_config_files config/Generic_Spiral.xml config/Generic_Spiral_SNR.xml)

endif()

add_library(pingvin_mri_noncartesian SHARED
  ${pingvin_mri_noncartesian_header_files}
  ${pingvin_mri_noncartesian_src_files}
  ${pingvin_mri_noncartesian_config_files}
)

set_target_properties(pingvin_mri_noncartesian PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_mri_noncartesian
    pingvin_core
    pingvin_toolbox_log
    pingvin_toolbox_mri_core
    pingvin_mricore
    pingvin_toolbox_cpucore
    pingvin_toolbox_cpufft
    pingvin_toolbox_cpunfft
    pingvin_toolbox_mri_core
    pingvin_toolbox_cpuoperator
    pingvin_toolbox_image_analyze_io


)
if(CUDA_FOUND)
    target_link_libraries(pingvin_mri_noncartesian
	pingvin_toolbox_gpu
	)
endif()

install(FILES
    ${pingvin_mri_noncartesian_header_files}
    DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)

install(TARGETS pingvin_mri_noncartesian
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)

install(FILES ${pingvin_mri_noncartesian_config_files} DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH} COMPONENT main)
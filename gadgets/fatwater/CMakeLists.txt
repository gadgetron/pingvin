add_library(pingvin_fatwater SHARED
  FatWaterGadget.h FatWaterGadget.cpp
)

set_target_properties(pingvin_fatwater PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(
  pingvin_fatwater
  pingvin_core
  pingvin_mricore
  pingvin_toolbox_cpucore
  pingvin_toolbox_cpufft
  pingvin_toolbox_cpucore_math
  pingvin_toolbox_log
  pingvin_toolbox_fatwater
)

install(FILES
  FatWaterGadget.h
  DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)

  install(TARGETS pingvin_fatwater
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)


install(FILES
        config/Generic_Cartesian_Grappa_FatWater.xml
  DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH} COMPONENT main)
add_library(pingvin_cartesian SHARED
  CartesianToGenericGadget.h
  CartesianToGenericGadget.cpp
)

set_target_properties(pingvin_cartesian PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_cartesian pingvin_toolbox_cpucore
  pingvin_core
  pingvin_toolbox_log
  pingvin_mricore
  pingvin_toolbox_cpucore_math
  )


install (FILES  CartesianToGenericGadget.h
                DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)

install(TARGETS pingvin_cartesian
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)
add_library(gadgetron_cartesian SHARED
  CartesianToGenericGadget.h
  CartesianToGenericGadget.cpp
)

set_target_properties(gadgetron_cartesian PROPERTIES VERSION ${GADGETRON_VERSION_STRING} SOVERSION ${GADGETRON_SOVERSION})

target_link_libraries(gadgetron_cartesian gadgetron_toolbox_cpucore
  gadgetron_core
  gadgetron_toolbox_log
  gadgetron_mricore
  gadgetron_toolbox_cpucore_math
  )


install (FILES  CartesianToGenericGadget.h
                DESTINATION ${GADGETRON_INSTALL_INCLUDE_PATH} COMPONENT main)

install(TARGETS gadgetron_cartesian
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)
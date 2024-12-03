add_library(gadgetron_gpuradial SHARED
  gpuRadialPrepGadget.h gpuRadialPrepGadget.cpp
  gpuRadialSensePrepGadget.h gpuRadialSensePrepGadget.cpp
  gpuRadialSpiritPrepGadget.h gpuRadialSpiritPrepGadget.cpp
  gpuRetroGatedSensePrepGadget.h gpuRetroGatedSensePrepGadget.cpp
  RadialPhaseCorrectionGadget.h RadialPhaseCorrectionGadget.cpp
  )

set_target_properties(gadgetron_gpuradial PROPERTIES VERSION ${GADGETRON_VERSION_STRING} SOVERSION ${GADGETRON_SOVERSION})

target_link_libraries(gadgetron_gpuradial
        gadgetron_core
        gadgetron_mricore
        gadgetron_toolbox_log
        gadgetron_gpuparallelmri
        gadgetron_toolbox_cpucore
        gadgetron_toolbox_cpucore_math
        gadgetron_toolbox_gpu)

install (FILES  gpuRadialPrepGadget.h
                gpuRadialSensePrepGadget.h
                gpuRadialSpiritPrepGadget.h
                gpuRetroGatedSensePrepGadget.h
                RadialPhaseCorrectionGadget.h
                DESTINATION ${GADGETRON_INSTALL_INCLUDE_PATH} COMPONENT main)

install(TARGETS gadgetron_gpuradial
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)

add_subdirectory(config)
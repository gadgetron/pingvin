add_library(pingvin_hyper SHARED
  NFFT2DGadget.h NFFT2DGadget.cpp
  CMRT3DGadget.h CMRT3DGadget.cpp
  CMRTGadget.h CMRTGadget.cpp
  CSIGadget.h CSIGadget.cpp
  gpuCSICoilEstimationGadget.cpp

  )

set_target_properties(pingvin_hyper PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_hyper
  pingvin_core
  pingvin_mricore
  pingvin_gpuparallelmri
  pingvin_toolbox_gpu
  pingvin_toolbox_cpucore
  pingvin_toolbox_cpufft
  pingvin_toolbox_cpucore_math)


install(FILES
  NFFT2DGadget.h
  CMRTGadget.h
  CMRT3DGadget.h
  CSIGadget.h
  DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH})

install(FILES NFFT2D.xml CMRT3D.xml CMRT.xml FS-CSI.xml DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH})


install(TARGETS pingvin_hyper
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)
if(CUDA_FOUND)
  if(CPU_REG)
    install (FILES 
      cpureg_cartesian_averaging.xml
      DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH} COMPONENT main)
  endif()
  
  if(GPU_REG)
    install (FILES 
      gpureg_cartesian_averaging.xml
      DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH} COMPONENT main)
  endif()
endif()

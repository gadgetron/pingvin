cmake_policy(SET CMP0028 NEW) # double colon only for imported or alias libraries

# ==============================================================================
# First try to find either the BARTmain library or a path to the BART source code

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)


# ==============================================================================

set(GADGET_FILES
  bartgadget.h
  bartgadget.cpp
  bart_helpers.h
  bart_helpers.cpp
  bart_logger.cpp
  BART_Recon.xml
  BART_Recon_cloud.xml
  BART_Recon_cloud_Standard.xml
)

# ------------------------------------------------------------------------------

if(BART_MEMONLY_CFL)
  add_definitions(-DMEMONLY_CFL)
endif()

add_library(pingvin_bart SHARED ${GADGET_FILES})
target_link_libraries(pingvin_bart BART::BARTMAIN)



set_target_properties(pingvin_bart
  PROPERTIES
  VERSION ${PINGVIN_VERSION_STRING}
  SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_bart pingvin_core
  pingvin_mricore
  pingvin_toolbox_mri_core
  pingvin_toolbox_log
  pingvin_toolbox_cpucore
  pingvin_toolbox_cpufft
  pingvin_toolbox_cpuklt
  )

  target_link_libraries(pingvin_bart pingvin_toolbox_cpucore_math)

install(FILES  bartgadget.h
  DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)

  install(TARGETS pingvin_bart
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)

set(PINGVIN_INSTALL_BART_PATH share/pingvin/bart)
install(FILES Sample_Grappa_Recon.sh Sample_Grappa_Recon_Standard.sh DESTINATION ${PINGVIN_INSTALL_BART_PATH})

install(FILES BART_Recon.xml BART_Recon_cloud.xml BART_Recon_cloud_Standard.xml DESTINATION ${PINGVIN_INSTALL_CONFIG_PATH} COMPONENT main)
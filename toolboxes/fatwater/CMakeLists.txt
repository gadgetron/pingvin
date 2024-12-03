add_library(pingvin_toolbox_fatwater SHARED
  fatwater.h
  fatwater.cpp
        graph_cut.cpp ImageGraph.cpp correct_frequency_shift.h correct_frequency_shift.cpp bounded_field_map.cpp)

set_target_properties(pingvin_toolbox_fatwater PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_toolbox_fatwater
  pingvin_toolbox_log
  pingvin_toolbox_cpucore
  pingvin_toolbox_cpucore_math
  pingvin_toolbox_cpufft
  armadillo
  )

target_include_directories(
    pingvin_toolbox_fatwater
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
  )

install(TARGETS pingvin_toolbox_fatwater
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)

install(FILES
  fatwater.h
  correct_frequency_shift.h
  DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)
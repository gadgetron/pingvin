


add_library(pingvin_toolbox_t1 SHARED
        t1fit.cpp)

set_target_properties(pingvin_toolbox_t1 PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_toolbox_t1
        pingvin_toolbox_log
        pingvin_toolbox_cpucore
        pingvin_toolbox_cpucore_math
		pingvin_toolbox_cpu_solver
		pingvin_toolbox_demons
		pingvin_toolbox_cmr
        armadillo
        )
target_include_directories(pingvin_toolbox_t1
        PUBLIC

                $<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )

install(TARGETS pingvin_toolbox_t1
	EXPORT pingvin-export
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)


install(FILES
        t1fit.h
        DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)

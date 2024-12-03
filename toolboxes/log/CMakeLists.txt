add_library(gadgetron_toolbox_log SHARED log.cpp)
target_include_directories(gadgetron_toolbox_log
		PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
set_target_properties(gadgetron_toolbox_log PROPERTIES VERSION ${GADGETRON_VERSION_STRING} SOVERSION ${GADGETRON_SOVERSION})

install(TARGETS gadgetron_toolbox_log
		EXPORT gadgetron-export
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)

install(FILES log.h DESTINATION ${GADGETRON_INSTALL_INCLUDE_PATH} COMPONENT main)
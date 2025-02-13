add_library(pingvin_core_parallel SHARED
        Branch.h
		Merge.h
		Fanout.h
		UnorderedMerge.h
		UnorderedMerge.cpp)

target_link_libraries(pingvin_core_parallel
        pingvin_core)

set_target_properties(pingvin_core_parallel PROPERTIES
        VERSION ${PINGVIN_VERSION_STRING}
        SOVERSION ${PINGVIN_SOVERSION})

install(TARGETS pingvin_core_parallel
		EXPORT pingvin-export
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)

install(FILES
        Branch.h
        Merge.h
        Fanout.h
        UnorderedMerge.h
        DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)
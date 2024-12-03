add_library(gadgetron_toolbox_demons SHARED demons_registration.cpp)
target_link_libraries(gadgetron_toolbox_demons
    gadgetron_toolbox_cpucore
    gadgetron_toolbox_cpucore_math
)

set_target_properties(gadgetron_toolbox_demons PROPERTIES VERSION ${GADGETRON_VERSION_STRING} SOVERSION ${GADGETRON_SOVERSION})

target_sources(gadgetron_toolbox_demons
    PUBLIC FILE_SET headers TYPE HEADERS
    FILES
        demons_registration.h
)

install(TARGETS gadgetron_toolbox_demons
    EXPORT gadgetron-export
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    FILE_SET headers DESTINATION ${GADGETRON_INSTALL_INCLUDE_PATH}
    INCLUDES DESTINATION ${GADGETRON_INSTALL_INCLUDE_PATH}
)

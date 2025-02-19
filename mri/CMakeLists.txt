add_library(pingvin_mri INTERFACE
    MRContext.h
    MRNode.h
    MRPureNode.h
    MRParallel.h
    MRSource.h
    MRSink.h
    )

target_link_libraries(pingvin_mri INTERFACE
    mrd::mrd
    pingvin_core
    )

target_include_directories(pingvin_mri
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    )

install(TARGETS pingvin_mri
    EXPORT pingvin-export
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    COMPONENT main
    )

install(FILES
    MRContext.h
    MRNode.h
    MRPureNode.h
    MRParallel.h
    MRSource.h
    MRSink.h
    DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH}/mri
    )
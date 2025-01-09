configure_file(../pingvin/pingvin_config.in pingvin_config.h)

add_executable(emperor
    main.cc

        ../pingvin/Config.cpp
        ../pingvin/Config.h

        ../../gadgets/mri_core/NoiseAdjustGadget.cpp
        ../../gadgets/mri_core/RemoveROOversamplingGadget.cpp
        ../../gadgets/mri_core/AcquisitionAccumulateTriggerGadget.cpp
        ../../gadgets/mri_core/BucketToBufferGadget.cpp
        ../../gadgets/mri_core/SimpleReconGadget.cpp
        ../../gadgets/mri_core/ImageArraySplitGadget.cpp
        ../../gadgets/mri_core/ExtractGadget.cpp

        ../pingvin/StreamConsumer.h

        ../pingvin/ErrorHandler.h

        ../pingvin/nodes/Processable.h
        ../pingvin/nodes/Processable.cpp

        ../pingvin/nodes/Stream.cpp
        ../pingvin/nodes/Stream.h
        ../pingvin/nodes/Parallel.cpp
        ../pingvin/nodes/Parallel.h
        ../pingvin/nodes/ParallelProcess.cpp
        ../pingvin/nodes/ParallelProcess.h
        ../pingvin/nodes/PureStream.cpp
        ../pingvin/nodes/PureStream.h
        )

# target_link_libraries(emperor
#         # Boost::system
#         # Boost::filesystem
#         Boost::program_options
# )
target_link_libraries(emperor
        pingvin_core
        pingvin_toolbox_log
        pingvin_toolbox_mri_core
        # pingvin_mricore
        Boost::system
        Boost::filesystem
        Boost::program_options
        ${PUGIXML_LIBRARIES}
        GTBLAS)

target_include_directories(emperor
        PRIVATE
        ${CMAKE_SOURCE_DIR}/apps/pingvin
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        )

install(TARGETS emperor DESTINATION bin COMPONENT main)
if (CUDA_FOUND)
    list(APPEND CPU_GADGETS cpuRegistrationAveragingGadget.cpp)
    list(APPEND CPU_LIBS pingvin_toolbox_cpucore_math pingvin_toolbox_cpureg)


    list(APPEND GPU_GADGETS gpuRegistrationAveragingGadget.cpp gpuRegistrationScatteringGadget.cpp)
    list(APPEND GPU_LIBS pingvin_toolbox_gpu ${CUDA_LIBRARIES})


    add_library(pingvin_moco SHARED
            cpuRegistrationAveragingGadget.h
            gpuRegistrationAveragingGadget.h
            gpuRegistrationScatteringGadget.h
            RegistrationAveragingGadget.h
            RegistrationScatteringGadget.h
            ${CPU_GADGETS}
            ${GPU_GADGETS}
            )

    set_target_properties(pingvin_moco PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

    target_link_libraries(pingvin_moco
            pingvin_core
            pingvin_toolbox_cpucore pingvin_mricore
            pingvin_toolbox_gpu
            )

	install(TARGETS pingvin_moco
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)
    install(TARGETS pingvin_moco DESTINATION lib COMPONENT main)

    install(FILES cpuRegistrationAveragingGadget.h
            gpuRegistrationAveragingGadget.h
            gpuRegistrationScatteringGadget.h
            RegistrationAveragingGadget.h
            RegistrationScatteringGadget.h
            DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)

    add_subdirectory(config)
elseif (CUDA_FOUND)
    message("Cuda not found, not compiling gpu-based registration gadgets")
endif ()
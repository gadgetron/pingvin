set_source_files_properties(CompressedFloatBufferSse41.cpp PROPERTIES COMPILE_FLAGS "-msse4.1")
set_source_files_properties(CompressedFloatBufferAvx2.cpp PROPERTIES COMPILE_FLAGS "-mavx2 -mfma")

set(gadgetron_mricore_header_files
        AugmentImageMetadataGadget.h
        NoiseAdjustGadget.h
        PCACoilGadget.h
        FFTGadget.h
        CombineGadget.h
        ExtractGadget.h
        FloatToFixPointGadget.h
        RemoveROOversamplingGadget.h
        CoilReductionGadget.h
        FlowPhaseSubtractionGadget.h
        PhysioInterpolationGadget.h
        AsymmetricEchoAdjustROGadget.h
        MaxwellCorrectionGadget.h
        ComplexToFloatGadget.h
        AcquisitionAccumulateTriggerGadget.h
        BucketToBufferGadget.h
        ImageArraySplitGadget.h
        SimpleReconGadget.h
        ImageSortGadget.h
        NHLBICompression.h
        cpuisa.h
        ImageArraySendMixin.h
        ImageArraySendMixin.hpp
        AutoScaleGadget.h
        DenoiseGadget.h
        FlagTriggerGadget.h
        ImageIndexGadget.h
        AccumulatorGadget.h
        ScaleGadget.h

        # These Gadgets are NOT TESTED and have not been upgraded to MRD v2
        # CoilComputationGadget.h
        # CropAndCombineGadget.h
        # ImageAccumulatorGadget.h
        # ImageFFTGadget.h
        # ImageResizingGadget.h
        # NoiseAdjustGadget_unoptimized.h
        # PartialFourierAdjustROGadget.h
        )

set(gadgetron_mricore_src_files
        AugmentImageMetadataGadget.cpp
        NoiseAdjustGadget.cpp
        PCACoilGadget.cpp
        FFTGadget.cpp
        CombineGadget.cpp
        ExtractGadget.cpp
        FloatToFixPointGadget.cpp
        RemoveROOversamplingGadget.cpp
        CoilReductionGadget.cpp
        FlowPhaseSubtractionGadget.cpp
        PhysioInterpolationGadget.cpp
        AsymmetricEchoAdjustROGadget.cpp
        MaxwellCorrectionGadget.cpp
        ComplexToFloatGadget.cpp
        AcquisitionAccumulateTriggerGadget.cpp
        BucketToBufferGadget.cpp
        ImageArraySplitGadget.cpp
        SimpleReconGadget.cpp
        ImageSortGadget.cpp
        CompressedFloatBuffer.cpp
        CompressedFloatBufferSse41.cpp
        CompressedFloatBufferAvx2.cpp
        cpuisa.cpp
        AutoScaleGadget.cpp
        DenoiseGadget.cpp
        FlagTriggerGadget.cpp
        ImageIndexGadget.cpp
        AccumulatorGadget.cpp
        ScaleGadget.cpp

        # These Gadgets are NOT TESTED and have not been upgraded to MRD v2
        # CoilComputationGadget.cpp
        # CropAndCombineGadget.cpp
        # ImageAccumulatorGadget.cpp
        # ImageFFTGadget.cpp
        # ImageResizingGadget.cpp
        # NoiseAdjustGadget_unoptimized.cpp
        # PartialFourierAdjustROGadget.cpp
        )

set(gadgetron_mricore_config_files
        config/default.xml
        config/default_optimized.xml
        config/default_measurement_dependencies.xml
        )
set(gadgetron_mricore_generic_recon_gadgets_header_files
        generic_recon_gadgets/GenericReconBase.h
        generic_recon_gadgets/GenericReconGadget.h
        generic_recon_gadgets/GenericReconCartesianGrappaGadget.h
        generic_recon_gadgets/GenericReconCartesianSpiritGadget.h
        generic_recon_gadgets/GenericReconCartesianNonLinearSpirit2DTGadget.h
        generic_recon_gadgets/GenericReconCartesianReferencePrepGadget.h
        generic_recon_gadgets/GenericReconPartialFourierHandlingGadget.h
        generic_recon_gadgets/GenericReconPartialFourierHandlingFilterGadget.h
        generic_recon_gadgets/GenericReconPartialFourierHandlingPOCSGadget.h
        generic_recon_gadgets/GenericReconKSpaceFilteringGadget.h
        generic_recon_gadgets/GenericReconFieldOfViewAdjustmentGadget.h
        generic_recon_gadgets/GenericReconImageArrayScalingGadget.h
        generic_recon_gadgets/GenericReconEigenChannelGadget.h
        generic_recon_gadgets/GenericReconNoiseStdMapComputingGadget.h
        generic_recon_gadgets/GenericReconStreamDef.h

        # These Gadgets are NOT TESTED and have not been upgraded to MRD v2
        # generic_recon_gadgets/GenericReconAccumulateImageTriggerGadget.h
        # generic_recon_gadgets/GenericReconCartesianFFTGadget.h
        # generic_recon_gadgets/GenericReconImageToImageArrayGadget.h
        # generic_recon_gadgets/GenericReconReferenceKSpaceDelayedBufferGadget.h
        # generic_recon_gadgets/GenericImageReconGadget.h
        # generic_recon_gadgets/GenericImageReconArrayToImageGadget.h
)

set(gadgetron_mricore_generic_recon_gadgets_src_files
        generic_recon_gadgets/GenericReconBase.cpp
        generic_recon_gadgets/GenericReconGadget.cpp
        generic_recon_gadgets/GenericReconCartesianGrappaGadget.cpp
        generic_recon_gadgets/GenericReconCartesianSpiritGadget.cpp
        generic_recon_gadgets/GenericReconCartesianNonLinearSpirit2DTGadget.cpp
        generic_recon_gadgets/GenericReconCartesianReferencePrepGadget.cpp
        generic_recon_gadgets/GenericReconPartialFourierHandlingGadget.cpp
        generic_recon_gadgets/GenericReconPartialFourierHandlingFilterGadget.cpp
        generic_recon_gadgets/GenericReconPartialFourierHandlingPOCSGadget.cpp
        generic_recon_gadgets/GenericReconKSpaceFilteringGadget.cpp
        generic_recon_gadgets/GenericReconFieldOfViewAdjustmentGadget.cpp
        generic_recon_gadgets/GenericReconImageArrayScalingGadget.cpp
        generic_recon_gadgets/GenericReconEigenChannelGadget.cpp
        generic_recon_gadgets/GenericReconNoiseStdMapComputingGadget.cpp

        # These Gadgets are NOT TESTED and have not been upgraded to MRD v2
        # generic_recon_gadgets/GenericReconAccumulateImageTriggerGadget.cpp
        # generic_recon_gadgets/GenericReconCartesianFFTGadget.cpp
        # generic_recon_gadgets/GenericReconImageToImageArrayGadget.cpp
        # generic_recon_gadgets/GenericReconReferenceKSpaceDelayedBufferGadget.cpp
        # generic_recon_gadgets/GenericImageReconGadget.cpp
        # generic_recon_gadgets/GenericImageReconArrayToImageGadget.cpp
)

set(gadgetron_mricore_generic_recon_gadgets_config_files
        generic_recon_gadgets/config/Generic_Cartesian_FFT.xml
        generic_recon_gadgets/config/Generic_Cartesian_Grappa_Cine_Denoise.xml
        generic_recon_gadgets/config/Generic_Cartesian_Grappa_Complex.xml
        generic_recon_gadgets/config/Generic_Cartesian_Grappa_EPI_AVE.xml
        generic_recon_gadgets/config/Generic_Cartesian_Grappa_EPI.xml
        generic_recon_gadgets/config/Generic_Cartesian_Grappa_ImageArray.xml
        generic_recon_gadgets/config/Generic_Cartesian_Grappa_RealTimeCine_Cloud.xml
        generic_recon_gadgets/config/Generic_Cartesian_Grappa_RealTimeCine.xml
        generic_recon_gadgets/config/Generic_Cartesian_Grappa_SNR.xml
        generic_recon_gadgets/config/Generic_Cartesian_Grappa_T2W.xml
        generic_recon_gadgets/config/Generic_Cartesian_Grappa.xml
        generic_recon_gadgets/config/Generic_Cartesian_Image_Chain_FFT.xml
        generic_recon_gadgets/config/Generic_Cartesian_NonLinear_Spirit_RealTimeCine_Cloud.xml
        generic_recon_gadgets/config/Generic_Cartesian_NonLinear_Spirit_RealTimeCine.xml
        generic_recon_gadgets/config/Generic_Cartesian_RandomSampling_NonLinear_Spirit_RealTimeCine_Cloud.xml
        generic_recon_gadgets/config/Generic_Cartesian_RandomSampling_NonLinear_Spirit_RealTimeCine.xml
        generic_recon_gadgets/config/Generic_Cartesian_Spirit_RealTimeCine.xml
        generic_recon_gadgets/config/Generic_Cartesian_Spirit_SASHA.xml
        generic_recon_gadgets/config/Generic_Cartesian_Spirit.xml
)

# if there is python, add gadgets using python
if (BUILD_PYTHON_SUPPORT)
    message(STATUS "Build python generic gadgets ... ")

    include_directories(
      ${CMAKE_SOURCE_DIR}/core
      ${CMAKE_SOURCE_DIR}/toolboxes/python
      ${PYTHON_INCLUDE_PATH}
      ${NUMPY_INCLUDE_DIRS}
      )

      set( gadgetron_mricore_generic_recon_gadgets_header_files ${gadgetron_mricore_generic_recon_gadgets_header_files} generic_recon_gadgets/GenericReconCartesianGrappaAIGadget.h)
      set( gadgetron_mricore_generic_recon_gadgets_src_files ${gadgetron_mricore_generic_recon_gadgets_src_files} generic_recon_gadgets/GenericReconCartesianGrappaAIGadget.cpp)
      set( gadgetron_mricore_generic_recon_gadgets_config_files ${gadgetron_mricore_generic_recon_gadgets_config_files} generic_recon_gadgets/config/Generic_Cartesian_Grappa_AI.xml)
      set( gadgetron_python_models_files models/grappa_ai.py )
endif ()

add_library(gadgetron_mricore SHARED
    ${gadgetron_mricore_header_files}
    ${gadgetron_mricore_src_files}
    ${gadgetron_mricore_config_files}
    ${gadgetron_mricore_generic_recon_gadgets_header_files}
    ${gadgetron_mricore_generic_recon_gadgets_src_files}
    ${gadgetron_mricore_generic_recon_gadgets_config_files}
)

set_target_properties(gadgetron_mricore PROPERTIES VERSION ${GADGETRON_VERSION_STRING} SOVERSION ${GADGETRON_SOVERSION})

target_link_libraries(gadgetron_mricore
        gadgetron_core
        gadgetron_toolbox_log
        gadgetron_toolbox_cpucore
        gadgetron_toolbox_cpufft
        gadgetron_toolbox_image_analyze_io
        gadgetron_toolbox_denoise
        gadgetron_toolbox_cpuoperator
        gadgetron_toolbox_cpuklt
        gadgetron_toolbox_mri_core
        )

target_include_directories(gadgetron_mricore
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if (BUILD_PYTHON_SUPPORT)
    target_link_libraries(gadgetron_mricore gadgetron_toolbox_python)

    install(FILES ${gadgetron_python_models_files}
            DESTINATION ${GADGETRON_INSTALL_PYTHON_MODULE_PATH} COMPONENT main)
endif ()

install(FILES
        ${gadgetron_mricore_header_files}
        DESTINATION ${GADGETRON_INSTALL_INCLUDE_PATH} COMPONENT main)

install(FILES
        ${gadgetron_mricore_generic_recon_gadgets_header_files}
        DESTINATION ${GADGETRON_INSTALL_INCLUDE_PATH}/generic_recon_gadgets COMPONENT main)


install(FILES
        ${gadgetron_mricore_config_files}
        ${gadgetron_mricore_generic_recon_gadgets_config_files}
         DESTINATION ${GADGETRON_INSTALL_CONFIG_PATH} COMPONENT main)

install(TARGETS gadgetron_mricore
        EXPORT gadgetron-export
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        COMPONENT main
)
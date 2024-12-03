add_subdirectory(examples)
add_subdirectory(debugging)
add_subdirectory(mri_core)
add_subdirectory(interventional_mri)
add_subdirectory(cmr)
add_subdirectory(epi)
add_subdirectory(spiral)
add_subdirectory(grappa)
add_subdirectory(python)

if (CUDA_FOUND)
  message("Cuda found, compiling gpu accelerated gadgets")
  add_subdirectory(pmri)
  add_subdirectory(radial)
  add_subdirectory(gpu)
else ()
  message("Cuda NOT found, NOT compiling gpu accelerated gadgets")
endif()


# NOTE:
# The remaining gadgets directories below are NOT TESTED and have not yet been upgraded to MRD v2.

# add_subdirectory(fatwater)
# add_subdirectory(T1)
# add_subdirectory(mri_noncartesian)
# add_subdirectory(cartesian)
# add_subdirectory(moco)

# if(BART_FOUND)
#   add_subdirectory(bart)
# else()
#   message("BART not found, NOT compiling bartgadget")
# endif()

# if (CUDA_FOUND)
#   add_subdirectory(hyper)
# endif()

# find_package(DCMTK CONFIG)
# if(DCMTK_FOUND)
#   add_subdirectory(dicom)
# else()
#   message("DCMTK NOT found, not compiling DICOM gadget")
# endif()

# if(PLPLOT_FOUND)
#     if (MKL_FOUND OR ARMADILLO_FOUND)
#         add_subdirectory(plplot)
#     endif ()
# endif()
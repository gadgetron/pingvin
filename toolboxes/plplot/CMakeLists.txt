# pingvin_toolbox_plplot is a toolbox for plotting gadgetron data structures using PLplot

include_directories(${PLPLOT_INCLUDE_DIR})
link_directories(${PLPLOT_LIB_DIR})

set(header_files GtPLplot.h)
set(src_files GtPLplot.cpp)

set(plplot_files ${header_files} ${src_files} )

source_group(plplot FILES ${header_files} ${src_files})

add_library(pingvin_toolbox_plplot SHARED
            ${plplot_files} )

set_target_properties(pingvin_toolbox_plplot PROPERTIES VERSION ${PINGVIN_VERSION_STRING} SOVERSION ${PINGVIN_SOVERSION})

target_link_libraries(pingvin_toolbox_plplot
                      ${PLPLOT_LIBRARIES}
                      pingvin_toolbox_cpucore
                      pingvin_toolbox_log
                      pingvin_toolbox_cpucore_math
                      pingvin_toolbox_mri_core
                      pingvin_toolbox_cpucore_math )

target_include_directories(pingvin_toolbox_plplot
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS pingvin_toolbox_plplot
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)

install (FILES
        ${header_files}
        DESTINATION ${PINGVIN_INSTALL_INCLUDE_PATH} COMPONENT main)
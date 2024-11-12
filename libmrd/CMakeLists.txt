include(FetchContent)
FetchContent_Declare(mrd2
    GIT_REPOSITORY https://github.com/ismrmrd/mrd.git
    GIT_TAG 00667f1f3b9f3c877e67bb5fadc011e9cdc845bb
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(mrd2)

set(HOWARD_HINNANT_DATE_MINIMUM_VERSION "3.0.0")
find_package(date ${HOWARD_HINNANT_DATE_MINIMUM_VERSION} REQUIRED)

set(XTENSOR_MINIMUM_VERSION "0.21.10")
find_package(xtensor ${XTENSOR_MINIMUM_VERSION} REQUIRED)

set(NLOHMANN_JSON_MINIMUM_VERSION "3.11.1")
find_package(nlohmann_json ${NLOHMANN_JSON_MINIMUM_VERSION} REQUIRED)

set(mrd_generated_files
    yardl/yardl.h
    protocols.cc
    types.cc
    binary/protocols.cc
)

set(mrd_model_dir ${mrd2_SOURCE_DIR}/model)
set(mrd_include_dir ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(mrd_generated_dir ${mrd_include_dir}/mrd)

file(GLOB mrd_model_files ${mrd_model_dir}/*.yml)
message(STATUS "mrd_model_files: ${mrd_model_files}")

list(TRANSFORM mrd_generated_files PREPEND ${mrd_generated_dir}/)
message(STATUS "mrd_generated_files: ${mrd_generated_files}")

add_custom_command(
    OUTPUT ${mrd_generated_files}
    COMMAND yardl generate -c cpp.overrideArrayHeader=gadgetron-arrays.h -c cpp.sourcesOutputDir=${mrd_generated_dir}
    WORKING_DIRECTORY ${mrd_model_dir}
    DEPENDS ${mrd_model_files}
    VERBATIM
)

add_library(gadgetron_mrd SHARED ${mrd_generated_files} include/gadgetron-arrays.h)
add_library(mrd::mrd ALIAS gadgetron_mrd)

target_include_directories(gadgetron_mrd
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
    PUBLIC "$<BUILD_INTERFACE:${mrd_include_dir}>"
)

target_link_libraries(gadgetron_mrd
    PUBLIC xtensor
    PUBLIC date::date
    PUBLIC nlohmann_json::nlohmann_json
    gadgetron_toolbox_cpucore
)

install(TARGETS gadgetron_mrd
    EXPORT gadgetron-export
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT main
)

install(DIRECTORY ${mrd_generated_dir}
    TYPE INCLUDE
    COMPONENT Devel
    FILES_MATCHING PATTERN "*.h"
)

#### TODO: Everything below will be removed when mrd is published with intermediate types (e.g. MRD v2.1)
#### TODO: Also need to `just localinstall-python-package`
#### TODO: Once published, add `mrd=2.1` and `mrd-python=2.1` to `environment.yml`

find_package(ISMRMRD CONFIG REQUIRED)

add_executable(
    ismrmrd_to_mrd
    ${mrd2_SOURCE_DIR}/cpp/mrd-tools/ismrmrd_to_mrd.cc
    ${mrd2_SOURCE_DIR}/cpp/mrd-tools/converters.cc
    ${mrd2_SOURCE_DIR}/cpp/mrd/protocols.cc
    ${mrd2_SOURCE_DIR}/cpp/mrd/types.cc
    ${mrd2_SOURCE_DIR}/cpp/mrd/binary/protocols.cc
)

target_include_directories(ismrmrd_to_mrd
    PUBLIC "$<BUILD_INTERFACE:${mrd2_SOURCE_DIR}/cpp/>"
)

target_link_libraries(
    ismrmrd_to_mrd
    ISMRMRD::ISMRMRD
)

add_executable(
    mrd_to_ismrmrd
    ${mrd2_SOURCE_DIR}/cpp/mrd-tools/mrd_to_ismrmrd.cc
    ${mrd2_SOURCE_DIR}/cpp/mrd-tools/converters.cc
    ${mrd2_SOURCE_DIR}/cpp/mrd/protocols.cc
    ${mrd2_SOURCE_DIR}/cpp/mrd/types.cc
    ${mrd2_SOURCE_DIR}/cpp/mrd/binary/protocols.cc
)

target_include_directories(mrd_to_ismrmrd
    PUBLIC "$<BUILD_INTERFACE:${mrd2_SOURCE_DIR}/cpp/>"
)

target_link_libraries(
    mrd_to_ismrmrd
    ISMRMRD::ISMRMRD
)

install(TARGETS
    ismrmrd_to_mrd
    mrd_to_ismrmrd
    DESTINATION bin)


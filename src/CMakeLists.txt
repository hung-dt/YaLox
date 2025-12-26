#------------------------------------------------------------------------------
# Generate build timestamp when building

add_custom_target(gen_buildtime_hpp
    COMMAND "${CMAKE_COMMAND}"
        "-D" "SRC_DIR=${PROJECT_SOURCE_DIR}/src"
        "-D" "OUT_DIR=${CMAKE_CURRENT_BINARY_DIR}"
        "-P" "${PROJECT_SOURCE_DIR}/cmake/GenerateBuildTime.cmake"
    COMMENT "Generate buildtime.hpp header file in ${CMAKE_CURRENT_BINARY_DIR}."
)

#------------------------------------------------------------------------------
# The tree-walk interpreter yalox
add_subdirectory(yalox)
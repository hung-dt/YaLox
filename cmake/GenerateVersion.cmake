# The full version number is in format $MAJOR.$MINOR.$PATCH
find_package(Git QUIET)

set(GIT_TAG "v0.1")
set(GIT_COMMIT_SHA "1234abc")

set(COMPILER_INFO "${CMAKE_CXX_COMPILER_ID} ${CMAKE_BASE_NAME} ${CMAKE_CXX_COMPILER_VERSION}")

configure_file(
    ${PROJECT_SOURCE_DIR}/src/version.hpp.in
    ${CMAKE_BINARY_DIR}/src/version.hpp
    @ONLY
)
message(STATUS "Generate ${CMAKE_BINARY_DIR}/src/version.hpp header file")
# This script runs at build time to generate the buildtime.hpp file
# Input params:
#   SRC_DIR - Directory of the .in file
#   OUT_DIR - Directory to put the generated file.

# Get the current date and time
string(TIMESTAMP BUILD_TIMESTAMP "%b %d %Y, %H:%M:%S %Z")
message(STATUS "Build time: ${BUILD_TIMESTAMP}")

# Configure the template to create the final header file
configure_file (
    ${SRC_DIR}/buildtime.hpp.in
    ${OUT_DIR}/buildtime.hpp
    @ONLY
)

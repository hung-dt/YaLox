# Tests for yalox interpreter
add_executable(test_scanner test_scanner.cpp)
target_include_directories(test_scanner PRIVATE ${PROJECT_SOURCE_DIR}/src/yalox)
target_link_libraries(test_scanner PRIVATE yalox_lib)
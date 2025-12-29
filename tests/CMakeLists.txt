# Tests for yalox interpreter
add_executable(test_scanner test_scanner.cpp)
target_include_directories(test_scanner PRIVATE ${PROJECT_SOURCE_DIR}/src/yalox)
target_link_libraries(test_scanner PRIVATE yalox_lib)
add_test(NAME TestScanner COMMAND test_scanner)

add_executable(test_parser test_parser.cpp)
target_include_directories(test_parser PRIVATE ${PROJECT_SOURCE_DIR}/src/yalox)
target_link_libraries(test_parser PRIVATE yalox_lib)
add_test(NAME TestParser COMMAND test_parser)

add_executable(test_interpreter test_interpreter.cpp)
target_include_directories(test_interpreter PRIVATE ${PROJECT_SOURCE_DIR}/src/yalox)
target_link_libraries(test_interpreter PRIVATE yalox_lib)
add_test(NAME TestInterpreter COMMAND test_interpreter)
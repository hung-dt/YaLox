add_library(yalox_lib SHARED
    yalox.cpp
    token.cpp
    scanner.cpp
)

target_include_directories(yalox_lib PRIVATE ${CMAKE_BINARY_DIR}/src)

add_executable(yalox
    main.cpp
)

add_dependencies(yalox gen_buildtime_hpp)

target_link_libraries(yalox PRIVATE yalox_lib)

set_property(TARGET yalox_lib PROPERTY OUTPUT_NAME yalox)
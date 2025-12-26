add_executable(yalox
    main.cpp
)

add_dependencies(yalox gen_buildtime_hpp)

target_include_directories(yalox PRIVATE ${CMAKE_BINARY_DIR}/src)

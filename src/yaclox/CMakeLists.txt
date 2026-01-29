add_executable(yaclox
    chunk.c
    memory.c
    debug.c
    value.c
    main.c
)

set_target_properties(yaclox
    PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
)
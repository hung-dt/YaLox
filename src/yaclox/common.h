#ifndef clox_common_h
#define clox_common_h

// A handful of types and constants (NULL, size_t, bool, uint8_t)
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEBUG_TRACE_EXECUTION

// Some error codes that match with sysexits.h
#define ERR_USAGE 64     // command line usage error
#define ERR_DATAERR 65   // data format error
#define ERR_SOFTWARE 70  // internal software error
#define ERR_IOERR 74     // input/output error

#endif  // !clox_common_h

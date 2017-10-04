#ifndef IFJ17_COMPILER_DEBUG_H
#define IFJ17_COMPILER_DEBUG_H

#include <stdio.h>

#ifndef NDEBUG

#define debugs(s) fprintf(stderr, "%s:%d: %s", __FILE__, __LINE__, s);
#define debug(format, ...) fprintf(stderr, format, __VA_ARGS__);

#else

#define debugs(s)
#define debug(format, ...)

#endif

#endif //IFJ17_COMPILER_DEBUG_H

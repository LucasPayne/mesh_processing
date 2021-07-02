#ifndef MESH_PROCESSING_LOGGING_H
#define MESH_PROCESSING_LOGGING_H

#include <stdio.h>
#include <stdarg.h>


[[maybe_unused]] static void log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf("[mesh_processing] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

[[maybe_unused]] static void log_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf("[mesh_processing, error] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

#endif // MESH_PROCESSING_LOGGING_H

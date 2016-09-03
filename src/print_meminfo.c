// vim:ts=4:sw=4:expandtab
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <yajl/yajl_gen.h>
#include <yajl/yajl_version.h>

#include "i3status.h"

/*
 * Reads the CPU utilization from /proc/stat and returns the usage as a
 * percentage.
 *
 */
void print_meminfo(yajl_gen json_gen, char *buffer, const char *format, int max_threshold) {
    const char *walk;
    char *outwalk = buffer;
    int mem_total = 0, mem_free = 0, mem_available = 0, mem_usage = 0;

#if defined(LINUX)
    static char statpath[512];
    char buf[1024];
    strcpy(statpath, "/proc/meminfo");
    if (!slurp(statpath, buf, sizeof(buf)) ||
        sscanf(buf, "MemTotal: %d kB MemFree: %d kB MemAvailable: %d", &mem_total, &mem_free, &mem_available) != 3)
        goto error;

    mem_usage = 100 * (mem_total - mem_available) / mem_total;
#else
    goto error;
#endif
    for (walk = format; *walk != '\0'; walk++) {
        if (*walk != '%') {
            *(outwalk++) = *walk;
            continue;
        }

        if (BEGINS_WITH(walk + 1, "usage")) {
            if (mem_usage >= max_threshold)
                START_COLOR("color_bad");
            else
                START_COLOR("color_good");
            outwalk += sprintf(outwalk, "%02d%%", mem_usage);
            END_COLOR;
            walk += strlen("usage");
        }
    }

    OUTPUT_FULL_TEXT(buffer);
    return;
error:
    OUTPUT_FULL_TEXT("cant read cpu usage");
    (void)fputs("i3status: Cannot read CPU usage\n", stderr);
}

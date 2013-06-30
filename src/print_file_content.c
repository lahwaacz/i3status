// vim:ts=8:expandtab
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <yajl/yajl_gen.h>
#include <yajl/yajl_version.h>

#include "i3status.h"

/*
 * Reads specified file containing integer value.
 *
 */
void print_file_content_int(yajl_gen json_gen, char *buffer, const char *path, const char *format, int threshold) {
    char *outwalk = buffer;
    const char *walk;

    INSTANCE(path);

    for (walk = format; *walk != '\0'; walk++) {
        if (*walk != '%') {
            *(outwalk++) = *walk;
            continue;
        }

        if (BEGINS_WITH(walk + 1, "value")) {
            static char buf[16];
            long int tmp;
            if (!slurp(path, buf, sizeof(buf)))
                goto error;
            tmp = strtol(buf, NULL, 10);
            if (tmp == LONG_MIN || tmp == LONG_MAX || tmp < 0)
                *(outwalk++) = '?';
            else {
                if (tmp >= threshold)
                    START_COLOR("color_bad");
                else
                    START_COLOR("color_good");
                outwalk += sprintf(outwalk, "%ld", tmp);
                END_COLOR;
            }
            walk += strlen("value");
        }
    }
    OUTPUT_FULL_TEXT(buffer);
    return;
error:
    OUTPUT_FULL_TEXT("cant read custom file");
    (void)fputs("i3status: Cannot read custom file. Make sure the file exists and contains integer value.\n", stderr);
}

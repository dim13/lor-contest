#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * undebugq(char *in, char *needle) {
    char *p = in;
    size_t c = 0, l = strlen(in), nl = strlen(needle);

    for (; *p; c++) {
        for (; *(p) == ' '; (p)++);
        if (*p == *needle) {
            if (l < c + nl) { return in; }
            if (! strncmp(p, needle, nl)) {
                if (p[nl] == ' ') {
                  memmove(p, p + nl + 1, l - nl + 1); // we need to copy \0
                  l -= nl;
                  continue;
                } else if (p[nl] == '\0') {
                  *p = '\0';
                  return in;
                }
            }
        }
        for (; *p != ' ' && *p; p++);
    }
    return in;
}

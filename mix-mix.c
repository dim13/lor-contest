#include <stdlib.h>
#include <string.h>

char *strcutm(char *str, const char *cut)
{
    char *token, *rest = str;
    size_t slen = strlen(str);
    size_t clen = strlen(cut);

    if (!clen) return str;
    while ((token = strsep(&rest, " \t"))) {
        size_t tlen = rest - token - 1;
        if (!strcmp(token, cut)) {
            slen -= clen + 1;
            tlen  = slen - (token - str) + 1;
            rest  = memmove(token, rest, tlen);
        } else if (!rest) continue;
        token[tlen] = token[tlen] ? '\0' : ' ';
    }

    return str;
}

/*
 * Copyright (c) 2014 wota
 *
 * https://www.linux.org.ru/forum/development/10349962?cid=10350284
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static void
strremove(char *s, const char *t)
{
	size_t l = strlen(t);
	char *d = s;

	do
		while (!strncmp(s, t, l))
			s += l;
	while ((*d++ = *s++));
}

char *
strremove_wrapper(char *hay, char *needle)
{
	strremove(hay, needle);

	return hay;
}

static void
remove_word(char *s, const char *w)
{
	int l = strlen(w);
	char *p = s;

	while ((p = strstr(p, w))) {
		if ((p == s || isspace(p[-1])) && (!p[l] || isspace(p[l])))
			memset(p, ' ', l);

		p += l;
	}
}

char *
remove_word_wrapper(char *hay, char *needle)
{
	remove_word(hay, needle);

	return hay;
}

extern char *strchrnul (const char *__s, int __c);

static void
undebug_wota(char *s)
{
#if __linux__
    char *p = s;
    for (; *p && (p = strchrnul(s, ' ')); s = p + 1) {
        if (p - s == 5 && (*((int*)s) & 0x000000FFFFFFFFFF) == 0x75626564 && s[4]=='g')
            memset(s, ' ', 5);
    }
#else
    long v = *((long *)s);
    if ((v & 0x000000FFFFFFFFFF) == 0x6775626564) {
        char s5 = (v & 0x0000FF0000000000) >> 40;
        if (s5) {
            if (s5 == 32)
                memset(s, ' ', 5);

            s += 6;
        } else {
            *s = 0;
            return;
        }
    }
    while ((s = strchr(++s, ' '))) {
        v = *((long *)s);
        if ((v & 0x0000FFFFFFFFFFFF) == 0x677562656420) {
            char s6 = (v & 0x00FF000000000000) >> 48;
            if (!s6) {
                *s = 0;
                break;
            } else if (s6 == ' ') {
                memset(s + 1, ' ', 5);
                s += 6;
            }
        }
    }
#endif
}

char *
undebug_wota_wrapper(char *hay, char *needle)
{
	undebug_wota(hay);

	return hay;
}

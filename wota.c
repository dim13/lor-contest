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

void
undebug_wota(char *s)
{
    char* p;

    for( ; ( p = strchrnul( s, ' ' ) ) ; s = p + 1 )
    {
        if( p - s != 5 )
        {
            if( !*p )
                break;

            continue;
        }

        long v = *((long*)s);
        if( ( v & 0x000000FFFFFFFFFF ) != 0x6775626564 )
        {
            if( !*p )
                break;
        }

        memset( s, ' ', 5 );

        if( !*p )
            break;
    }
}

char *
undebug_wota_wrapper(char *hay, char *needle)
{
	undebug_wota(hay);

	return hay;
}

/*
 * Copyright (c) 2014 Dimitri Sokolyuk <demon@dim13.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
cutout_orig(char *hay, char *needle)
{
	char *p, *s = hay;

	while ((s = strstr(s, needle)) != NULL) {
		if ((p = strchr(s, ' ')) != NULL)
			memmove(s, p + 1, strlen(p));
		else
			*s = '\0';
	}

	return hay;
}

char *
cutout(char *hay, char *needle)
{
	char *p, *s = hay;
	int match;

	while ((p = strchr(s, ' ')) != NULL) {
		*p = '\0';
		match = strcmp(s, needle);
		*p = ' ';

		if (match == 0)
			memmove(s, p + 1, strlen(p));
		else
			s = p + 1;
	}

	if (strcmp(s, needle) == 0)
		*s = '\0';

	return hay;
}

/*
 * "debug" => ""
 * "debugfs" => "debugfs"
 * "debug=1" => "debug=1"
 * "debug systemd.debug" => " systemd.debug"
 * "debug 123 debug 456" => " 123 456"
 */

char *
undebug(char *hay, char *needle)
{
	char *n, *s = hay;
	size_t len = strlen(needle);
	char *end = strchr(hay, '\0');

	while ((s = strstr(s, needle)) != NULL) {
		n = s + len;
		if ((*n == '\0' || *n++ == ' ')
		&& (s == hay || *(s - 1) == ' '))
			memmove(s, n, end - n + 1);
		else
			s = n;
	}

	return hay;
}

char *
split(char *hay, char *needle)
{
	char *p, *s = strdup(hay);
	size_t len = strlen(hay);
	int first = 1;

	while ((p = strsep(&hay, " ")) != NULL) {
		if (strcmp(p, needle) != 0) {
			if (first) {
				first = 0;
				strncpy(s, p, len);
			} else {
				strncat(s, " ", len);
				strncat(s, p, len);
			}
		}
	}

	return s;
}

/* FSM */
char *
wipeout(char *hay, char *needle)
{
	char *p, *s = hay;
	int inspace = 0;
	int state = 0;

	/* FSM: whiteout needle */
	do {
		if  (state == -1) {
			if (*s == ' ')
				state = 0;
		} else {
			if  (needle[state] == '\0') {
				if (*s == ' ' || *s == '\0')
					for (p = s; state > 0; state--)
						*--p = ' ';
				else
					state = -1;
			} else {
				if (needle[state] == *s)
					state++;
				else if (*s == ' ')
					state = 0;
				else
					state = -1;
			}
		}
	} while (*s++);

	/* collapse spaces */
	p = s = hay;
	while (*s && *s == ' ')
		s++;

	do {
		if (*s != ' ' || !inspace)
			*p++ = *s;
		inspace = *s++ == ' ';
	} while (*s);

	do
		*p = '\0';
	while (*--p == ' ');

	return hay;
}

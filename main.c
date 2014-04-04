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

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* rules: https://www.linux.org.ru/forum/development/10349962?cid=10352344 */

struct test {
	char *string;
	char *target;
	char *needle;
} *tc, testcases[] = {
	{ "debug", "", "debug" },
	{ "debugfs", "debugfs", "debug" },
	{ "debug=1", "debug=1", "debug" },
	{ "systemd.debug", "systemd.debug", "debug" },
	{ "debug 123 debug 456", "123 456", "debug" },
	{ "debug debugfs debug debug=1 systemd.debug debug", "debugfs debug=1 systemd.debug", "debug" },
    { "debu", "debu", "debug" },
	{ NULL }
};

int rounds = 100000;
int passes = 20;

char *cutout(char *, char *);
char *undebug(char *, char *);
char *split(char *, char *);
char *delsubstr(char *, char *);
char *process_wrapper(char *, char *);
char *strdel_wrapper(char *, char *);
char *remove_string(char *, char *);
char *remove_string_2(char *, char *);
char *wordstrips(char *, char *);
char *cut(char *, char *);
char *cut2(char *, char *);
char *strremove_wrapper(char *, char *);
char *remove_word_wrapper(char *, char *);
char *strcut_wrapper(char *, char *);
char *undebugq(char *, char *);

struct part {
	char *name;
	char *(*f) (char *, char *);
	char *fname;
	int pass;
	double time;
	double grostime;
	int passed;
}   *p, part[] = {
	{ .name = "beastie", .fname = "cutout", .f = &cutout },
	{ .name = "beastie", .fname = "undebug", .f = &undebug },
	{ .name = "beastie", .fname = "split", .f = &split },
	{ .name = "Eddy_Em", .fname = "delsubstr", .f = &delsubstr },
	{ .name = "Gvidon", .fname = "process", .f = &process_wrapper },
	{ .name = "KennyMinigun", .fname = "strdel", .f = &strdel_wrapper },
	{ .name = "nokachi", .fname = "remove", .f = &remove_string },
	{ .name = "qulinxao", .fname = "wordstrings", .f = &wordstrips },
	{ .name = "true_admin", .fname = "cut", .f = &cut },
	{ .name = "true_admin", .fname = "cut2", .f = &cut2 },
	{ .name = "wota", .fname = "strremove", .f = &strremove_wrapper },
	{ .name = "wota", .fname = "remove_word", .f = &remove_word_wrapper },
	{ .name = "anonymous", .fname = "strcut", .f = &strcut_wrapper },
	{ .name = "qnikst", .fname = "undebugq", .f = &undebugq },
	{ NULL },
};

char *passstat[] = {"fail", "pass"};

/* strip whitespaces */
char *
strip(char *s)
{
	char *r, *p;

	r = p = s;
	while (*s && *p) {
		if (*p == ' ')
			p++;
		else
			*s++ = *p++;
	}
	*s = '\0';

	return r;
}


void
go(struct part *p, char *s, char *n)
{
	char *r, *o;

	r = s = strdup(s);

	/* fire! */
	o = p->f(s, n);

	/* free internal allocated space */
	if (o != s)
		free(o);

	free(r);
}

void
prepare(struct part *p, struct test *test)
{
	char *s, *t, *o;

	s = strdup(test->string);
	t = strdup(test->target);
	o = p->f(s, test->needle);

	/* remove whitespaces from output for compare */
	p->pass = !strcmp(strip(o), strip(t));
	p->passed += p->pass;

	if (o != s)
		free(o);
	free(s);
	free(t);
}

void
runtest(struct part *p, struct test *t)
{
	struct timeval begin, end;
	int i, k;

 	prepare(p, t);

	fprintf(stderr, "%16s ", p->fname);
	p->time = 0.0;
	for (k = 0; k < passes; k++) {
		fprintf(stderr, ".");
		gettimeofday(&begin, NULL);
		for (i = 0; i < rounds; i++)
			go(p, t->string, t->needle);
		gettimeofday(&end, NULL);
		p->time += (end.tv_sec - begin.tv_sec) * 1000.0;
		p->time += (end.tv_usec - begin.tv_usec) / 1000.0;
	}
	/* average time */
	p->time /= passes;
	p->grostime += p->time;
	fprintf(stderr, "%5s%9.2f ms\n", passstat[p->pass], p->time);
}

void
result(struct part *p)
{
	struct part *z;
	double minval = 100000.0;

	for (z = p; z->name != NULL; z++)
		if (z->pass && z->grostime < minval)
			minval = z->grostime;

	printf("\nGros Relults\n----\n\n");
	printf("%-16s| %-16s| %-12s| %-12s| %-12s\n",
	    "name", "func name", "passed", "gros time", "slower");
	printf("%-16s| %-16s| %-12s| %-12s| %-12s\n",
	    "---", "---", "---", "---", "---");
	for (; p->name != NULL; p++)
		printf("%-16s| %-16s|%8d |%8.2f ms | %9.2f %% \n",
		    p->name, p->fname, p->passed,
		    p->grostime, 100.0 * (p->grostime - minval) / minval);
}

int
main(int argc, char **argv)
{
	int i;

	for (tc = testcases; tc->string; tc++) {
		fprintf(stderr, "\n%16s \"%s\"\n", "input", tc->string);
		for (p = part; p->name; p++)
			runtest(p, tc);
	}

	result(part);

	return 0;
}

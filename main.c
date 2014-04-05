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
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>
#if MTRACE
#include <mcheck.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* settings */

int rounds = 500000;
int passes = 20;

/* rules: https://www.linux.org.ru/forum/development/10349962?cid=10352344 */

struct test {
	char *string;
	char *target;
	char *needle;
	int contest;
} testcases[] = {
	{ "debug", "", "debug" },
	{ "debug debug", " ", "debug" },
	{ "debu", "debu", "debug" },
	{ "debugfs", "debugfs", "debug" },
	{ "debug=1", "debug=1", "debug" },
	{ "systemd.debug", "systemd.debug", "debug" },
	{ "systemd.debug", "", "systemd.debug" }, //do we handle different strings?
	{ "debug 123 debug 456", "123 456", "debug" },
	{ "debug debugfs debug debug=1 systemd.debug debug", "debugfs debug=1 systemd.debug", "debug" },
    // end of the string
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro debug",
	  "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro", "debug" },
    // begining of the string
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug dolvm debug=UUID=42debug5-6ee1-464c-bc41-debug42debug debug ro",
	  "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 dolvm debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro", "debug" },
    // multiple times
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug dolvm debug debug=UUID=42debug5-6ee1-464c-bc41-debug42debug debug ro",
	  "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 dolvm debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro", "debug" },
    // long needle middle
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro debug",
	  "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 ro debug", "debug=UUID=42debug5-6ee1-464c-bc41-debug42debug" },
    // long needle end
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 ro debug debug=UUID=42debug5-6ee1-464c-bc41-debug42debug",
	  "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 ro debug", "debug=UUID=42debug5-6ee1-464c-bc41-debug42debug"},
    // long needle beging
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro debug",
	  "debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro debug", "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64" },
    // no remove short
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro debuq",
	  "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro debuq", "debug" },
    // no remove long
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debuq ro debug",
	  "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debuq ro debug", "debug=UUID=42debug5-6ee1-464c-bc41-debug42debug" },
	{ NULL }
};

char *nop(char *, char *);
char *cutout(char *, char *);
char *cutout_orig(char *, char *);
char *undebug(char *, char *);
char *split(char *, char *);
char *wipeout(char *, char *);
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
char *str_drop_str(char *, char *);
char *str_mask_str(char *, char *);
char *undebugq(char *, char *);
char *undebugq2(char *, char *);
char *undebugq3(char *, char *);
char *undebugq_ker(char *, char *);
char *strcutm(char *, char *);
char *carb_wrapper(char *, char *);
char *anon_strcut(char *, char *);
char *undebug_wota_wrapper(char *, char *);
char *anon_wipedebug(char *, char *);

struct part {
	char *name;
	char *(*f) (char *, char *);
	char *fname;
	int pass;
	double time;
	double grostime;
	int passed;
	char pchart[16];
} part[] = {
	{ .name = "nop", .fname = "nop", .f = &nop },
	{ .name = "beastie", .fname = "cutout", .f = &cutout },
	{ .name = "beastie", .fname = "cutout_orig", .f = &cutout_orig },
	{ .name = "beastie", .fname = "undebug", .f = &undebug },
	{ .name = "beastie", .fname = "split", .f = &split },
	{ .name = "beastie", .fname = "wipeout", .f = &wipeout },
	{ .name = "Eddy_Em", .fname = "delsubstr", .f = &delsubstr },
	{ .name = "Gvidon", .fname = "process", .f = &process_wrapper },
	{ .name = "KennyMinigun", .fname = "strdel", .f = &strdel_wrapper },
	{ .name = "nokachi", .fname = "remove", .f = &remove_string },
	{ .name = "qulinxao", .fname = "wordstrings", .f = &wordstrips },
	{ .name = "true_admin", .fname = "cut", .f = &cut },
	{ .name = "true_admin", .fname = "cut2", .f = &cut2 },
	{ .name = "wota", .fname = "strremove", .f = &strremove_wrapper },
	{ .name = "wota", .fname = "remove_word", .f = &remove_word_wrapper },
	{ .name = "wota", .fname = "undebug_wota", .f = &undebug_wota_wrapper },
	{ .name = "anonymous", .fname = "strcut", .f = &strcut_wrapper },
	{ .name = "anonymous", .fname = "anon_strcut", .f = &anon_strcut},
	{ .name = "anonymous", .fname = "anon_wipedebug", .f = &anon_wipedebug},
	{ .name = "puzan", .fname = "str_drop_str", .f = &str_drop_str },
	{ .name = "puzan", .fname = "str_mask_str", .f = &str_mask_str },
	{ .name = "qnikst", .fname = "undebugq uspace", .f = &undebugq },
	{ .name = "qnikst", .fname = "undebugq2", .f = &undebugq2 },
	{ .name = "qnikst", .fname = "undebugq3", .f = &undebugq3 },
	{ .name = "qnikst", .fname = "undebugq", .f = &undebugq_ker },
	{ .name = "mix-mix", .fname = "strcut", .f = &strcutm },
	{ .name = "Carb", .fname = "debugdel", .f = &carb_wrapper },
	{ NULL },
};

/* collapse whitespaces */
static char *
strip(char *p)
{
	char *d, *s;
	int inspace = 0;

	d = s = p;
	while (*s && *s == ' ')
		s++;
	do {
		if (*s != ' ' || !inspace)
			*d++ = *s;
		inspace = *s++ == ' ';
	} while (*s);
	do
		*d = '\0';
	while (*--d == ' ');

	return p;
}


static void
go(struct part *p, char *s, char *n)
{
	char *o;

	s = strdup(s);

	/* fire! */
	o = p->f(s, n);

	/* free internal allocated space */
	if (o != s)
		free(o);

	free(s);
}

static void
prepare(struct part *p, struct test *test)
{
	char *s, *t, *o;

	s = strdup(test->string);
	t = strdup(test->target);
	o = p->f(s, test->needle);

	/* remove whitespaces from output for compare */
	p->pass = !strcmp(strip(o), strip(t));
	p->passed += p->pass;
	strncat(p->pchart, p->pass ? "*" : " ", 1);

	if (o != s)
		free(o);
	free(s);
	free(t);
}

static void
runtest(struct part *p, struct test *t)
{
	char *stat[] = {"fail", "pass"};
	struct timeval begin, end;
	int i, k;
	double time;

 	prepare(p, t);
#if MTRACE || GCOV
	return;
#endif

	fprintf(stderr, "%16s ", p->fname);
	p->time = 1000000.0;

	for (k = 0; k < passes; k++) {
		fprintf(stderr, ".");

		gettimeofday(&begin, NULL);
		for (i = 0; i < rounds; i++)
			go(p, t->string, t->needle);
		gettimeofday(&end, NULL);

		time = (end.tv_sec - begin.tv_sec) * 1000.0;
		time += (end.tv_usec - begin.tv_usec) / 1000.0;

		/* count min time only */
		if (time < p->time)
			p->time = time;
	}

	if (t->contest)
		p->grostime += p->time;

	fprintf(stderr, "%5s%9.2f ms\n", stat[p->pass], p->time);
}

void
spawn(struct part *p, struct test *t)
{
	pid_t pid;
	int stat, fd[2];

	pipe(fd);

	switch ((pid = fork())) {
	case -1:
		warnx("forkin of %s failed", p->fname);
		break;
	case 0:		/* child */
		close(fd[0]);
		runtest(p, t);
		write(fd[1], p, sizeof(struct part));
		exit(0);
		break;
	default:	/* parent */
		close(fd[1]);
		read(fd[0], p, sizeof(struct part));
		waitpid(pid, &stat, 0);
		if (WEXITSTATUS(stat) != 0)
			warnx("%s died with %d", p->fname, WEXITSTATUS(stat));
		break;
	}
}

static void
result(struct part *p, char *user)
{
	struct part *z;
	double minval = 1000000.0;

	for (z = p; z->name != NULL; z++)
		if (strcmp(z->fname, "nop") != 0 && z->grostime < minval) {
			if (user) {
				if (strcmp(user, z->name) == 0)
					minval = z->grostime;
			} else
				minval = z->grostime;
		}

	printf("\nGros Relults\n----\n\n");
	printf("%-16s | %-16s | %-16s | %-12s | %-12s\n",
	    "name", "func name", "passed", "gros time", "slower");
	printf("%-16s | %-16s | %-16s | %-12s | %-12s\n",
	    "---", "---", "---", "---", "---");
	for (; p->name != NULL; p++) {
		if (user && strcmp(user, p->name) != 0)
			continue;
		printf("%-16s | %-16s | %-16s | %9.2f ms | %9.2f %% \n",
		    p->name, p->fname, p->pchart,
		    p->grostime, 100.0 * (p->grostime - minval) / minval);
	}
}

void
usage()
{
	extern char *__progname;

	fprintf(stderr, "Usage: %s [-r <rounds>] [-p <passes per round>] [-u <user>]\n", __progname);
	exit(1);
}

int
main(int argc, char **argv)
{
	struct part *p;
	struct test *t;
	char *contest[] = { "testing", "contest" };
	char *user = NULL;
	int ch;

	while ((ch = getopt(argc, argv, "r:p:u:h")) != -1)
		switch (ch) {
		case 'r':
			rounds = atoi(optarg);
			break;
		case 'p':
			passes = atoi(optarg);
			break;
		case 'u':
			user = strdup(optarg);
			break;
		case 'h':
		default:
			usage();
			break;
		}

	argc -= optind;
	argv += optind;

#if MTRACE
	mtrace();
#endif
	for (t = testcases; t->string; t++) {
		t->contest = strlen(t->string) > 40;
		fprintf(stderr, "\n%-10s%6s \"%s\"\n",
		    contest[t->contest], "input", t->string);
		for (p = part; p->name; p++) {
			if (user && strcmp(user, p->name) != 0)
				continue;
			spawn(p, t);
		}
	}

#if MTRACE
	muntrace();
#endif

#if !GCOV
	result(part, user);
#endif

	if (user)
		free(user);

	return 0;
}

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedefs.h"
#include "functions.h"

static char *argv0;

static void
usage(void)
{
	weprintf("usage: %s < code.sten\n"
	         "       %s file ...\n"
	         "       %s -h", argv0, argv0, argv0);
	exit(2);
}

int
main(int argc, char *argv[])
{
	argv0 = argv[0];
	FILE *in;

	if (argc < 2) {
		symtabzero();
		yyparse();
		return (0);
	}

	if (!strcmp(argv[1], "-h"))
		usage();

	for (argc--; argc > 0; argc--) {
		in = freopen(argv[argc], "r", stdin);
		if (in == NULL) {
			weprintf("freopen: %s:", argv[argc]);
			exit(1);
		}
		symtabzero();
		yyparse();
	}

	return (0);
}

/* weprintf from The Practice of Programming by Brian W. Kernighan and Rob Pike */
void
weprintf(const char *fmt, ...)
{
	va_list ap;
	if (strncmp(fmt, "usage", strlen("usage")))
		fprintf(stderr, "%s: ", argv0);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	if ((fmt[0] != '\0') && (fmt[strlen(fmt)-1] == ':'))
		fprintf(stderr, " %s", strerror(errno));
	fprintf(stderr, "\n");
}

void *
xcalloc(size_t nmemb, size_t size)
{
	void *p = calloc(nmemb, size);
	if (!p) {
		weprintf("Memory allocation failure");
		exit(1);
	}
	return (p);
}

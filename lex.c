#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "typedefs.h"
#include "functions.h"
#include "grammar.h"

int
yylex(void)
{
	int c, i;
	/* tokens */
	int cval;
	Name id;

	while (isspace(c = getchar()))
		;
	if (isdigit(c)) {
		cval = c - '0';
		while (isdigit(c = getchar()))
			cval = cval * 10 + c - '0';
		ungetc(c, stdin);
		yylval.cval = cval;
		return (CONST);
	}
	if (isalpha(c) || (c == '_')) {
		i = 0;
		id[i++] = c;
		c = getchar();
		for (; isalpha(c) || isdigit(c) || (c == '_'); c = getchar()) {
			if (i < (ISIZE-1))
				id[i++] = c;
		}
		id[i] = '\0';
		ungetc(c, stdin);
		if (!strcmp(id, "int"))      return (LINT);
		if (!strcmp(id, "stencil"))  return (LSTENCIL);
		if (!strcmp(id, "if"))       return (LIF);
		if (!strcmp(id, "else"))     return (LELSE);
		if (!strcmp(id, "while"))    return (LWHILE);
		if (!strcmp(id, "for"))      return (LFOR);
		if (!strcmp(id, "do"))       return (LDO);
		memcpy(yylval.id, id, ISIZE);
		return (ID);
	}
	if ((c > ' ') && (c < '~')) {
		id[0] = c;
		id[1] = getchar();
		id[2] = '\0';
		if (!strcmp(id, "//")) {
			while ((c = getchar()) != '\n')
				;
			return (yylex());
		}
		if (!strcmp(id, "||"))  return (LOR);
		if (!strcmp(id, "&&"))  return (LAND);
		if (!strcmp(id, "=="))  return (LEQ);
		if (!strcmp(id, "!="))  return (LNE);
		if (!strcmp(id, "<="))  return (LLE);
		if (!strcmp(id, ">="))  return (LGE);
		if (!strcmp(id, "++"))  return (LINC);
		if (!strcmp(id, "--"))  return (LDEC);
		ungetc(id[1], stdin);
	}

	return (c);
}

void
yyerror(char *s)
{
	weprintf("%s", s);
}

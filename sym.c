#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedefs.h"
#include "functions.h"

#define NEWSYM() xcalloc(1, sizeof(Symbol))

Symbol *SymTab[SYMHASH];

static int tmpi = 1;

/* x(x+2^7-1)(x+2^11-1) = 2^31-1-xx(x-2^7) */
static int
hash(const char *s)
{
	int h = 0;
	for (; *s != '\0'; s++)
		h = ((h+743) * (*s+739) * 733) % (SYMHASH/4);
	return (h);
}

void
symtabzero(void)
{
	memset(SymTab, 0, sizeof(Symbol *) * SYMHASH);
}

char *
symstrdup(Symbol *sym)
{
	char buf[ISIZE];
	switch (sym->type) {
	case SVAR:
		snprintf(buf, ISIZE-1, "%s", sym->u.id);
		break;
	case SARRAY:
		snprintf(buf, ISIZE-1, "%s", sym->u.arr.id);
		break;
	case SCONST:
		snprintf(buf, ISIZE-1, "%d", sym->u.cval);
		break;
	case STEMP:
		snprintf(buf, ISIZE-1, "$t%d", sym->u.tnum % 10);
		break;
	default:
		weprintf("symstrdup: %d: Symbol type error", sym->type);
	}
	return (strdup(buf));
}

Symbol *
symtemp(void)
{
	int h;

	h = (SYMHASH/2) + (tmpi % (SYMHASH/2));
	if (SymTab[h]) {
		weprintf("%d: Symbol collision", tmpi);
		exit(1);
	}

	SymTab[h] = NEWSYM();
	SymTab[h]->type = STEMP;
	SymTab[h]->u.tnum = tmpi++;
	return (SymTab[h]);
}

Symbol *
symconst(int cval)
{
	int h;
	Name id;

	snprintf(id, ISIZE-1, "%d", cval);
	h = (SYMHASH/4) + hash(id);
	if (SymTab[h])
		return (SymTab[h]);

	SymTab[h] = NEWSYM();
	SymTab[h]->type = SCONST;
	SymTab[h]->u.cval = cval;
	return (SymTab[h]);
}

Symbol *
symdeclar(Name id, ARNode *array)
{
	int h;

	id[ISIZE-1] = '\0';
	h = hash(id);
	if (SymTab[h]) {
		weprintf("%s: Already defined symbol", id);
		exit(1);
	}

	SymTab[h] = NEWSYM();
	if (array != NULL) {
		SymTab[h]->type = SARRAY;
		memcpy(SymTab[h]->u.arr.id, id, ISIZE);
		SymTab[h]->u.arr.node = array;
	} else {
		SymTab[h]->type = SVAR;
		memcpy(SymTab[h]->u.id, id, ISIZE);
	}
	return (SymTab[h]);
}

Symbol *
symget(Name id)
{
	int h;

	id[ISIZE-1] = '\0';
	h = hash(id);
	if (!SymTab[h]) {
		weprintf("%s: Undefined symbol", id);
		exit(1);
	}
	return (SymTab[h]);
}

ARNode *
symanode(int size, ARNode *next)
{
	ARNode *node = xcalloc(1, sizeof(ARNode));
	node->size = size;
	node->next = next;
	return (node);
}

void
symtabprint(void)
{
	int i;
	char *sym;
	ARNode *n;
	printf("===== Symbol Table =====\n");
	for (i = 0; i < SYMHASH; i++) {
		if (SymTab[i] == NULL)
			continue;
		printf("%6d | ", i);
		switch (SymTab[i]->type) {
		case SVAR:   printf("VAR   | "); break;
		case SARRAY: printf("ARRAY | "); break;
		case SCONST: printf("CONST | "); break;
		case STEMP:  printf("TEMP  | "); break;
		default: weprintf("symtabprint: %d: Symbol type error", SymTab[i]->type);
		}
		sym = symstrdup(SymTab[i]);
		printf("%s", sym);
		free(sym);
		if (SymTab[i]->type == SARRAY) {
			for (n = SymTab[i]->u.arr.node; n != NULL; n = n->next)
				printf("[%d]", n->size);
		}
		printf("\n");
	}
}

void
symtabfree(void)
{
	int i;
	ARNode *a, *n;
	for (i = 0; i < SYMHASH; i++) {
		if (SymTab[i] == NULL)
			continue;
		if (SymTab[i]->type == SARRAY) {
			for (a = SymTab[i]->u.arr.node; a != NULL;) {
				n = a;
				a = a->next;
				free(n);
			}
		}
		free(SymTab[i]);
	}
}

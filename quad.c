#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"
#include "functions.h"

#define NEWQUAD() xcalloc(1, sizeof(Quad))

static int labeli = 1;

Quad *
quadgen(int type, int op, Symbol *dest, Symbol *arg1, Symbol *arg2)
{
	Quad *quad = NEWQUAD();
	quad->type = type;
	quad->u.op.op = op;
	quad->u.op.dest = dest;
	quad->u.op.arg1 = arg1;
	quad->u.op.arg2 = arg2;
	return (quad);
}

Quad *
quadjump(int type, Symbol *isz, Quad *jump)
{
	Quad *quad = NEWQUAD();
	quad->type = type;
	quad->u.j.isz = isz;
	quad->u.j.jump = jump;
	return (quad);
}

Quad *
quadlabel(void)
{
	Quad *quad = NEWQUAD();
	quad->type = QLABEL;
	snprintf(quad->u.l.label, LSIZE-1, "L%d", labeli++);
	return (quad);
}

Quad *
quadcat(Quad *qa, Quad *qb)
{
	Quad *ret = qa;
	if (!qa && !qb) return (NULL);
	if (!qa) return (qb);
	if (!qb) return (qa);

	while (qa->next)
		qa = qa->next;
	if (qb->prev)
		weprintf("quadcat: Concat in the middle");

	qa->next = qb;
	qb->prev = qa;
	return (ret);
}

void
quadfree(Quad *start)
{
	Quad *a, *n;
	for (a = start; a != NULL;) {
		n = a;
		a = a->next;
		free(n);
	}
}

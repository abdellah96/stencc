#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"
#include "functions.h"

static char *optable[] = {
	[OPCOPY] = "move",
	[OPNEG]  = "neg",
	[OPEQ]   = "seq",
	[OPNE]   = "sne",
	[OPLT]   = "slt",
	[OPGT]   = "sgt",
	[OPLE]   = "sle",
	[OPGE]   = "sge",
	[OPOR]   = "or",
	[OPXOR]  = "xor",
	[OPAND]  = "and",
	[OPADD]  = "add",
	[OPSUB]  = "sub",
	[OPMUL]  = "mul",
	[OPDIV]  = "div",
	[OPREM]  = "rem",
};

void
mipsgen(Quad *list)
{
	int i;
	char *dest;
	char *arg1;
	char *arg2;

	printf("\t.text\n\n");
	printf("main:\n");

	for (; list != NULL; list = list->next) {
		if (list->type != QLABEL)
			printf("\t");
		switch (list->type) {
		case QCOPY:
		case QUNARY:
			dest = symstrdup(list->u.op.dest);
			arg1 = symstrdup(list->u.op.arg1);
			if (list->u.op.arg1->type != STEMP)
				printf("la $v1, %s\n\t", arg1);
			if (list->u.op.dest->type != STEMP)
				printf("la $v0, %s\n\t", dest);
			switch (list->u.op.op) {
			case OPINC: printf("add $v0, $v1, 1"); break;
			case OPDEC: printf("sub $v0, $v1, 1"); break;
			default:
				if (list->u.op.dest->type != STEMP)
					printf("%s $v0, ", optable[list->u.op.op]);
				else
					printf("%s %s, ", optable[list->u.op.op], dest);
				if (list->u.op.arg1->type != STEMP)
					printf("$v1");
				else
					printf("%s", arg1);
			}
			free(arg1);
			free(dest);
			break;
		case QBINARY:
			dest = symstrdup(list->u.op.dest);
			arg1 = symstrdup(list->u.op.arg1);
			arg2 = symstrdup(list->u.op.arg2);
			if (list->u.op.arg2->type != STEMP)
				printf("la $s0, %s\n\t", arg2);
			if (list->u.op.arg1->type != STEMP)
				printf("la $v1, %s\n\t", arg1);
			if (list->u.op.dest->type != STEMP)
				printf("la $v0, %s\n\t", dest);

			if (list->u.op.dest->type != STEMP)
				printf("%s $v0, ", optable[list->u.op.op]);
			else
				printf("%s %s, ", optable[list->u.op.op], dest);
			if (list->u.op.arg1->type != STEMP)
				printf("$v1, ");
			else
				printf("%s, ", arg1);
			if (list->u.op.arg2->type != STEMP)
				printf("$s0");
			else
				printf("%s", arg2);

			free(arg2);
			free(arg1);
			free(dest);
			break;
		case QLABEL:
			printf("%s:", list->u.l.label);
			break;
		case QGOTO:
			printf("b %s", list->u.j.jump->u.l.label);
			break;
		case QGOTOIF:
			arg1 = symstrdup(list->u.j.isz);
			printf("bnez %s, %s", arg1, list->u.j.jump->u.l.label);
			free(arg1);
			break;
		case QARRAY:
			dest = symstrdup(list->u.op.dest);
			arg1 = symstrdup(list->u.op.arg1);
			arg2 = symstrdup(list->u.op.arg2);
			switch (list->u.op.op) {
			case QLEFT: printf("move %s(%s), %s", arg2, dest, arg1); break;
			default:    printf("move %s, %s(%s)", dest, arg2, arg1);
			}
			free(arg2);
			free(arg1);
			free(dest);
			break;
		default:
			weprintf("mipsgen: %d: Quad type error", list->type);
		}
		printf("\n");
	}

	printf("exit:\n\n");
	printf("\t.data\n\n");

	for (i = 0; i < SYMHASH; i++) {
		if (SymTab[i] == NULL)
			continue;
		if (SymTab[i]->type == SVAR) {
			dest = symstrdup(SymTab[i]);
			printf("%s:\t.word 0\n", dest);
			free(dest);
		}
	}
}

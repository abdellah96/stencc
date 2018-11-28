#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"
#include "functions.h"

#define NEWNODE() xcalloc(1, sizeof(ASTNode))

ASTNode *
astvar(Name id)
{
	ASTNode *node = NEWNODE();
	node->type = ASTVAR;
	node->u.sym = symget(id);
	return (node);
}

ASTNode *
astconst(int cval)
{
	ASTNode *node = NEWNODE();
	node->type = ASTCONST;
	node->u.sym = symconst(cval);
	return (node);
}

ASTNode *
astx(ASTNode *expr, ASTNode *child)
{
	ASTNode *node = NEWNODE();
	node->type = ASTX;
	node->u.x.expr = expr;
	node->u.x.child = child;
	return (node);
}

ASTNode *
astxvar(Name id, ASTNode *child)
{
	ASTNode *node = NEWNODE();
	node->type = ASTXVAR;
	node->u.x.sym = symget(id);
	node->u.x.child = child;
	return (node);
}

ASTNode *
astxconst(ASTNode *child, ASTNode *next)
{
	ASTNode *node = NEWNODE();
	node->type = ASTXCONST;
	node->u.x.child = child;
	node->u.x.next = next;
	return (node);
}

ASTNode *
astcopy(ASTNode *left, ASTNode *right)
{
	ASTNode *node = NEWNODE();
	node->type = ASTCOPY;
	node->u.expr.op = OPCOPY;
	node->u.expr.left = left;
	node->u.expr.right = right;
	return (node);
}

ASTNode *
astpre(int op, ASTNode *left)
{
	ASTNode *node = NEWNODE();
	node->type = ASTPRE;
	node->u.expr.op = op;
	node->u.expr.left = left;
	return (node);
}

ASTNode *
astpost(int op, ASTNode *left)
{
	ASTNode *node = NEWNODE();
	node->type = ASTPOST;
	node->u.expr.op = op;
	node->u.expr.left = left;
	return (node);
}

ASTNode *
astunary(int op, ASTNode *expr)
{
	ASTNode *node = NEWNODE();
	node->type = ASTUNARY;
	node->u.expr.op = op;
	node->u.expr.left = expr;
	return (node);
}

ASTNode *
astbinary(int op, ASTNode *left, ASTNode *right)
{
	ASTNode *node = NEWNODE();
	node->type = ASTBINARY;
	node->u.expr.op = op;
	node->u.expr.left = left;
	node->u.expr.right = right;
	return (node);
}

ASTNode *
astcond(int op, ASTNode *left, ASTNode *right)
{
	ASTNode *node = NEWNODE();
	node->type = ASTCOND;
	node->u.cond.op = op;
	node->u.cond.left = left;
	node->u.cond.right = right;
	return (node);
}

ASTNode *
astif(ASTNode *cond, ASTNode *block)
{
	ASTNode *node = NEWNODE();
	node->type = ASTIF;
	node->u.elif.cond = cond;
	node->u.elif.block = block;
	return (node);
}

ASTNode *
astdo(ASTNode *block, ASTNode *cond)
{
	ASTNode *node = NEWNODE();
	node->type = ASTDO;
	node->u.loop.cond = cond;
	node->u.loop.block = block;
	return (node);
}

ASTNode *
astwhile(ASTNode *cond, ASTNode *block)
{
	ASTNode *node = NEWNODE();
	node->type = ASTWHILE;
	node->u.loop.cond = cond;
	node->u.loop.block = block;
	return (node);
}

ASTNode *
astelif(ASTNode *cond, ASTNode *block, ASTNode *elsebl)
{
	ASTNode *node = NEWNODE();
	node->type = ASTELIF;
	node->u.elif.cond = cond;
	node->u.elif.block = block;
	node->u.elif.elsebl = elsebl;
	return (node);
}

ASTNode *
astfor(ASTNode *once, ASTNode *cond, ASTNode *iter, ASTNode *block)
{
	ASTNode *node = NEWNODE();
	node->type = ASTFOR;
	node->u.loop.once = once;
	node->u.loop.cond = cond;
	node->u.loop.iter = iter;
	node->u.loop.block = block;
	return (node);
}

Symbol *
astgen(ASTNode *ast)
{
	Symbol *l;
	Symbol *r;
	Symbol *sym = NULL;
	Quad *q1;
	Quad *q2;
	Quad *q3;

	switch (ast->type) {
	case ASTVAR:
	case ASTCONST:
		sym = ast->u.sym;
		break;
	case ASTX:
		l = astgen(ast->u.x.expr);
		r = ast->u.x.child ? astgen(ast->u.x.child) : symconst(4);
		sym = symtemp();
		q1 = quadgen(QBINARY, OPMUL, sym, l, r);
		ast->code = quadcat(ast->u.x.child ? ast->u.x.child->code : NULL, q1);
		ast->code = quadcat(ast->u.x.expr->code, ast->code);
		break;
	case ASTXVAR:
		sym = ast->u.x.sym;
		sym->u.arr.off = astgen(ast->u.x.child);
		ast->code = ast->u.x.child->code;
		break;
	case ASTCOPY:
		sym = astgen(ast->u.expr.left);
		r = astgen(ast->u.expr.right);
		if (sym->type == SARRAY) {
			q1 = quadgen(QARRAY, QLEFT, sym, r, sym->u.arr.off);
		} else if (r->type == SARRAY) {
			q1 = quadgen(QARRAY, QRIGHT, sym, r, r->u.arr.off);
		} else {
			q1 = quadgen(QCOPY, OPCOPY, sym, r, NULL);
		}
		ast->code = quadcat(ast->u.expr.right->code, q1);
		ast->code = quadcat(ast->u.expr.left->code, ast->code);
		break;
	case ASTPRE:
	case ASTPOST:
		sym = astgen(ast->u.expr.left);
		q1 = quadgen(QUNARY, ast->u.expr.op, sym, sym, NULL);
		ast->code = quadcat(ast->u.expr.left->code, q1);
		break;
	case ASTUNARY:
		l = astgen(ast->u.expr.left);
		sym = symtemp();
		q1 = quadgen(QUNARY, ast->u.expr.op, sym, l, NULL);
		ast->code = quadcat(ast->u.expr.left->code, q1);
		break;
	case ASTBINARY:
		l = astgen(ast->u.expr.left);
		r = astgen(ast->u.expr.right);
		sym = symtemp();
		q1 = quadgen(QBINARY, ast->u.expr.op, sym, l, r);
		ast->code = quadcat(ast->u.expr.right->code, q1);
		ast->code = quadcat(ast->u.expr.left->code, ast->code);
		break;
	case ASTCOND:
		switch (ast->u.cond.op) {
		case OPLOR:
			q1 = quadlabel();
			ast->u.cond.left->u.cond.truel = ast->u.cond.truel;
			ast->u.cond.left->u.cond.falsel = q1;
			ast->u.cond.right->u.cond.truel = ast->u.cond.truel;
			ast->u.cond.right->u.cond.falsel = ast->u.cond.falsel;
			astgen(ast->u.cond.left);
			astgen(ast->u.cond.right);
			ast->code = quadcat(q1, ast->u.cond.right->code);
			ast->code = quadcat(ast->u.cond.left->code, ast->code);
			break;
		case OPLAND:
			q1 = quadlabel();
			ast->u.cond.left->u.cond.truel = q1;
			ast->u.cond.left->u.cond.falsel = ast->u.cond.falsel;
			ast->u.cond.right->u.cond.truel = ast->u.cond.truel;
			ast->u.cond.right->u.cond.falsel = ast->u.cond.falsel;
			astgen(ast->u.cond.left);
			astgen(ast->u.cond.right);
			ast->code = quadcat(q1, ast->u.cond.right->code);
			ast->code = quadcat(ast->u.cond.left->code, ast->code);
			break;
		case OPNOT:
			ast->u.cond.left->u.cond.truel = ast->u.cond.falsel;
			ast->u.cond.left->u.cond.falsel = ast->u.cond.truel;
			astgen(ast->u.cond.left);
			ast->code = ast->u.cond.left->code;
			break;
		case OPREL:
			l = astgen(ast->u.cond.left);
			q1 = quadjump(QGOTOIF, l, ast->u.cond.truel);
			q2 = quadjump(QGOTO, NULL, ast->u.cond.falsel);
			ast->code = quadcat(q1, q2);
			ast->code = quadcat(ast->u.cond.left->code, ast->code);
			break;
		}
		break;
	case ASTIF:
		q1 = quadlabel();
		q2 = quadlabel();
		ast->u.elif.cond->u.cond.truel = q1;
		ast->u.elif.cond->u.cond.falsel = q2;
		astgen(ast->u.elif.cond);
		astgen(ast->u.elif.block);
		ast->code = quadcat(ast->u.elif.block->code, q2);
		ast->code = quadcat(q1, ast->code);
		ast->code = quadcat(ast->u.elif.cond->code, ast->code);
		break;
	case ASTELIF:
		q1 = quadlabel();
		q2 = quadlabel();
		q3 = quadlabel();
		ast->u.elif.cond->u.cond.truel = q1;
		ast->u.elif.cond->u.cond.falsel = q2;
		astgen(ast->u.elif.cond);
		astgen(ast->u.elif.block);
		astgen(ast->u.elif.elsebl);
		ast->code = quadcat(ast->u.elif.elsebl->code, q3);
		ast->code = quadcat(q2, ast->code);
		ast->code = quadcat(quadjump(QGOTO, NULL, q3), ast->code);
		ast->code = quadcat(ast->u.elif.block->code, ast->code);
		ast->code = quadcat(q1, ast->code);
		ast->code = quadcat(ast->u.elif.cond->code, ast->code);
		break;
	case ASTWHILE:
		q1 = quadlabel();
		q2 = quadlabel();
		q3 = quadlabel();
		ast->u.loop.cond->u.cond.truel = q2;
		ast->u.loop.cond->u.cond.falsel = q3;
		astgen(ast->u.loop.cond);
		astgen(ast->u.loop.block);
		ast->code = quadcat(quadjump(QGOTO, NULL, q1), q3);
		ast->code = quadcat(ast->u.loop.block->code, ast->code);
		ast->code = quadcat(q2, ast->code);
		ast->code = quadcat(ast->u.loop.cond->code, ast->code);
		ast->code = quadcat(q1, ast->code);
		break;
	case ASTFOR:
		q1 = quadlabel();
		q2 = quadlabel();
		q3 = quadlabel();
		ast->u.loop.cond->u.cond.truel = q2;
		ast->u.loop.cond->u.cond.falsel = q3;
		astgen(ast->u.loop.once);
		astgen(ast->u.loop.cond);
		astgen(ast->u.loop.iter);
		astgen(ast->u.loop.block);
		ast->code = quadcat(quadjump(QGOTO, NULL, q1), q3);
		ast->code = quadcat(ast->u.loop.iter->code, ast->code);
		ast->code = quadcat(ast->u.loop.block->code, ast->code);
		ast->code = quadcat(q2, ast->code);
		ast->code = quadcat(ast->u.loop.cond->code, ast->code);
		ast->code = quadcat(q1, ast->code);
		ast->code = quadcat(ast->u.loop.once->code, ast->code);
		break;
	case ASTDO:
		q1 = quadlabel();
		q2 = quadlabel();
		ast->u.loop.cond->u.cond.truel = q1;
		ast->u.loop.cond->u.cond.falsel = q2;
		astgen(ast->u.loop.cond);
		astgen(ast->u.loop.block);
		ast->code = quadcat(ast->u.loop.cond->code, q2);
		ast->code = quadcat(ast->u.loop.block->code, ast->code);
		ast->code = quadcat(q1, ast->code);
		break;
	default:
		weprintf("astgen: %d: AST type error", ast->type);
	}

	if (ast->next != NULL) {
		astgen(ast->next);
		ast->code = quadcat(ast->code, ast->next->code);
	}
	return (sym);
}

static void
printop(int op)
{
	switch (op) {
	case OPCOPY:    printf(":="); break;
	case OPINC:     printf("++"); break;
	case OPDEC:     printf("--"); break;
	case OPPLUS:    printf("+");  break;
	case OPNEG:     printf("-");  break;
	case OPLOR:     printf("||"); break;
	case OPLAND:    printf("&&"); break;
	case OPNOT:     printf("!");  break;
	case OPREL:     printf("?");  break;
	case OPEQ:      printf("=="); break;
	case OPNE:      printf("!="); break;
	case OPLT:      printf("<");  break;
	case OPGT:      printf(">");  break;
	case OPLE:      printf("<="); break;
	case OPGE:      printf(">="); break;
	case OPOR:      printf("|");  break;
	case OPXOR:     printf("^");  break;
	case OPAND:     printf("&");  break;
	case OPADD:     printf("+");  break;
	case OPSUB:     printf("-");  break;
	case OPMUL:     printf("*");  break;
	case OPDIV:     printf("/");  break;
	case OPREM:     printf("%%"); break;
	case OPSTEN:    printf("$");  break;
	default:        printf("#");  break;
	}
}

void
astprint(ASTNode *ast, int level)
{
#define INDENT() for (i = 0; i < level; i++) printf("    ")
	int i;
	char *sym;

	INDENT();
	switch (ast->type) {
	case ASTVAR:
	case ASTCONST:
		sym = symstrdup(ast->u.sym);
		printf("%s\n", sym); free(sym);
		break;
	case ASTCOND:
		if (ast->u.cond.op == OPREL) {
			printf("\n");
			astprint(ast->u.cond.left, level);
		} else {
			printop(ast->u.cond.op);
			printf("\n");
			astprint(ast->u.cond.left, level + 1);
		}
		if (ast->u.cond.right != NULL)
			astprint(ast->u.cond.right, level + 1);
		break;
	case ASTPRE:
	case ASTPOST:
	case ASTCOPY:
	case ASTUNARY:
	case ASTBINARY:
		printop(ast->u.expr.op);
		printf("\n");
		astprint(ast->u.expr.left, level + 1);
		if (ast->u.expr.right != NULL)
			astprint(ast->u.expr.right, level + 1);
		break;
	case ASTIF:
	case ASTELIF:
		printf("if\n");
		astprint(ast->u.elif.cond, level + 1);
		INDENT(); printf("then\n");
		astprint(ast->u.elif.block, level + 1);
		if (ast->u.elif.elsebl != NULL) {
			INDENT(); printf("else\n");
			astprint(ast->u.elif.elsebl, level + 1);
		}
		break;
	case ASTWHILE:
		printf("while\n");
		astprint(ast->u.loop.cond, level + 1);
		INDENT(); printf("do\n");
		astprint(ast->u.loop.block, level + 1);
		break;
	case ASTFOR:
		printf("for\n");
		astprint(ast->u.loop.once, level + 1);
		INDENT(); printf("cond\n");
		astprint(ast->u.loop.cond, level + 1);
		INDENT(); printf("iter\n");
		astprint(ast->u.loop.iter, level + 1);
		INDENT(); printf("do\n");
		astprint(ast->u.loop.block, level + 1);
		break;
	case ASTDO:
		printf("do\n");
		astprint(ast->u.loop.block, level + 1);
		INDENT(); printf("while\n");
		astprint(ast->u.loop.cond, level + 1);
		break;
	default:
		weprintf("astprint: %d: AST type error", ast->type);
	}
	if (ast->next != NULL)
		astprint(ast->next, level);
}

void
astfree(ASTNode *ast)
{
	switch (ast->type) {
	case ASTVAR:
	case ASTCONST:
		break;
	case ASTX:
	case ASTXVAR:
	case ASTXCONST:
		if (ast->u.x.child != NULL)
			astfree(ast->u.x.child);
		if (ast->u.x.next != NULL)
			astfree(ast->u.x.next);
		if (ast->u.x.expr != NULL)
			astfree(ast->u.x.expr);
		break;
	case ASTPRE:
	case ASTPOST:
	case ASTCOND:
	case ASTCOPY:
	case ASTUNARY:
	case ASTBINARY:
		astfree(ast->u.expr.left);
		if (ast->u.expr.right != NULL)
			astfree(ast->u.expr.right);
		break;
	case ASTIF:
	case ASTELIF:
		astfree(ast->u.elif.cond);
		astfree(ast->u.elif.block);
		if (ast->u.elif.elsebl != NULL)
			astfree(ast->u.elif.elsebl);
		break;
	case ASTDO:
	case ASTWHILE:
		astfree(ast->u.loop.cond);
		astfree(ast->u.loop.block);
		break;
	case ASTFOR:
		astfree(ast->u.loop.once);
		astfree(ast->u.loop.cond);
		astfree(ast->u.loop.iter);
		astfree(ast->u.loop.block);
		break;
	default:
		weprintf("astfree: %d: AST type error", ast->type);
	}
	if (ast->next != NULL)
		astfree(ast->next);

	free(ast);
}

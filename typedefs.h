#define LSIZE 8
typedef char Label[LSIZE];
#define ISIZE 32
typedef char Name[ISIZE];

typedef struct Quad Quad;
typedef struct Symbol Symbol;
typedef struct ARNode ARNode;
typedef struct ASTNode ASTNode;

/* AST variants */
enum {
	/* error */
	ASTE,
	/* sym */
	ASTX,
	ASTVAR,
	ASTCONST,
	ASTXVAR,
	ASTXCONST,
	/* control flow */
	ASTIF,
	ASTDO,
	ASTWHILE,
	ASTELIF,
	ASTFOR,
	/* operations */
	ASTCOPY,
	ASTBINARY,
	ASTUNARY,
	ASTCOND,
	ASTPOST,
	ASTPRE,
};

/* operation types */
enum {
	OPE,
	OPCOPY,
	/* --/++ */
	OPINC,
	OPDEC,
	/* unary */
	OPPLUS,
	OPNEG,
	/* bool */
	OPLOR,
	OPLAND,
	OPNOT,
	OPREL,
	/* comp */
	OPEQ,
	OPNE,
	OPLT,
	OPGT,
	OPLE,
	OPGE,
	/* binary */
	OPOR,
	OPXOR,
	OPAND,
	OPADD,
	OPSUB,
	OPMUL,
	OPDIV,
	OPREM,
	/* stencil */
	OPSTEN,
};

/* symbol types */
enum {
	SE,
	STEMP,
	SVAR,
	SCONST,
	SARRAY,
};

struct ARNode {
	int size;
	ARNode *next;
};

struct Symbol {
	int type;
	union {
		int tnum;
		Name id;
		int cval;
		struct {
			Name id;
			Symbol *off;
			ARNode *node;
		} arr;
	} u;
};

/* quad types */
enum {
	QE,
	QCOPY,
	QUNARY,
	QBINARY,
	QLABEL,
	QGOTO,
	QGOTOIF,
	QARRAY,
	QLEFT,
	QRIGHT,
};

struct Quad {
	int type;
	union {
		struct {
			int op;
			Symbol *dest;
			Symbol *arg1;
			Symbol *arg2;
		} op;
		struct {
			Symbol *isz;
			Quad *jump;
		} j;
		struct {
			Label label;
		} l;
	} u;
	Quad *next;
	Quad *prev;
};

/* Abstract Syntax Tree */
struct ASTNode {
	int type;
	Quad *code;
	ASTNode *next;
	union {
		Symbol *sym;
		struct {
			Symbol *sym;
			ASTNode *expr;
			ASTNode *next;
			ASTNode *child;
		} x;
		struct {
			int op;
			ASTNode *left;
			ASTNode *right;
		} expr;
		struct {
			int op;
			Quad *truel;
			Quad *falsel;
			ASTNode *left;
			ASTNode *right;
		} cond;
		struct {
			ASTNode *cond;
			ASTNode *block;
			ASTNode *elsebl;
		} elif;
		struct {
			ASTNode *once;
			ASTNode *cond;
			ASTNode *iter;
			ASTNode *block;
		} loop;
	} u;
};

/* symbols hash table */
#define SYMHASH 8192
extern Symbol *SymTab[SYMHASH];

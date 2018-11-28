/* grammar */
int yyparse(void);

/* lex */
int  yylex(void);
void yyerror(char *s);

/* sym */
Symbol *symtemp(void);
Symbol *symconst(int cval);
Symbol *symdeclar(Name id, ARNode *array);
Symbol *symget(Name id);
ARNode *symanode(int size, ARNode *next);
char   *symstrdup(Symbol *sym);
void    symtabzero(void);
void    symtabprint(void);
void    symtabfree(void);

/* ast */
ASTNode *astvar(Name id);
ASTNode *astconst(int cval);
ASTNode *astx(ASTNode *n, ASTNode *child);
ASTNode *astxvar(Name id, ASTNode *child);
ASTNode *astxconst(ASTNode *child, ASTNode *next);
ASTNode *astcopy(ASTNode *dest, ASTNode *expr);
ASTNode *astpre(int op, ASTNode *dest);
ASTNode *astpost(int op, ASTNode *dest);
ASTNode *astunary(int op, ASTNode *expr);
ASTNode *astbinary(int op, ASTNode *left, ASTNode *right);
ASTNode *astcond(int op, ASTNode *left, ASTNode *right);
ASTNode *astif(ASTNode *cond, ASTNode *block);
ASTNode *astdo(ASTNode *block, ASTNode *cond);
ASTNode *astwhile(ASTNode *cond, ASTNode *block);
ASTNode *astelif(ASTNode *cond, ASTNode *block, ASTNode *elsebl);
ASTNode *astfor(ASTNode *once, ASTNode *cond, ASTNode *iter, ASTNode *block);
Symbol  *astgen(ASTNode *ast);
void     astprint(ASTNode *ast, int level);
void     astfree(ASTNode *ast);

/* quad */
Quad *quadlabel(void);
Quad *quadgen(int type, int op, Symbol *l, Symbol *r1, Symbol *r2);
Quad *quadjump(int type, Symbol *b, Quad *dest);
Quad *quadcat(Quad *qa, Quad *qb);
void  quadfree(Quad *start);

/* asm */
void mipsgen(Quad *list);

/* main */
void  weprintf(const char *fmt, ...);
void *xcalloc(size_t nmemb, size_t size);

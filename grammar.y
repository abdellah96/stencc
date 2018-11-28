%{
#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"
#include "functions.h"
%}

%union {
	int cval;
	Name id;
	ASTNode *node;
	ARNode *anode;
}

%token <cval> CONST
%token <id> ID

%type <anode> dim zdim

%type <node> function
%type <node> init decline declist dec varinit varonly
%type <node> statlist stat bool comp expr varname elemlist edim

%token LINT LSTENCIL
%token LIF LELSE LWHILE LFOR LDO
%token LOR LAND LEQ LNE LLE LGE LINC LDEC

%%

function: init statlist           { $1->next = $2;
                                    astgen($$);
                                    printf("===== Abstract Syntax Tree =====\n");
                                    astprint($$, 0);
                                    symtabprint();
                                    printf("#===== MIPS Code =====\n");
                                    mipsgen($$->code);
                                    quadfree($$->code);
                                    symtabfree();
                                    /*astfree($$);*/ }

init: decline init                { $1->next = $2; }
    | decline                     { $$ = $1; }

decline: type declist ';'         { $$ = $2; }
type: LINT
    | LSTENCIL

declist: dec ',' declist          { $1->next = $3; }
       | dec                      { $$ = $1; }

dec: varinit '=' expr             { $$ = astcopy($1, $3); }
   | varonly                      { $$ = $1; }

varinit: ID zdim                  { symdeclar($1, $2);   $$ = astvar($1); }
       | ID                       { symdeclar($1, NULL); $$ = astvar($1); }
varonly: ID dim                   { symdeclar($1, $2);   $$ = astvar($1); }
       | ID                       { symdeclar($1, NULL); $$ = astvar($1); }

dim: '[' CONST ']' dim            { $$ = symanode($2, $4); }
   | '[' CONST ']'                { $$ = symanode($2, NULL); }
zdim: '[' ']' zdim                { $$ = symanode(-1, $3); }
    | '[' ']'                     { $$ = symanode(-1, NULL); }

/* end init */

statlist: stat statlist           { $1->next = $2; }
        | stat                    { $$ = $1; }

stat: expr ';'                                                    { $$ = $1; }
    | LIF    '(' bool ')' '{' statlist '}'                        { $$ = astif($3, $6); }
    | LWHILE '(' bool ')' '{' statlist '}'                        { $$ = astwhile($3, $6); }
    | LIF    '(' bool ')' '{' statlist '}' LELSE '{' statlist '}' { $$ = astelif($3, $6, $10); }
    | LFOR   '(' expr ';' bool ';' expr ')' '{' statlist '}'      { $$ = astfor($3, $5, $7, $10); }
    | LDO    '{' statlist '}' LWHILE '(' bool ')' ';'             { $$ = astdo($3, $7); }

bool: '(' bool ')'                { $$ = $2; }
    | '!' bool                    { $$ = astcond(OPNOT,  $2, NULL); }
    | bool LAND bool              { $$ = astcond(OPLAND, $1, $3); }
    | bool LOR  bool              { $$ = astcond(OPLOR,  $1, $3); }
    | comp                        { $$ = astcond(OPREL,  $1, NULL); }

comp: '(' comp ')'                { $$ = $2; }
    | expr LEQ expr               { $$ = astbinary(OPEQ, $1, $3); }
    | expr LNE expr               { $$ = astbinary(OPNE, $1, $3); }
    | expr '<' expr               { $$ = astbinary(OPLT, $1, $3); }
    | expr '>' expr               { $$ = astbinary(OPGT, $1, $3); }
    | expr LLE expr               { $$ = astbinary(OPLE, $1, $3); }
    | expr LGE expr               { $$ = astbinary(OPGE, $1, $3); }
    | expr                        { $$ = $1; }

expr: '(' expr ')'                { $$ = $2; }
    | '{' elemlist '}'            { $$ = $2; }
    | varname                     { $$ = $1; }
    | varname '=' expr            { $$ = astcopy($1, $3); }
    | varname LINC                { $$ = astpost(OPINC, $1); }
    | varname LDEC                { $$ = astpost(OPDEC, $1); }
    | LINC varname                { $$ = astpre(OPINC,  $2); }
    | LDEC varname                { $$ = astpre(OPDEC,  $2); }
    | expr '|' expr               { $$ = astbinary(OPOR,   $1, $3); }
    | expr '^' expr               { $$ = astbinary(OPXOR,  $1, $3); }
    | expr '&' expr               { $$ = astbinary(OPAND,  $1, $3); }
    | expr '+' expr               { $$ = astbinary(OPADD,  $1, $3); }
    | expr '-' expr               { $$ = astbinary(OPSUB,  $1, $3); }
    | expr '*' expr               { $$ = astbinary(OPMUL,  $1, $3); }
    | expr '/' expr               { $$ = astbinary(OPDIV,  $1, $3); }
    | expr '%' expr               { $$ = astbinary(OPREM,  $1, $3); }
    | expr '$' expr               { $$ = astbinary(OPSTEN, $1, $3); }
    | '+' expr                    { $$ = astunary(OPPLUS, $2); }
    | '-' expr                    { $$ = astunary(OPNEG,  $2); }
    | CONST                       { $$ = astconst($1); }

varname: ID edim                  { $$ = astxvar($1, $2); }
       | ID                       { $$ = astvar($1); }

elemlist: expr ',' elemlist       { $$ = astxconst($1, $3); }
        | expr                    { $$ = astxconst($1, NULL); }

edim: '[' expr ']' edim           { $$ = astx($2, $4); }
    | '[' expr ']'                { $$ = astx($2, NULL); }

%{
#include "ast/ast.h"
#include <memory>
ASTPtr root;
int yylex();
void yyerror(const char*) {}
%}

%union {
    int ival;
    char* sval;
    ASTPtr* node;
}

%token <ival> NUMBER
%token <sval> IDENT
%token WHILE IF PRINT

%type <node> program stmt stmt_list expr term factor

%%
program:
    stmt_list { root = std::move(*$1); }
;

stmt_list:
    stmt_list stmt { $$ = new ASTPtr(std::make_unique<SeqAST>(std::move(*$1), std::move(*$2))); }
  | stmt           { $$ = $1; }
;

stmt:
    IDENT '=' expr ';'
        { $$ = new ASTPtr(std::make_unique<AssignAST>($1, std::move(*$3))); }
  | PRINT expr ';'
        { $$ = new ASTPtr(std::make_unique<PrintAST>(std::move(*$2))); }
  | IF '(' expr ')' '{' stmt_list '}'
        { $$ = new ASTPtr(std::make_unique<IfAST>(std::move(*$3), std::move(*$6))); }
  | WHILE '(' expr ')' '{' stmt_list '}'
        { $$ = new ASTPtr(std::make_unique<WhileAST>(std::move(*$3), std::move(*$6))); }
;

expr:
    expr '+' term { $$ = new ASTPtr(std::make_unique<BinOpAST>('+', std::move(*$1), std::move(*$3))); }
  | expr '-' term { $$ = new ASTPtr(std::make_unique<BinOpAST>('-', std::move(*$1), std::move(*$3))); }
  | term          { $$ = $1; }
;

term:
    term '*' factor { $$ = new ASTPtr(std::make_unique<BinOpAST>('*', std::move(*$1), std::move(*$3))); }
  | term '/' factor { $$ = new ASTPtr(std::make_unique<BinOpAST>('/', std::move(*$1), std::move(*$3))); }
  | factor          { $$ = $1; }
;

factor:
    NUMBER { $$ = new ASTPtr(std::make_unique<NumberAST>($1)); }
  | IDENT  { $$ = new ASTPtr(std::make_unique<VarAST>($1)); }
  | '(' expr ')' { $$ = $2; }
  | '?' { $$ = new ASTPtr(std::make_unique<InputAST>()); }
;
%%

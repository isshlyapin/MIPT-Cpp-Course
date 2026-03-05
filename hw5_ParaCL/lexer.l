%{
#include "parser.hpp"
#include <cstdlib>
%}

%%
"while"        { return WHILE; }
"if"           { return IF; }
"print"        { return PRINT; }

[0-9]+         { yylval.ival = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return IDENT; }

[ \t\r\n]+     { }
.              { return yytext[0]; }
%%

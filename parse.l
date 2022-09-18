%{
/* See LICENSE file for copyright and license details */
#include <err.h>
#include "y.tab.h"
%}

%%

target {
	return TARGET;
}

source {
	return SOURCE;
}

extension {
	return EXTENSION;
}

suffix {
	return SUFFIX;
}

require {
	return REQUIRE;
}

filter {
	return FILTER;
}

none {
	return NONE;
}

[{}] return yytext[0];

\"[^"\n]*["\n] |
'[^'\n]*['\n] |
[^ \t\n]+ {
	yylval.str = yytext;
	return STRING;
}

[ \t\n]+

%%
void yyerror(char *e) {
	err(1, "syntax error: %s", e);
}

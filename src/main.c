#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#include "ast.h"
#define LEXER_INCLUDE_IMPLEMENTATION
#include "lexer.h"

static char input_stream[256];

int main()
{   
    Lexer lexer;

    while(true)
    {
        fputs("> ", stdout);
        fgets(input_stream, 256, stdin);

        lexer_init(&lexer, input_stream);

        Expr* ast = parse_expr(&lexer);

        Value val = eval(ast);
        print_value(val);

        clean_expr(ast);
    }
}

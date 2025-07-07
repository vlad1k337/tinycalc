#ifndef AST_H__
#define AST_H__

#include "lexer.h"

typedef struct Expr Expr;

typedef enum {
    EXPR_CONST,
    EXPR_UNOP,
    EXPR_BINOP,
} ExprType;

typedef enum {
    VALUE_INT,
    VALUE_DOUBLE,
    VALUE_ERROR,
} ValueType;

typedef struct {
    ValueType val_type;

    union {
        double real;
        long   integer;
    };
} Value;

static const ValueType binop_table[2][2] = {
    { VALUE_INT,    VALUE_DOUBLE },
    { VALUE_DOUBLE, VALUE_DOUBLE },
};

static const ValueType bit_table[2][2] = {
    { VALUE_INT,   VALUE_ERROR },
    { VALUE_ERROR, VALUE_ERROR },
};

struct Expr {
    ExprType expr_type; 

    union {
        struct Const { Value value; } Const;
        struct UnOp  { Expr* sub_expr; TokenType type; } UnOp;
        struct BinOp { Expr* left; Expr* right; TokenType type; } BinOp;
    } data;
};

Expr* make_const(Value value);

Expr* make_unop(Expr* sub_expr, TokenType type);

Expr* make_binop(Expr* left, Expr* right, TokenType type);


Expr* parse_expr(Lexer* lexer);

Expr* parse_bitor(Lexer* lexer);

Expr* parse_bitand(Lexer* lexer);

Expr* parse_shifts(Lexer* lexer);

Expr* parse_add_sub(Lexer* lexer);

Expr* parse_mult_div(Lexer* lexer);

Expr* parse_power(Lexer* lexer);

Expr* parse_unary(Lexer* lexer);

Expr* parse_primary(Lexer* lexer);

Expr* parse_number(Lexer* lexer);


Value eval(Expr* expression);

Value make_value(ValueType type, double value);

void print_value(Value value);

void clean_expr(Expr* expression);

#endif // AST_H__

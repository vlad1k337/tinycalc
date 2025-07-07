#include "ast.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "lexer.h"

Expr* make_const(Value value)
{
    Expr* new_node = malloc(sizeof *new_node);
    assert(NULL != new_node);

    new_node->expr_type = EXPR_CONST;

    new_node->data.Const.value = value;

    return new_node;
}

Expr* make_unop(Expr* sub_expr, TokenType type)
{
    Expr* new_node = malloc(sizeof *new_node);
    assert(NULL != new_node);

    new_node->expr_type = EXPR_UNOP;

    new_node->data.UnOp.sub_expr = sub_expr;
    new_node->data.UnOp.type     = type;

    return new_node;
}

Expr* make_binop(Expr* left, Expr* right, TokenType type)
{
    Expr* new_node = malloc(sizeof *new_node);
    assert(NULL != new_node);

    new_node->expr_type = EXPR_BINOP;

    new_node->data.BinOp.left= left;
    new_node->data.BinOp.right = right;
    new_node->data.BinOp.type = type;

    return new_node;
}

Expr* parse_expr(Lexer* lexer)
{
    return parse_bitor(lexer);
}

Expr* parse_bitor(Lexer* lexer)
{
    assert(NULL != lexer);

    Expr* result = parse_bitand(lexer);

    while(lexer->token.type == TOKEN_BITOR)
    {
        TokenType type = lexer->token.type;
        lexer_advance(lexer);

        Expr* right_term = parse_bitand(lexer);
        result = make_binop(result, right_term, type);
    }

    return result;
}

Expr* parse_bitand(Lexer* lexer)
{
    assert(NULL != lexer);

    Expr* result = parse_shifts(lexer);

    while(lexer->token.type == TOKEN_BITAND)
    {
        TokenType type = lexer->token.type;
        lexer_advance(lexer);

        Expr* right_term = parse_shifts(lexer);
        result = make_binop(result, right_term, type);
    }

    return result;
}

Expr* parse_shifts(Lexer* lexer)
{
    assert(NULL != lexer);

    Expr* result = parse_add_sub(lexer);

    while(lexer->token.type == TOKEN_SHR || lexer->token.type == TOKEN_SHL)
    {
        TokenType type = lexer->token.type;
        lexer_advance(lexer);

        Expr* right_term = parse_add_sub(lexer);
        result = make_binop(result, right_term, type);
    }

    return result;
}

Expr* parse_add_sub(Lexer* lexer)
{
    assert(NULL != lexer);

    Expr* result = parse_mult_div(lexer);

    while(lexer->token.type == TOKEN_PLUS || lexer->token.type == TOKEN_MINUS)
    {
        TokenType type = lexer->token.type;
        lexer_advance(lexer);

        Expr* right_term = parse_mult_div(lexer);
        result = make_binop(result, right_term, type);
    }

    return result;
}

Expr* parse_mult_div(Lexer* lexer)
{
    assert(NULL != lexer);

    Expr* result = parse_power(lexer);

    while(lexer->token.type == TOKEN_MULT || lexer->token.type == TOKEN_DIV)
    {
        TokenType type = lexer->token.type;
        lexer_advance(lexer);

        Expr* right_term = parse_power(lexer);
        result = make_binop(result, right_term, type);
    }

    return result;
}

Expr* parse_power(Lexer* lexer)
{
    assert(NULL != lexer); 

    Expr* result = parse_unary(lexer);

    while(lexer->token.type == TOKEN_POWER)
    {
        TokenType type = lexer->token.type;
        lexer_advance(lexer);

        Expr* power = parse_unary(lexer);
        result = make_binop(result, power, type);
    }

    return result;
}

Expr* parse_unary(Lexer* lexer)
{
    assert(NULL != lexer);

    while(lexer->token.type == TOKEN_MINUS || lexer->token.type == TOKEN_SIN || lexer->token.type == TOKEN_ABS || lexer->token.type == TOKEN_BITNOT)
    {
        TokenType type = lexer->token.type;
        lexer_advance(lexer);

        Expr* sub_expr = parse_unary(lexer);

        return make_unop(sub_expr, type);
    }

    return parse_primary(lexer);
}

Expr* parse_primary(Lexer* lexer)
{
    assert(NULL != lexer);

    if(lexer->token.type == TOKEN_LEFT_PAR)
    {
        lexer_advance(lexer);
        Expr* result = parse_expr(lexer);
        lexer_advance(lexer);

        return result;
    } else {
        return parse_number(lexer);
    }
}

Expr* parse_number(Lexer* lexer)
{
    assert(NULL != lexer);

    bool is_real = false;

    Value value;
    double number = 0;

    for(size_t i = 0; i < lexer->token.length; ++i)
    {
        if(lexer->token.data[i] == '.')
        {
            is_real = true;

            double rest = strtod(lexer->token.data + i, NULL);
            number += rest;

            break;
        }

        number = number * 10 + to_digit(lexer->token.data[i]);
    }

    lexer_advance(lexer);

    if(is_real)
    {
        value.val_type = VALUE_DOUBLE;
        value.real = number; 
    } else {
        value.val_type = VALUE_INT;
        value.integer = (long)number;
    }

    return make_const(value);
}

Value eval(Expr* expression)
{
    assert(NULL != expression); 

    if(expression->expr_type == EXPR_CONST)
    {
        return expression->data.Const.value;
    }

    if(expression->expr_type == EXPR_UNOP)
    {
        Value val = eval(expression->data.UnOp.sub_expr);
        double num_value = (val.val_type == VALUE_DOUBLE) ? val.real : val.integer;

        switch(expression->data.UnOp.type)
        {
            case TOKEN_MINUS:
                return make_value(val.val_type, (-1) * num_value);

            case TOKEN_SIN:
                return make_value(VALUE_DOUBLE, sin(num_value));

            case TOKEN_ABS:
                if(val.val_type == VALUE_INT)
                {
                    return make_value(val.val_type, labs((long)num_value));
                } else {
                    return make_value(val.val_type, fabs(num_value));
                }

            case TOKEN_BITNOT:
                if(val.val_type != VALUE_INT)
                {
                    fprintf(stderr, "Type error: can't perform bit operations on non-integer values.\n");
                }
                return make_value(VALUE_INT, ~(long)num_value);

            default:
                assert(0 && "eval(): undefined unary operator");
        }
    }

    if(expression->expr_type == EXPR_BINOP)
    {
        Value left  = eval(expression->data.BinOp.left);
        Value right = eval(expression->data.BinOp.right);

        ValueType deduced;

        double left_val  = (left.val_type  == VALUE_DOUBLE) ? left.real  : left.integer;
        double right_val = (right.val_type == VALUE_DOUBLE) ? right.real : right.integer;
        
        switch(expression->data.BinOp.type)
        {
            case TOKEN_PLUS:
                deduced = binop_table[left.val_type][right.val_type];
                return make_value(deduced, left_val + right_val);

            case TOKEN_MINUS:
                deduced = binop_table[left.val_type][right.val_type];
                return make_value(deduced, left_val - right_val);

            case TOKEN_MULT:
                deduced = binop_table[left.val_type][right.val_type];
                return make_value(deduced, left_val * right_val);

            case TOKEN_DIV:
                deduced = binop_table[left.val_type][right.val_type];
                return make_value(deduced, left_val / right_val);

            case TOKEN_POWER:
                deduced = binop_table[left.val_type][right.val_type];
                return make_value(deduced, pow(left_val, right_val));

            case TOKEN_BITAND:
                deduced = bit_table[left.val_type][right.val_type];
                if(deduced == VALUE_ERROR)
                {
                    fprintf(stderr, "Type error: can't perform bit operations on non-integer values.\n");
                }
                return make_value(deduced, (long)left_val & (long)right_val);

            case TOKEN_BITOR:
                deduced = bit_table[left.val_type][right.val_type];
                if(deduced == VALUE_ERROR)
                {
                    fprintf(stderr, "Type error: can't perform bit operations on non-integer values.\n");
                }
                return make_value(deduced, (long)left_val | (long)right_val);

            case TOKEN_SHR:
                deduced = bit_table[left.val_type][right.val_type];
                if(deduced == VALUE_ERROR)
                {
                    fprintf(stderr, "Type error: can't perform bit operations on non-integer values.\n");
                }
                return make_value(deduced, (long)left_val >> (long)right_val);

            case TOKEN_SHL:
                deduced = bit_table[left.val_type][right.val_type];
                if(deduced == VALUE_ERROR)
                {
                    fprintf(stderr, "Type error: can't perform bit operations on non-integer values.\n");
                }
                return make_value(deduced, (long)left_val << (long)right_val);

            default:
                assert(0 && "eval(): undefined binary operator");
        }
    }

    return (Value){.val_type = VALUE_DOUBLE, .real = 0.0 };
}

void print_value(Value value)
{
    switch(value.val_type)
    {
        case VALUE_DOUBLE:
            printf("%f\n", value.real);
            return;

        case VALUE_INT:
            printf("%ld\n", value.integer);
            return;

        default:
            return;
    }
}

Value make_value(ValueType type, double value)
{
    Value new_val;  
    new_val.val_type = type;

    if(type == VALUE_INT)
    {
        // not a fan of this cast, but it works.
        new_val.integer = (long)value;
    } else {
        new_val.real = value;
    }

    return new_val;
}

void clean_expr(Expr* expression)
{
    if(NULL == expression)
    {
        return;
    }

    switch(expression->expr_type)
    {
        case EXPR_CONST:
            break;

        case EXPR_UNOP:
            clean_expr(expression->data.UnOp.sub_expr);
            break;

        case EXPR_BINOP:
            clean_expr(expression->data.BinOp.left);
            clean_expr(expression->data.BinOp.right);
            break;

        default:
            assert(0 && "clean_expr(): undefined AST node type");
    }

    free(expression);
    expression = NULL;
}

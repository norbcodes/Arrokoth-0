/*
Arrokoth-0 - A simple, basic programming language.
Copyright (C) 2026  NorbCodes

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <arrokoth-0/lexer.h>
#include <arrokoth-0/parser.h>
#include <arrokoth-0/ast.h>

static node_expr_t* ParseExpr(stream_t stream);
static node_block_t* ParseBlock(stream_t stream);

static enum type_expr_e GetExprType(char* op)
{
    if (!strcmp(op, "+")) return T_EXPR_ADD;
    if (!strcmp(op, "-")) return T_EXPR_MINUS;
    if (!strcmp(op, "*")) return T_EXPR_MULT;
    if (!strcmp(op, "/")) return T_EXPR_DIV;
    if (!strcmp(op, "^")) return T_EXPR_EXP;

    if (!strcmp(op, "==")) return T_EXPR_EQUAL;
    if (!strcmp(op, "!=")) return T_EXPR_UNEQUAL;
    if (!strcmp(op, ">"))  return T_EXPR_GREATER;
    if (!strcmp(op, "<"))  return T_EXPR_LESSER;
    if (!strcmp(op, ">=")) return T_EXPR_GE;
    if (!strcmp(op, "<=")) return T_EXPR_LE;
    
    return T_EXPR_ATOM;
}

static uint32_t GetPrecedence(enum type_expr_e op)
{
    switch (op)
    {
        default:
        case T_EXPR_ADD:
        case T_EXPR_MINUS:
            return 0;
        case T_EXPR_MULT:
        case T_EXPR_DIV:
            return 1;
        case T_EXPR_EXP: 
            return 2;
        case T_EXPR_EQUAL:   
        case T_EXPR_UNEQUAL:
        case T_EXPR_GREATER:
        case T_EXPR_LESSER:
        case T_EXPR_GE:
        case T_EXPR_LE:
            return 3;

        case T_EXPR_ATOM:
            return 4;
    }
}


double ParseNumber(stream_t stream)
{
    token_t tok = LexPop(stream);
    LexAssertType(tok, TT_NUMBER);

    return strtod(tok->content, NULL);
}

char* ParseIden(stream_t stream)
{
    token_t tok = LexPop(stream);
    LexAssertType(tok, TT_IDENTIFIER);

    return tok->content;
}



node_atom_t* ParseAtom(stream_t stream)
{
    node_atom_t* node = malloc(sizeof(node_atom_t));

    token_type_t type = LexPeek(stream)->type;
    switch (type)
    {
        case TT_IDENTIFIER:
            node->type = T_ATOM_IDEN;
            node->iden = ParseIden(stream);
            break;
        case TT_NUMBER:
            node->type = T_ATOM_NUMBER;
            node->number = ParseNumber(stream);
            break;
        case TT_LEFT_PAREN:
            LexExpect(stream, "(");
            node->type = T_ATOM_SUBEXPR;
            node->expr = ParseExpr(stream);
            LexExpect(stream, ")");
            break;
        default:
            fprintf(
                stderr, 
                "Unrecognized atom token type: %s\n", 
                TokenTypeToStr(type)
            );
            exit(1);
    }

    return node;
}

static node_expr_t* ParseExprMain(stream_t stream, uint32_t level)
{
    if (level == 4) 
    {
        node_expr_t* node = malloc(sizeof(node_expr_t));
        node->type = T_EXPR_ATOM;
        node->leaf = ParseAtom(stream);
        return node;
    }

    node_expr_t* left = ParseExprMain(stream, level+1);

    enum type_expr_e op = GetExprType(LexPeek(stream)->content);
    if (GetPrecedence(op) != level)
        return left;
    LexPop(stream);

    node_expr_t* right = ParseExprMain(stream, level);

    node_expr_t* node = malloc(sizeof(node_expr_t));
    node->type = op;
    node->left  = left;
    node->right = right;
    return node;
}

static node_expr_t* ParseExpr(stream_t stream)
{ return ParseExprMain(stream, 0); }

static node_call_t* ParseCall(stream_t stream)
{
    LexExpect(stream, "run");
    node_call_t* node = malloc(sizeof(node_call_t));
    node->target = ParseIden(stream);
    return node;
}

static node_assign_t* ParseAssign(stream_t stream)
{
    node_assign_t* node = malloc(sizeof(node_assign_t));
    node->destination = ParseIden(stream);
    LexExpect(stream, "=");
    node->source = ParseExpr(stream);
    return node;
}

static node_while_t* ParseWhile(stream_t stream)
{
    LexExpect(stream, "while");
    node_while_t* node = malloc(sizeof(node_while_t));
    node->condition = ParseExpr(stream);
    node->body      = ParseBlock(stream);
    return node;
}

static node_if_t* ParseIf(stream_t stream)
{
    LexExpect(stream, "if");
    node_if_t* node = malloc(sizeof(node_if_t));
    node->condition = ParseExpr(stream);
    node->body      = ParseBlock(stream);
    return node;
}

static node_print_t* ParsePrint(stream_t stream)
{
    LexExpect(stream, "#");
    node_print_t* node = malloc(sizeof(node_print_t));
    node->target = ParseExpr(stream);
    return node;
}

static node_statement_t* ParseStmt(stream_t stream)
{
    node_statement_t* node = malloc(sizeof(node_statement_t));
    const char* content = LexPeek(stream)->content;

    if (!strcmp(content, "run")) {
        node->content = ParseCall(stream);
        node->type    = T_STMT_CALL;
    } else if (!strcmp(content, "while")) {
        node->content = ParseWhile(stream);
        node->type    = T_STMT_WHILE;
    } else if (!strcmp(content, "if")) {
        node->content = ParseIf(stream);
        node->type    = T_STMT_IF;
    } else if (!strcmp(content, "#")) {
        node->content = ParsePrint(stream);
        node->type    = T_STMT_PRINT;
    } else {
        node->content = ParseAssign(stream);
        node->type    = T_STMT_ASSIGN;
    }

    LexExpect(stream, ";");

    return node;
}

static node_block_t* ParseBlock(stream_t stream)
{
    node_statement_t* buffer[4096];
    uint32_t size = 0;

    LexExpect(stream, "{");
    while (LexPeek(stream)->type != TT_RIGHT_BRACKET)
        buffer[size++] = ParseStmt(stream);

    LexExpect(stream, "}");

    uint32_t buffer_size = sizeof(node_statement_t) * size;
    node_statement_t** content = malloc(buffer_size);
    memcpy(content, buffer, buffer_size);

    node_block_t* node = malloc(sizeof(node_block_t));
    node->content = content;
    node->size = size;
    return node;
}


static node_proc_t* ParseProc(stream_t stream)
{
    LexExpect(stream, "proc");

    node_proc_t* node = malloc(sizeof(node_proc_t));
    node->name = LexPop(stream)->content;
    node->body = ParseBlock(stream);
    
    return node;
}


node_program_t* ParseProg(stream_t stream)
{
    node_proc_t* procs_buffer[4096];
    uint32_t procs_count = 0;

    char** vars = malloc(4096);
    uint32_t vars_count = 0;

    if (LexCheck(stream, "program"))
    {
        LexExpect(stream, "program");

        for(;;)
        {
            const char* content = LexPeek(stream)->content;

            if (!strcmp(content, "var"))
            {
                LexExpect(stream, "var");
                while (LexPeek(stream)->type != TT_STATEMENT_DELIMITER)
                {
                    vars[vars_count++] = LexPop(stream)->content;
                    if (LexPeek(stream)->type == TT_COMMA) LexPop(stream);
                }
                LexExpect(stream, ";");
            }

            if (!strcmp(content, "proc")) 
                procs_buffer[procs_count++] = ParseProc(stream);
            if (!strcmp(content, "end")) 
                goto done;
        }
    done:

        LexExpect(stream, "end");
    }

    node_program_t* prog = malloc(sizeof(node_program_t));

    size_t procs_buffer_size = sizeof(node_proc_t) * procs_count;
    prog->procs = malloc(procs_buffer_size);
    prog->procs_count = procs_count;
    memcpy(prog->procs, procs_buffer, procs_buffer_size);

    prog->vars = vars; 
    prog->vars_count = vars_count;

    return prog;
}

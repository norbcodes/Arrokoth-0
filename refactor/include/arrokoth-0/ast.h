
#ifndef ARROKOTH_AST_H
#define ARROKOTH_AST_H

#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    enum {
        T_ATOM_IDEN,
        T_ATOM_NUMBER,
        T_ATOM_SUBEXPR,
    } type;
    union {
        double number;
        char* iden;
        void* expr; // TODO: fix type
    };
}
node_atom_t;


typedef struct node_expr_s
{
    enum type_expr_e {
        T_EXPR_ADD,
        T_EXPR_MINUS,
        T_EXPR_MULT,
        T_EXPR_DIV,
        T_EXPR_EXP,
        T_EXPR_EQUAL,
        T_EXPR_UNEQUAL,
        T_EXPR_GREATER,
        T_EXPR_LESSER,
        T_EXPR_GE,
        T_EXPR_LE,

        T_EXPR_ATOM,
    } type;

    node_atom_t* leaf;

    struct node_expr_s* left;
    struct node_expr_s* right;
}
node_expr_t;





typedef struct
{
    void* content;

    enum {
        T_STMT_ASSIGN,
        T_STMT_CALL,
        T_STMT_WHILE,
        T_STMT_IF,
        T_STMT_PRINT,
    } type;
}
node_statement_t;


typedef struct
{
    char* destination;
    node_expr_t* source;
}
node_assign_t;

typedef struct
{
    char* target;
}
node_call_t;



typedef struct
{
    node_statement_t** content;
    size_t size;
}
node_block_t;



typedef struct
{
    node_expr_t* condition;
    node_block_t* body;
}
node_while_t;

typedef struct
{
    node_expr_t* condition;
    node_block_t* body;
}
node_if_t;

typedef struct
{
    node_expr_t* target;
}
node_print_t;

typedef struct
{
    char* name;
    node_block_t* body;
}
node_proc_t;

typedef struct 
{
    node_proc_t** procs;
    uint32_t procs_count;

    char** vars;
    uint32_t vars_count;
}
node_program_t;


void FreeRoot(node_program_t* node);

#endif // ARROKOTH_AST_H

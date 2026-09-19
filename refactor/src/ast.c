
#include <arrokoth-0/ast.h>


static void FreeBlock(node_block_t* node);
static void FreeExpr(node_expr_t* node);


static void FreeAtom(node_atom_t* node)
{
    if (node->type == T_ATOM_SUBEXPR)
        FreeExpr(node->expr);

    free(node);
}

static void FreeExpr(node_expr_t* node)
{
    if (node->type == T_EXPR_ATOM)
        FreeAtom(node->leaf);
    else
    {
        FreeExpr(node->left);
        FreeExpr(node->right);
    }

    free(node);
}


static void FreeAssign(node_assign_t* node)
{
    FreeExpr(node->source);
    free(node);
}
static void FreeCall(node_call_t* node)
{
    free(node);
}
static void FreeWhile(node_while_t* node)
{
    FreeExpr(node->condition);
    FreeBlock(node->body);
    free(node);
}
static void FreeIf(node_if_t* node)
{
    FreeExpr(node->condition);
    FreeBlock(node->body);
    free(node);
}
static void FreePrint(node_print_t* node)
{
    FreeExpr(node->target);
    free(node);
}


static void FreeStmt(node_statement_t* node)
{
    switch(node->type)
    {
        case T_STMT_ASSIGN: FreeAssign(node->content); break;
        case T_STMT_CALL  : FreeCall  (node->content); break;
        case T_STMT_WHILE : FreeWhile (node->content); break;
        case T_STMT_IF    : FreeIf    (node->content); break;
        case T_STMT_PRINT : FreePrint (node->content); break;
    }
    free(node);
}


static void FreeBlock(node_block_t* node)
{
    for (size_t i = 0; i < node->size; i++)
        FreeStmt(node->content[i]);
    free(node->content);
    free(node);
}


static void FreeProc(node_proc_t* node)
{
    FreeBlock(node->body);
    free(node);
}

static void FreeProg(node_program_t* node)
{
    for (size_t i = 0; i < node->procs_count; i++)
        FreeProc(node->procs[i]);
    free(node->procs);
    free(node->vars);
    free(node);
}

void FreeRoot(node_program_t* node)
{
    FreeProg(node);
}



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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arrokoth-0/graph.h>
#include <arrokoth-0/parser.h>
#include <arrokoth-0/frontend.h>
#include <arrokoth-0/ast.h>

static char* GraphExpr(FILE* fd, node_expr_t* expr);
static char* GraphBlock(FILE* fd, node_block_t* block);

static char* GraphAtom(FILE* fd, node_atom_t* node)
{
    static size_t num = 0;
    char name[256];


    char content[256];
    switch(node->type)
    {
        case T_ATOM_IDEN:    sprintf(content, "%s",  node->iden  ); break;
        case T_ATOM_NUMBER:  sprintf(content, "%lf", node->number); break;
        case T_ATOM_SUBEXPR: sprintf(content, "subexpr"); break;
    }

    sprintf(name, "atom%zu", num++);
    fprintf(fd, "%s [label=\"atom(%s)\",fillcolor=gray50,style=filled,fontcolor=black]\n", name, content);

    if (node->type == T_ATOM_SUBEXPR)
    {
        char* target = GraphExpr(fd, node->expr);
        fprintf(fd, "%s -> %s\n", name, target);
        free(target);
    }

    return strdup(name);
}


static char* GraphExpr(FILE* fd, node_expr_t* expr)
{
    static size_t num = 0;
    char name[256];
    sprintf(name, "expr%zu", num++);

    char* op;
    switch (expr->type)
    {
        case T_EXPR_ADD:     op = "+"; break;
        case T_EXPR_MINUS:   op = "-"; break;
        case T_EXPR_MULT:    op = "*"; break;
        case T_EXPR_DIV:     op = "/"; break;
        case T_EXPR_EXP:     op = "^"; break;
        case T_EXPR_EQUAL:   op = "=="; break;
        case T_EXPR_UNEQUAL: op = "!="; break;
        case T_EXPR_GREATER: op = ">"; break;
        case T_EXPR_LESSER:  op = "<"; break;
        case T_EXPR_GE:      op = ">="; break;
        case T_EXPR_LE:      op = "<="; break;
        case T_EXPR_ATOM:    op = "atom"; break;
    }

    fprintf(fd, "%s [label=\"expr(%s)\",fillcolor=yellow,style=filled,fontcolor=black]\n", name, op);

    if (expr->type == T_EXPR_ATOM)
    {
        char* target = GraphAtom(fd, expr->leaf);
        fprintf(fd, "%s -> %s\n", name, target);
        free(target);
    }
    else
    {
        char* target_left  = GraphExpr(fd, expr->left);
        char* target_right = GraphExpr(fd, expr->right);

        fprintf(fd, "%s -> %s\n", name, target_left);
        fprintf(fd, "%s -> %s\n", name, target_right);

        free(target_left);
        free(target_right);
    }

    return strdup(name);
}

static char* GraphPrint(FILE* fd, node_print_t* node)
{
    static size_t num = 0;
    char name[256];
    sprintf(name, "print%zu", num++);

    fprintf(fd, "%s [label=\"print\",fillcolor=darkorange1,style=filled,fontcolor=black]\n", name);

    char* target = GraphExpr(fd, node->target);
    fprintf(fd, "%s -> %s\n", name, target);
    free(target);

    return strdup(name);
}

static char* GraphCall(FILE* fd, node_call_t* node)
{
    static size_t num = 0;
    char name[256];
    sprintf(name, "func%zu", num++);

    fprintf(
        fd, 
        "%s [label=\"run(%s)\",fillcolor=green,style=filled,fontcolor=black]\n", 
        name, 
        node->target
    );

    return strdup(name);
}

static char* GraphAssign(FILE* fd, node_assign_t* node)
{
    static size_t num = 0;
    char name[256];
    sprintf(name, "assign%zu", num++);

    fprintf(
        fd, 
        "%s [label=\"assign(%s)\",fillcolor=blue,style=filled,fontcolor=white]\n", 
        name, 
        node->destination
    );

    char* target = GraphExpr(fd, node->source);
    fprintf(fd, "%s -> %s\n", name, target);
    free(target);

    return strdup(name);
}

static char* GraphWhile(FILE* fd, node_while_t* node)
{
    static size_t num = 0;
    char name[256];
    sprintf(name, "while%zu", num++);

    fprintf(fd, "%s [label=while,fillcolor=brown,style=filled,fontcolor=black]\n", name);

    char* cond_target = GraphExpr(fd, node->condition);
    fprintf(fd, "%s -> %s\n", name, cond_target);
    char* body_target = GraphBlock(fd, node->body);
    fprintf(fd, "%s -> %s\n", name, body_target);

    free(cond_target);
    free(body_target);

    return strdup(name);
}

static char* GraphIf(FILE* fd, node_if_t* node)
{
    static size_t num = 0;
    char name[256];
    sprintf(name, "if%zu", num++);

    fprintf(fd, "%s [label=if,fillcolor=cadetblue1,style=filled,fontcolor=black]\n", name);

    char* cond_target = GraphExpr(fd, node->condition);
    fprintf(fd, "%s -> %s\n", name, cond_target);
    char* body_target = GraphBlock(fd, node->body);
    fprintf(fd, "%s -> %s\n", name, body_target);

    free(cond_target);
    free(body_target);

    return strdup(name);
}

static char* GraphStmt(FILE* fd, node_statement_t* stmt)
{
    static size_t stmtNum = 0;
    char stmtName[256];
    sprintf(stmtName, "stmt%zu", stmtNum++);

    fprintf(fd, "%s [label=statement,fillcolor=gray28,style=filled,fontcolor=white]\n", stmtName);

    char* target;
    switch(stmt->type)
    {
        case T_STMT_ASSIGN: target = GraphAssign(fd, stmt->content); break;
        case T_STMT_CALL  : target = GraphCall  (fd, stmt->content); break;
        case T_STMT_WHILE : target = GraphWhile (fd, stmt->content); break;
        case T_STMT_IF    : target = GraphIf    (fd, stmt->content); break;
        case T_STMT_PRINT : target = GraphPrint (fd, stmt->content); break;
    }

    fprintf(fd, "%s -> %s\n", stmtName, target);
    free(target);

    return strdup(stmtName);
}

static char* GraphBlock(FILE* fd, node_block_t* block)
{
    static size_t blockNum = 0;
    char blockName[256];
    sprintf(blockName, "block%zu", blockNum++);

    fprintf(fd, "%s [label=block,fillcolor=gray10,style=filled,fontcolor=white]\n", blockName);

    for (size_t n = 0; n < block->size; n++)
    {
        char* target = GraphStmt(fd, block->content[n]);
        fprintf(fd, "%s -> %s\n", blockName, target);
        free(target);
    }

    return strdup(blockName);
}

static char* GraphProc(FILE* fd, node_proc_t* proc)
{
    static size_t procNum = 0;
    char procName[256];
    sprintf(procName, "\"proc(%s)%zu\"", proc->name, procNum++);
    fprintf(
        fd, 
        "%s [label=\"procedure(%s)\",fillcolor=purple,style=filled,fontcolor=white]\n", 
        procName,
        proc->name
    );

    char* target = GraphBlock(fd, proc->body);
    fprintf(fd, "%s -> %s\n", procName, target);
    free(target);

    return strdup(procName);
}

static void GraphProg(FILE* fd, node_program_t* root)
{
    fprintf(fd, "PROG [label=program,fillcolor=red,style=filled]\n");
    for (size_t n = 0; n < root->procs_count; n++)
    {
        char* target = GraphProc(fd, root->procs[n]);
        fprintf(fd, "PROG -> %s\n", target);
        free(target);
    }
}

void Graph(compiler_params_t params, node_program_t* root)
{
    char outputPath[256];
    sprintf(outputPath, "%s.dot", params.outputFile);

    bool isStdout = !strcmp(params.outputFile, "-");
    FILE* fd  = isStdout ? stdout : fopen(outputPath, "w");

    fprintf(fd, "digraph\n{\n");
    GraphProg(fd, root);
    fprintf(fd, "}\n");

    fclose(fd);
}

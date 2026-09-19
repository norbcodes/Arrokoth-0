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

#include <arrokoth-0/parser.h>
#include <arrokoth-0/codegen.h>
#include <arrokoth-0/frontend.h>

static void GenBlock(FILE* fd, node_block_t* node, node_program_t* root);
static char* LoadExpr(FILE* fd, node_expr_t* node, node_program_t* root);

static char* FreshTemp()
{
    static uint32_t count = 0;
    char buffer[256];
    sprintf(buffer, "%%tmp%d", count++);
    return strdup(buffer);
}
static char* FreshLabel()
{
    static uint32_t count = 0;
    char buffer[256];
    sprintf(buffer, "_label_%d", count++);
    return strdup(buffer);
}

static void CheckError(const char* format, char* name)
{
    fprintf(stderr, format, name);
    exit(1);
}

static void CheckProc(node_program_t* root, char* proc_name)
{
    for (size_t i = 0; i < root->procs_count; i++)
        if (!strcmp(root->procs[i]->name, proc_name))
            return;

    CheckError("Error: Procedure '%s' does not exist\n", proc_name);
}
static void CheckVar(node_program_t* root, char* var_name)
{
    for (size_t i = 0; i < root->vars_count; i++)
        if (!strcmp(root->vars[i], var_name))
            return;

    CheckError("Error: Variable '%s' does not exist\n", var_name);
}


static char* LoadAtom(FILE* fd, node_atom_t* node, node_program_t* root)
{
    char* target = FreshTemp();
    switch(node->type)
    {
        case T_ATOM_NUMBER: 
            fprintf(fd, "%s = fadd double 0.0, %lf\n", target, node->number);
            return target;

        case T_ATOM_IDEN:
            CheckVar(root, node->iden);
            fprintf(fd, "%s = load double, ptr @%s\n", target, node->iden);
            return target;

        case T_ATOM_SUBEXPR:
            free(target);
            return LoadExpr(fd, node->expr, root);

    }

    return NULL;
}

static char* LoadExpr(FILE* fd, node_expr_t* node, node_program_t* root)
{
    if (node->type == T_EXPR_ATOM)
        return LoadAtom(fd, node->leaf, root);

    char* left  = LoadExpr(fd, node->left,  root);
    char* right = LoadExpr(fd, node->right, root);
    char* target = FreshTemp();

    char* op;
    switch (node->type)
    {
        case T_EXPR_ADD:   op = "fadd"; goto normal;
        case T_EXPR_MINUS: op = "fsub"; goto normal;
        case T_EXPR_MULT:  op = "fmul"; goto normal;
        case T_EXPR_DIV:   op = "fdiv"; goto normal;

        case T_EXPR_EQUAL:   op = "fcmp oeq"; goto normal;
        case T_EXPR_UNEQUAL: op = "fcmp one"; goto normal;
        case T_EXPR_LESSER:  op = "fcmp olt"; goto normal;
        case T_EXPR_GREATER: op = "fcmp ogt"; goto normal;
        case T_EXPR_LE:      op = "fcmp ole"; goto normal;
        case T_EXPR_GE:      op = "fcmp oge"; goto normal;

        case T_EXPR_EXP:
            fprintf(
                fd, 
                "%s = call double(double,double) @pow(double %s, double %s)\n", 
                target, left, right
            );
            goto done;

        default:;
    }

normal:
    fprintf(fd, "%s = %s double %s, %s\n", target, op, left, right);
done:
    free(left);
    free(right);

    return target;
}



static void GenWhile(FILE* fd, node_while_t* node, node_program_t* root)
{
    char* cond_label = FreshLabel();
    fprintf(fd, "br label %%%s\n", cond_label);

    char* body_label = FreshLabel();
    char* done_label = FreshLabel();

    fprintf(fd, "%s:\n", cond_label);
    char* cond_result = LoadExpr(fd, node->condition, root);
    fprintf(fd, "br i1 %s, label %%%s, label %%%s\n", cond_result, body_label, done_label);

    fprintf(fd, "%s:\n", body_label);
    GenBlock(fd, node->body, root);

    fprintf(fd, "br label %%%s\n", cond_label);
    fprintf(fd, "%s:\n", done_label);

    free(cond_label);
    free(body_label);
    free(done_label);

    free(cond_result);
}

static void GenIf(FILE* fd, node_if_t* node, node_program_t* root)
{
    char* true_label  = FreshLabel();
    char* false_label = FreshLabel();

    char* cond_result = LoadExpr(fd, node->condition, root);

    fprintf(fd, "br i1 %s, label %%%s, label %%%s\n", cond_result, true_label, false_label);
    fprintf(fd, "%s:\n", true_label);

    GenBlock(fd, node->body, root);

    fprintf(fd, "br label %%%s\n", false_label);
    fprintf(fd, "%s:\n", false_label);

    free(true_label);
    free(false_label);

    free(cond_result);
}




static void GenAssign(FILE* fd, node_assign_t* node, node_program_t* root)
{
    CheckVar(root, node->destination);

    char* source = LoadExpr(fd, node->source, root);
    fprintf(fd, "store double %s, ptr @%s\n", source, node->destination);
    free(source);
}

static void GenCall(FILE* fd, node_call_t* node, node_program_t* root)
{
    CheckProc(root, node->target);
    fprintf(fd, "call void @%s()\n", node->target);
}




static void GenPrint(FILE* fd, node_print_t* node, node_program_t* root)
{
    char* tmp = LoadExpr(fd, node->target, root);
    fprintf(
        fd, 
        "call i32 (ptr,...) @printf(ptr @FORMATSTR, double %s)\n", 
        tmp
    );
    free(tmp);
}

static void GenStmt(FILE* fd, node_statement_t* node, node_program_t* root)
{
    switch (node->type)
    {
        case T_STMT_ASSIGN: GenAssign(fd, node->content, root); break;
        case T_STMT_CALL:   GenCall  (fd, node->content, root); break;
        case T_STMT_WHILE:  GenWhile (fd, node->content, root); break;
        case T_STMT_IF:     GenIf    (fd, node->content, root); break;
        case T_STMT_PRINT:  GenPrint (fd, node->content, root); break;
    }
}

static void GenBlock(FILE* fd, node_block_t* node, node_program_t* root)
{
    for (size_t i = 0; i < node->size; i++)
        GenStmt(fd, node->content[i], root);
}

static void GenProc(FILE* fd, node_proc_t* node, node_program_t* root)
{
    bool isMain = !strcmp(node->name, "main");

    fprintf(
        fd, 
        isMain ? "define i32 @%s()\n{\n" : "define void @%s()\n{\n",
        node->name
    );

    GenBlock(fd, node->body, root);

    fprintf(
        fd, 
        isMain ? "ret i32 0\n}\n" : "ret void \n}\n"
    );
}


static void GenProg(FILE* fd, node_program_t* prog)
{
    CheckProc(prog, "main");

    fprintf(fd, "@FORMATSTR = private constant [5 x i8] c\"%%lf\\0A\\00\"\n");
    fprintf(fd, "declare i32 @printf(ptr, ...)\n");
    fprintf(fd, "declare double @pow(double, double)\n\n");

    for (size_t i = 0; i < prog->vars_count; i++)
        fprintf(fd, "@%s = global double 0.0\n", prog->vars[i]);
    fprintf(fd, "\n");

    for (size_t i = 0; i < prog->procs_count; i++)
        GenProc(fd, prog->procs[i], prog);

}

void Codegen(compiler_params_t params, node_program_t* root)
{
    char outputPath[256];
    sprintf(outputPath, "%s.ll", params.outputFile);

    bool isStdout = !strcmp(params.outputFile, "-");
    FILE* fd  = isStdout ? stdout : fopen(outputPath, "w");

    GenProg(fd, root);

    fclose(fd);
}

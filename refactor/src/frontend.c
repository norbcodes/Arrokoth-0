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
#include <stdint.h>

#include <arrokoth-0/frontend.h>
#include <arrokoth-0/lexer.h>
#include <arrokoth-0/parser.h>
#include <arrokoth-0/graph.h>
#include <arrokoth-0/codegen.h>

//#include <arrokoth-0/semantic_analyzer.h>

void usage()
{
    fprintf(stderr, "Usage: ./a.out -i <input> -o <output> [-a] [-ntta]\n");
    exit(1);
}

void error(char* msg)
{
    fprintf(stderr, "Error: %s.\n", msg);
    usage();
}

void arg_error(char* before)
{
    fprintf(stderr, "Error: Expected argument after '%s'.\n", before);
    usage();
}


int main(int argc, char** argv)
{
    if (argc == 0) usage();

    compiler_params_t params = {
        .inputFile           = NULL,
        .outputFile          = NULL,
        .graph               = false,
    };

    #define READ_ARG ((n+1) < argc ? argv[++n] : (arg_error(argv[n]), NULL))
    for (int n = 1; n < argc; n++)
    {
        /**/ if (!strcmp(argv[n], "-i"))    params.inputFile  = READ_ARG;
        else if (!strcmp(argv[n], "-o"))    params.outputFile = READ_ARG;
        else if (!strcmp(argv[n], "-a"))    params.graph      = true;
    }

    if (!params.inputFile)  error("No input  file specified");
    if (!params.outputFile) error("No output file specified");

    bool isStdin = !strcmp(params.inputFile, "-");
    FILE* fd = isStdin ? stdin : fopen(params.inputFile, "r");

    if (!fd) error("Unable to open input file");

    token_t stream = Lex(fd);
    token_t streamer = stream;
    node_program_t* root = ParseProg(&streamer);

    if (params.graph)
        Graph(params, root);

    Codegen(params, root);
    
    FreeStream(stream);
    FreeRoot(root);
    return 0;
}

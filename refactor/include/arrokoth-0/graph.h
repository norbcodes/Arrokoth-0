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

#ifndef ARROKOTH_AST_GRAPH_H
#define ARROKOTH_AST_GRAPH_H

#include <arrokoth-0/parser.h>
#include <arrokoth-0/frontend.h>

void Graph(compiler_params_t params, node_program_t* root);

#endif // ARROKOTH_AST_GRAPH_H

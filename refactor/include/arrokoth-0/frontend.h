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

#ifndef ARROKOTH_FRONTEND_H
#define ARROKOTH_FRONTEND_H

#include <stdbool.h>

typedef struct compiler_params_s
{
    char* inputFile;
    char* outputFile;
    bool graph;
}
compiler_params_t;


#endif // ARROKOTH_FRONTEND_H

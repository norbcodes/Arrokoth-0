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

#ifndef ARROKOTH_LEXER_H
#define ARROKOTH_LEXER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum token_type_e
{
    TT_IDENTIFIER,
    TT_STATEMENT_DELIMITER,
    TT_LEFT_PAREN,
    TT_RIGHT_PAREN,
    TT_OPERATOR,
    TT_NUMBER,
    TT_COMMA,
    TT_LEFT_BRACKET,
    TT_RIGHT_BRACKET,
    TT_PRINT,
    TT_FORMATTING,
} token_type_t;

typedef struct token_s
{
    size_t line;
    token_type_t type;
    char* content;

    // NULL for end of token stream.
    struct token_s* next;
}
*token_t;

// Global var, fight me
// *fights you* >:3
//linked_list_node_t* TokenList = NULL;
typedef token_t* stream_t;


const char* TokenTypeToStr(token_type_t tt);
token_t Lex(FILE* fd);

token_t LexPop(stream_t stream);
token_t LexPeek(stream_t stream);
void LexExpect(stream_t stream, const char* ref);
bool LexCheck(stream_t stream, const char* ref);
void LexAssertType(token_t tok, token_type_t ref);

void FreeStream(token_t stream);


#endif // ARROKOTH_LEXER_H

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
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <arrokoth-0/lexer.h>

const char* TokenTypeToStr(token_type_t tt)
{
    switch (tt)
    {
        case TT_IDENTIFIER:          return "IDENTIFIER";
        case TT_STATEMENT_DELIMITER: return "STATEMENT_DELIMITER";
        case TT_LEFT_PAREN:          return "LEFT PAREN";
        case TT_RIGHT_PAREN:         return "RIGHT PAREN";
        case TT_OPERATOR:            return "OPERATOR";
        case TT_NUMBER:              return "NUMBER";
        case TT_COMMA:               return "COMMA";
        case TT_LEFT_BRACKET:        return "LEFT BRACKET";
        case TT_RIGHT_BRACKET:       return "RIGHT BRACKET";
        case TT_PRINT:               return "PRINT";
        default:                     return "<INVALID TOKEN TYPE>";
    }
}

static token_type_t GetTokenType(char c)
{
    if (c >= 'a' && c <= 'z') return TT_IDENTIFIER;
    if (c >= 'A' && c <= 'Z') return TT_IDENTIFIER;
    if (c == '_')             return TT_IDENTIFIER;

    if (c == ';') return TT_STATEMENT_DELIMITER;

    if (c == '(') return TT_LEFT_PAREN;
    if (c == ')') return TT_RIGHT_PAREN;

    if (c == '{') return TT_LEFT_BRACKET;
    if (c == '}') return TT_RIGHT_BRACKET;

    if ((c == '*') || (c == '/') || (c == '+') || (c == '-') || (c == '^')) return TT_OPERATOR;
    if ((c == '>') || (c == '<')) return TT_OPERATOR;
    if (c == '=') return TT_OPERATOR;
    if (c == '!') return TT_OPERATOR;

    if (c >= '0' && c <= '9') return TT_NUMBER;

    if (c == ',') return TT_COMMA;
    if (c == '#') return TT_PRINT;

    if ((c >= 9 && c <= 13) || (c == ' ')) return TT_FORMATTING;

    fprintf(stderr, "Warning: Unclassified character '%c'.\n", c);
    return TT_FORMATTING;
}


static void PushToken(token_t* iter, size_t line, token_type_t type, char* content)
{
    token_t new = malloc(sizeof(struct token_s));
    new->line = line;
    new->type = type;
    new->content = strdup(content);

    (*iter)->next = new;
    (*iter)       = new;

    new->next = NULL;
}

token_t Lex(FILE* fd)
{
    struct token_s hook;
    token_t iterator = &hook;


    fseek(fd, 0, SEEK_END);
    size_t size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    char* source = malloc(size);
    fread(source, sizeof(char), size, fd);


    token_type_t old = TT_FORMATTING;
    token_type_t new = TT_FORMATTING;

    char buffer[4096];
    char* ptr = buffer;
    bool comment = false;

    size_t line = 1;
    for (size_t i = 0; i < size; i++)
    {
        char c = source[i];
        if (c == '\n') line++;

        if (c == '[' ) comment = true;
        if (c == '\n') comment = false;
        if (comment) continue;

        new = GetTokenType(c);
        if (new != old)
        {
            if (old == TT_FORMATTING) goto skip;
            *ptr = '\0';
            PushToken(&iterator, line, old, buffer);
        skip:
            ptr = buffer;
        }

        old = new;
        *ptr++ = c; 
    }

    free(source);
    return hook.next;
}


token_t LexPop(stream_t stream)
{
    token_t curr = *stream;
    *stream = curr->next;
    return curr;
}
token_t LexPeek(stream_t stream)
{
    return *stream;
}

static void TokenError(const char* T1, const char* T2, size_t Line)
{
    fprintf(
        stderr, 
        "Error at line %zu: Expected '%s', got '%s' instead\n", 
        Line, 
        T1,
        T2
    );
    exit(1);
}

void LexExpect(stream_t stream, const char* ref)
{
    token_t tok = LexPop(stream);

    if (strcmp(tok->content, ref))
        TokenError(tok->content, ref, tok->line);
}
bool LexCheck(stream_t stream, const char* ref)
{
    token_t tok = LexPeek(stream);
    return !strcmp(tok->content, ref);
}
void LexAssertType(token_t tok, token_type_t ref)
{
    if (tok->type != ref) 
        TokenError(
            TokenTypeToStr(ref), 
            TokenTypeToStr(tok->type),
            tok->line
        );
}

void FreeStream(token_t stream)
{
    while (stream)
    {
        token_t next = stream->next; 
        free(stream->content);
        free(stream);
        stream = next;
    }
}


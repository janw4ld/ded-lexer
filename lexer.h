#ifndef LEXER_H_
#define LEXER_H_

#include <stddef.h>
#include <stdint.h>
#include "common.h"

typedef enum {
    TOKEN_END = 0,
    TOKEN_INVALID,
    TOKEN_PREPROC,
    TOKEN_SYMBOL,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_OPEN_CURLY,
    TOKEN_CLOSE_CURLY,
    TOKEN_SEMICOLON,
    TOKEN_KEYWORD,
    TOKEN_COMMENT,
    TOKEN_STRING,
    TOKEN_ARITHMETIC,
    TOKEN_ASSIGNMENT =0x0F00000,
    TOKEN_COMPARISON=0xF000000,
} Token_Kind;

const char *token_kind_name(Token_Kind kind);

typedef struct {
    Token_Kind kind;
    const char *text;
    size_t text_len;
} Token;

typedef struct {
    Token *items;
    size_t count;
    size_t capacity;
} Tokens;

typedef struct {
    const char *content;
    size_t content_len;
    size_t cursor;
} Lexer;

Lexer lexer_new(const char *content, size_t content_len);
Token lexer_next(Lexer *l);

#endif  // LEXER_H_

#include <assert.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "./common.h"
#include "./lexer.h"

typedef struct {
    Token_Kind kind;
    const char *text;
} Literal_Token;

Literal_Token literal_tokens[] = {
    {.text = "(", .kind = TOKEN_OPEN_PAREN},
    {.text = ")", .kind = TOKEN_CLOSE_PAREN},
    {.text = "{", .kind = TOKEN_OPEN_CURLY},
    {.text = "}", .kind = TOKEN_CLOSE_CURLY},
    {.text = ";", .kind = TOKEN_SEMICOLON},
    //! checking for `comparisons` must be before `literal_tokens`
    {.text = "=", .kind = TOKEN_ASSIGNMENT},
    {.text = "++", .kind = TOKEN_ARITHMETIC_ASSIGNMENT},
    {.text = "--", .kind = TOKEN_ARITHMETIC_ASSIGNMENT},
};
#define literal_tokens_count (sizeof(literal_tokens) / sizeof(literal_tokens[0]))

Literal_Token arithmetic_operators[] = {
    // TODO?: replace with unique kinds for each operator
    {.text = "+", .kind = TOKEN_ARITHMETIC},
    {.text = "-", .kind = TOKEN_ARITHMETIC},
    {.text = "*", .kind = TOKEN_ARITHMETIC},  // TODO: handle pointers
    {.text = "/", .kind = TOKEN_ARITHMETIC},
    {.text = "%", .kind = TOKEN_ARITHMETIC},
};
#define arithmetic_operators_count \
    (sizeof(arithmetic_operators) / sizeof(arithmetic_operators[0]))

Literal_Token comparison_operators[] = {
    // TODO?: replace with unique kinds for each comparison
    {.text = "==", .kind = TOKEN_COMPARISON},
    {.text = "!=", .kind = TOKEN_COMPARISON},
    {.text = "<=", .kind = TOKEN_COMPARISON},  // a1: these two tokens
    {.text = ">=", .kind = TOKEN_COMPARISON},  // a2: have to precede
    {.text = "<", .kind = TOKEN_COMPARISON},   // b1: these two
    {.text = ">", .kind = TOKEN_COMPARISON},   // b2:
};
#define comparison_operators_count \
    (sizeof(comparison_operators) / sizeof(comparison_operators[0]))

const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double",
    "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register",
    "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
    "union", "unsigned", "void", "volatile", "while", "alignas", "alignof", "and",
    "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept", "bitand",
    "bitor", "bool", "catch", "char16_t", "char32_t", "char8_t", "class", "co_await",
    "co_return", "co_yield", "compl", "concept", "const_cast", "consteval", "constexpr",
    "constinit", "decltype", "delete", "dynamic_cast", "explicit", "export", "false",
    "friend", "inline", "mutable", "namespace", "new", "noexcept", "not", "not_eq",
    "nullptr", "operator", "or", "or_eq", "private", "protected", "public", "reflexpr",
    "reinterpret_cast", "requires", "static_assert", "static_cast", "synchronized",
    "template", "this", "thread_local", "throw", "true", "try", "typeid", "typename",
    "using", "virtual", "wchar_t", "xor", "xor_eq",
};
#define keywords_count (sizeof(keywords) / sizeof(keywords[0]))

const char *token_kind_name(Token_Kind kind) {
    switch (kind) {
        case TOKEN_END:
            return "end of content";
        case TOKEN_INVALID:
            return "invalid token";
        case TOKEN_PREPROC:
            return "preprocessor directive";
        case TOKEN_COMMENT:
            return "comment";
        case TOKEN_STRING:
            return "string";
        case TOKEN_SYMBOL:
            return "symbol";
        case TOKEN_OPEN_PAREN:
            return "open paren";
        case TOKEN_CLOSE_PAREN:
            return "close paren";
        case TOKEN_OPEN_CURLY:
            return "open curly";
        case TOKEN_CLOSE_CURLY:
            return "close curly";
        case TOKEN_SEMICOLON:
            return "semicolon";
        case TOKEN_KEYWORD:
            return "keyword";
        case TOKEN_COMPARISON:
            return "comparison operator";
        case TOKEN_ASSIGNMENT:
            return "assignment";
        case TOKEN_ARITHMETIC:
            return "arithmetic operator";
        case TOKEN_ARITHMETIC_ASSIGNMENT:
            return "arithmetic assignment";
        default: break;
    }
    char error_buffer[25] = {0};
    snprintf(error_buffer, 25, "token_kind_name@kind=%d", kind);
    UNREACHABLE(error_buffer);
    return NULL;
}

Lexer lexer_new(const char *content, size_t content_len) {
    Lexer l = {0};
    // l.atlas = atlas;
    l.content = content;
    l.content_len = content_len;
    return l;
}

bool lexer_starts_with(Lexer *l, const char *prefix) {
    size_t prefix_len = strlen(prefix);
    if (prefix_len == 0) {
        return true;
    }
    if (l->cursor + prefix_len - 1 >= l->content_len) {
        return false;
    }
    for (size_t i = 0; i < prefix_len; ++i) {
        if (prefix[i] != l->content[l->cursor + i]) {
            return false;
        }
    }
    return true;
}

void lexer_chop_char(Lexer *l, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        // TODO: get rid of this assert by checking the length of the choped prefix upfront
        assert(l->cursor < l->content_len);
        l->cursor += 1;
    }
}

void lexer_trim_left(Lexer *l) {
    while (l->cursor < l->content_len && isspace(l->content[l->cursor])) {
        lexer_chop_char(l, 1);
    }
}

bool is_symbol_start(char x) {
    return isalpha(x) || x == '_';
}

bool is_symbol(char x) {
    return isalnum(x) || x == '_';
}

Token lexer_next(Lexer *l) {
    lexer_trim_left(l);

    Token token = {
        .text = &l->content[l->cursor],
    };

    if (l->cursor >= l->content_len) return token;

    if (l->content[l->cursor] == '"') {
        // TODO: TOKEN_STRING should also handle escape sequences
        token.kind = TOKEN_STRING;
        lexer_chop_char(l, 1);
        while (l->cursor < l->content_len &&
               l->content[l->cursor] != '"' &&
               l->content[l->cursor] != '\n') {
            lexer_chop_char(l, 1);
        }
        if (l->content[l->cursor] == '"') {
            lexer_chop_char(l, 1);
        }
        token.text_len = &l->content[l->cursor] - token.text;
        return token;
    }

    if (l->content[l->cursor] == '#') {
        // TODO: preproc should also handle newlines
        token.kind = TOKEN_PREPROC;
        while (l->cursor < l->content_len && l->content[l->cursor] != '\n') {
            lexer_chop_char(l, 1);
        }
        token.text_len = &l->content[l->cursor] - token.text;
        return token;
    }

    if (lexer_starts_with(l, "//")) {
        token.kind = TOKEN_COMMENT;
        while (l->cursor < l->content_len && l->content[l->cursor] != '\n') {
            lexer_chop_char(l, 1);
        }
        token.text_len = &l->content[l->cursor] - token.text;
        return token;
    }

    for (size_t i = 0; i < comparison_operators_count; ++i) {
        if (lexer_starts_with(l, comparison_operators[i].text)) {
            // NOTE: this code assumes that there is no newlines in comparisons[i].text
            size_t text_len = strlen(comparison_operators[i].text);
            token.kind = comparison_operators[i].kind;
            token.text_len = text_len;
            lexer_chop_char(l, text_len);
            return token;
        }
    }

    for (size_t i = 0; i < literal_tokens_count; ++i) {
        if (lexer_starts_with(l, literal_tokens[i].text)) {
            // NOTE: this code assumes that there is no newlines in literal_tokens[i].text
            size_t text_len = strlen(literal_tokens[i].text);
            token.kind = literal_tokens[i].kind;
            token.text_len = text_len;
            lexer_chop_char(l, text_len);
            return token;
        }
    }

    for (size_t i = 0; i < arithmetic_operators_count; ++i) {
        if (lexer_starts_with(l, arithmetic_operators[i].text)) {
            // NOTE: this code assumes that there is no newlines in arithmetic_operators_count[i].text
            size_t text_len = strlen(arithmetic_operators[i].text);
            Token_Kind kind = TOKEN_ARITHMETIC;
            lexer_chop_char(l, text_len);
            if (lexer_starts_with(l, "=")) {
                text_len += 1;
                kind &= TOKEN_ASSIGNMENT;
                lexer_chop_char(l, 1);
            }
            token.kind = kind;
            token.text_len = text_len;
            return token;
        }
    }

    if (is_symbol_start(l->content[l->cursor])) {
        token.kind = TOKEN_SYMBOL;
        while (l->cursor < l->content_len && is_symbol(l->content[l->cursor])) {
            lexer_chop_char(l, 1);
            token.text_len += 1;
        }

        for (size_t i = 0; i < keywords_count; ++i) {
            size_t keyword_len = strlen(keywords[i]);
            if (keyword_len == token.text_len &&
                memcmp(keywords[i], token.text, keyword_len) == 0) {
                token.kind = TOKEN_KEYWORD;
                break;
            }
        }

        return token;
    }

    lexer_chop_char(l, 1);
    token.kind = TOKEN_INVALID;
    token.text_len = 1;
    return token;
}

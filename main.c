#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "./common.h"
#include "./lexer.h"

typedef struct {
    size_t begin;
    size_t end;
} Line;

typedef struct {
    Line *items;
    size_t count;
    size_t capacity;
} Lines;

typedef struct {
    String_Builder data;
    Lines lines;
    Tokens tokens;
} Editor;

Errno editor_load_from_file(Editor *e, const char *file_path);
void editor_tokenize(Editor *e);

int main(int argc, char **argv) {
    static Editor e = {0};
    // Errno err;
    if (argc > 1) {
        const char *file_path = argv[1];
        Errno err = editor_load_from_file(&e, file_path);
        if (err != 0) {
            fprintf(stderr, "ERROR: Could not read file %s: %s\n",
                    file_path, strerror(err));
            return 1;
        }
        editor_tokenize(&e);
        for (uint32_t n = 0; n <= e.tokens.count; ++n) {
            Token t = e.tokens.items[n];
            printf("%.*s\t---\t%s\n", (uint32_t)t.text_len, t.text,
                   token_kind_name(t.kind));
        }
    }
}

Errno editor_load_from_file(Editor *e, const char *file_path) {
    printf("Loading %s\n", file_path);

    e->data.count = 0;
    Errno err = read_entire_file(file_path, &e->data);
    if (err != 0) return err;

    // e->cursor = 0;

    // editor_tokenize(e);

    // e->file_path.count = 0;
    // sb_append_cstr(&e->file_path, file_path);
    // sb_append_null(&e->file_path);

    return 0;
}

void editor_tokenize(Editor *e) {
    {  // lines
        e->lines.count = 0;

        Line line;
        line.begin = 0;

        for (size_t i = 0; i < e->data.count; ++i) {
            if (e->data.items[i] == '\n') {
                line.end = i;
                da_append(&e->lines, line);
                line.begin = i + 1;
            }
        }

        line.end = e->data.count;
        da_append(&e->lines, line);
    }
    {  // tokens
        e->tokens.count = 0;
        Lexer l = lexer_new(e->data.items, e->data.count);
        Token t = lexer_next(&l);
        while (t.kind != TOKEN_END) {
            da_append(&e->tokens, t);
            t = lexer_next(&l);
        }
    }
}

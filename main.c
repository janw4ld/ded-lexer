#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "./common.h"
#include "./lexer.h"

typedef struct {
    String_Builder data;
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
    }
}

Errno editor_load_from_file(Editor *e, const char *file_path) {
    printf("Loading %s\n", file_path);

    e->data.count = 0;
    Errno err = read_entire_file(file_path, &e->data);
    if (err != 0) return err;
    return 0;
}

void editor_tokenize(Editor *e) {
    e->tokens.count = 0;
    Lexer l = lexer_new(e->data.items, e->data.count);
    Token t = lexer_next(&l);
    while (t.kind != TOKEN_END) {
        printf("---\ntoken:%.*s\nkind:%s\n", (uint32_t)t.text_len, t.text,
               token_kind_name(t.kind));
        da_append(&e->tokens, t);
        t = lexer_next(&l);
    }
}

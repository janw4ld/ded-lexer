#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "common.h"

static Errno file_size(FILE *file, size_t *size) {
    long saved = ftell(file);
    if (saved < 0) return errno;
    if (fseek(file, 0, SEEK_END) < 0) return errno;
    long result = ftell(file);
    if (result < 0) return errno;
    if (fseek(file, saved, SEEK_SET) < 0) return errno;
    *size = (size_t)result;
    return 0;
}

Errno read_entire_file(const char *file_path, String_Builder *sb) {
    Errno result = 0;
    FILE *f = NULL;

    f = fopen(file_path, "r");
    if (f == NULL) return_defer(errno);

    size_t size;
    Errno err = file_size(f, &size);
    if (err != 0) return_defer(err);

    if (sb->capacity < size) {
        sb->capacity = size;
        sb->items = realloc(sb->items, sb->capacity * sizeof(*sb->items));
        assert(sb->items != NULL && "Buy more RAM lol");
    }

    fread(sb->items, size, 1, f);
    if (ferror(f)) return_defer(errno);
    sb->count = size;

defer:
    if (f) fclose(f);
    return result;
}

Errno editor_load_from_file(Editor *e, const char *file_path)
{
    printf("Loading %s\n", file_path);

    e->data.count = 0;
    Errno err = read_entire_file(file_path, &e->data);
    if (err != 0) return err;

    // e->cursor = 0;

    editor_retokenize(e);

    e->file_path.count = 0;
    sb_append_cstr(&e->file_path, file_path);
    sb_append_null(&e->file_path);

    return 0;
}
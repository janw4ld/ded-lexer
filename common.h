#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

#define DA_INIT_CAP 256

#define da_append(da, item)                                                            \
    do {                                                                               \
        if ((da)->count >= (da)->capacity) {                                           \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity * 2;   \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items)); \
            assert((da)->items != NULL && "Buy more RAM lol");                         \
        }                                                                              \
                                                                                       \
        (da)->items[(da)->count++] = (item);                                           \
    } while (0)

#define da_append_many(da, new_items, new_items_count)                                        \
    do {                                                                                      \
        if ((da)->count + new_items_count > (da)->capacity) {                                 \
            if ((da)->capacity == 0) {                                                        \
                (da)->capacity = DA_INIT_CAP;                                                 \
            }                                                                                 \
            while ((da)->count + new_items_count > (da)->capacity) {                          \
                (da)->capacity *= 2;                                                          \
            }                                                                                 \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));        \
            assert((da)->items != NULL && "Buy more RAM lol");                                \
        }                                                                                     \
        memcpy((da)->items + (da)->count, new_items, new_items_count * sizeof(*(da)->items)); \
        (da)->count += new_items_count;                                                       \
    } while (0)

#define sb_append_cstr(sb, cstr)  \
    do {                          \
        const char *s = (cstr);   \
        size_t n = strlen(s);     \
        da_append_many(sb, s, n); \
    } while (0)
#define sb_append_null(sb) da_append_many(sb, "", 1)

#define UNIMPLEMENTED(...)                                                      \
    do {                                                                        \
        printf("%s:%d: UNIMPLEMENTED: %s \n", __FILE__, __LINE__, __VA_ARGS__); \
        exit(1);                                                                \
    } while (0)
#define UNREACHABLE(...)                                                      \
    do {                                                                      \
        printf("%s:%d: UNREACHABLE: %s \n", __FILE__, __LINE__, __VA_ARGS__); \
        exit(1);                                                              \
    } while (0)
#define UNUSED(x) (void)(x)

#define return_defer(value) \
    do {                    \
        result = (value);   \
        goto defer;         \
    } while (0)

typedef uint32_t Errno;

typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} String_Builder;

Errno read_entire_file(const char *file_path, String_Builder *sb);

#endif  // COMMON_H_

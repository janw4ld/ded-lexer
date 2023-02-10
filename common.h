#include <stddef.h>

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

typedef int Errno;
typedef struct {
    char *items;
    size_t count;
    size_t capacity;
} String_Builder;
Errno read_entire_file(const char *file_path, String_Builder *sb);

#pragma once

#define getBitFlag(v, f) ((v) & (1 << (f)) ? 1 : 0)
#define setBitFlag(v, f) (*(v) = (*(v) | (1 << (f))))
#define clearBitFlag(v, f) (*(v) = *(v) ^ (*(v) & (1 << (f))))

#define arrayInit(type, ...) ((type[]){__VA_ARGS__})
#define arrayArg(type, ...) arrayInit(type, __VA_ARGS__)
#define createArray(type, count, reset) (reset ? (type *)xcalloc(count, sizeof(type)) : (type *)xmalloc(count * sizeof(type))) 
#define arrayLength(arr) (sizeof(arr) / sizeof((arr)[0]))
#define chance(p) ((rand() % 100) < p)
#define chancef(p) ((rand() / RAND_MAX) < p)
#define randomi(a, b) ((a) + (rand() % ((b) - (a))))
#define randomf(a, b) ((a) + ((f32)rand() / RAND_MAX) * ((b) - (a)))
#define inRange(x, a, b) ((x) >= (a) && (x) < (b))
#define debugVar(x) printf("variable "#x" = %d\n", x)

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef unsigned char uchar;

typedef size_t size32;

typedef union {
    u8 rgba[4];
    struct {
        u8 r,g,b,a;
    };
} u8Color;

#define NULL 0

#define internal static
#define global static
#define local static

#define DEFAULT_BUFFER_SIZE 1024

#define NOT_IMPLEMENTED fprintf(stderr, "Not implemented at %s (%d)", __FILE__, __LINE__)

#define readu8(arr, index) (arr[index])
#define reads16(arr, index) (*(s16*)(rawResource.data + (index)))
#define readu16(arr, index) (*(u16*)(rawResource.data + (index)))
#define reads32(arr, index) (*(s32*)(rawResource.data + (index)))
#define readu32(arr, index) (*(u32*)(rawResource.data + (index)))

void* xmalloc(size32 size, char *file, s32 line)
{
    void *ptr = malloc(size);
    if (!ptr)
    {
        perror("xmalloc failed!");
        if (file)
        {
            fprintf(stderr, "  at file %s (%d)\n", file, line);
        }
        exit(1);
    }

    return ptr;
}

void* xcalloc(size32 count, size32 size, char *file, s32 line)
{
    void *ptr = calloc(count, size);
    if (!ptr)
    {
        perror("xcalloc failed!");
        if (file)
        {
            fprintf(stderr, "  at file %s (%d)\n", file, line);
        }
        exit(1);
    }

    return ptr;
}

void* xrealloc(void *ptr, size32 size, char *file, s32 line)
{
    ptr = realloc(ptr, size);
    if (!ptr)
    {
        perror("xrealloc failed!");
        if (file)
        {
            fprintf(stderr, "  at file %s (%d)\n", file, line);
        }
        exit(1);
    }

    return ptr;
}

#define xmalloc(size) xmalloc(size, __FILE__, __LINE__)
#define xcalloc(count, size) xcalloc(count, size, __FILE__, __LINE__)
#define xrealloc(ptr, size) xrealloc(ptr, size, __FILE__, __LINE__)

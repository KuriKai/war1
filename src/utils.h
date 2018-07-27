#pragma once

#define getBitFlag(v, f) ((v) & (1 << (f)) ? 1 : 0)
#define setBitFlag(v, f) (*(v) = (*(v) | (1 << (f))))
#define clearBitFlag(v, f) (*(v) = *(v) ^ (*(v) & (1 << (f))))

#define arrayLength(arr) (sizeof(arr) / sizeof(*arr))

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

typedef size_t size32;

#define null 0

#define internal static
#define global static
#define local static

#define DEFAULT_BUFFER_SIZE 1024

#define readu8(arr, index) (arr[index])
#define reads16(arr, index) (*(s16*)(rawResource.data + (index)))
#define readu16(arr, index) (*(u16*)(rawResource.data + (index)))
#define reads32(arr, index) (*(s32*)(rawResource.data + (index)))
#define readu32(arr, index) (*(u32*)(rawResource.data + (index)))

/* time stuff */
// global s64 globalPerfCountFrequency;
// global LARGE_INTEGER initialCounter;

// inline LARGE_INTEGER getWallClock(void)
// {
//     LARGE_INTEGER result;
//     QueryPerformanceCounter(&result);
//     return result;
// }

// inline f32 getSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
// {
//     f32 result = ((f32)(end.QuadPart - start.QuadPart) / (f32)globalPerfCountFrequency);
//     return result;
// }

// inline void initWallClock()
// {
//     LARGE_INTEGER perfCountFrequencyResult;
//     QueryPerformanceFrequency(&perfCountFrequencyResult);

//     globalPerfCountFrequency = perfCountFrequencyResult.QuadPart;
//     initialCounter = getWallClock();
// }

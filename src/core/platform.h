#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef unsigned int uint;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#ifdef _WIN32

#ifdef _WIN64
#define PLATFORM_WINDOWS

#else
#error "x86 Builds are not supported!"

#endif

#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>

#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS simulator is not supported!"

#elif TARGET_OS_IPHONE == 1
#define PLATFORM_IOS

#elif TARGET_OS_MAC == 1
#define PLATFORM_MACOS

#else
#error "Unknown Apple platform!"

#endif

#elif defined(__ANDROID__)

#define PLATFORM_ANDROID
#error "Android is not supported!"

#elif defined(__linux__)

#define PLATFORM_LINUX
#else

#error "Unknown platform!"
#endif

#endif // PLATFORM_H

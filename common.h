#ifndef COMMON_H_
#define COMMON_H_

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;
typedef enum { false, true } bool;
typedef unsigned char uchar;

#define AX_DEBUG(n, ...) fprintf(stdout, "[DEBUG][%s][%s][%d] " n "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define AX_PRINT(n, ...) fprintf(stdout, n "\n", ##__VA_ARGS__)

#define UNPACKRGB16_I(x)\
(U8)(((x) & 0xFF0000) >> 11),\
(U8)(((x) & 0x00FF00) >> 5),\
(U8)(((x) & 0x0000FF))\

#define UNPACKRGB32_I(x)\
(U8)(((x) & 0xFF000000) >> 24),\
(U8)(((x) & 0x00FF0000) >> 16),\
(U8)(((x) & 0x0000FF00) >> 8)\
(U8)(((x) & 0x000000FF))\

#define UNPACKRGBA_F(x)\
(float)(((x) & 0xFF000000) >> 24) / 255.0f,\
(float)(((x) & 0x00FF0000) >> 16) / 255.0f,\
(float)(((x) & 0x0000FF00) >> 8) / 255.0f,\
(float)((x) & 0x000000FF) / 255.0f\

#define PACKRGBA(R,G,B,A) (U32)(R << 24 | G << 16 | B << 8| A)
#define PACKRGB16(R,G,B) (U16)(R << 11 | G << 5 | B)

#define LIMIT(x, low, high) (((x) > (high)) ? (high) : ((x) < (low)) ? (low) : (x))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#endif // COMMON_H_

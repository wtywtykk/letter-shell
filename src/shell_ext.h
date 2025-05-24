/**
 * @file shell_ext.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell extensions
 * @version 3.0.0
 * @date 2019-12-31
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#ifndef __SHELL_EXT_H__
#define __SHELL_EXT_H__

#include "shell.h"
#include <stddef.h>

typedef enum
{
    LSH_INT,

    LSH_U8,
    LSH_U16,
    LSH_U32,
    LSH_U64,
    LSH_UINT_PLT,

    LSH_S8,
    LSH_S16,
    LSH_S32,
    LSH_S64,
    LSH_SINT_PLT,

    LSH_F32,
    LSH_F64,

    LSH_CH,
    LSH_STR,
} ShellParamType;

typedef int shellArgType_LSH_INT;

typedef uint8_t shellArgType_LSH_U8;
typedef uint16_t shellArgType_LSH_U16;
typedef uint32_t shellArgType_LSH_U32;
typedef uint64_t shellArgType_LSH_U64;

typedef int8_t shellArgType_LSH_S8;
typedef int16_t shellArgType_LSH_S16;
typedef int32_t shellArgType_LSH_S32;
typedef int64_t shellArgType_LSH_S64;

typedef float shellArgType_LSH_F32;
typedef double shellArgType_LSH_F64;

typedef char shellArgType_LSH_CH;
typedef char *shellArgType_LSH_STR;

// 似乎SHELL_CONCAT2已经被用过了，宏展开时会被跳过
#define SHELL_TYPE_CONCAT_IMPL(x, y) x##y
#define SHELL_TYPE_CONCAT(x, y) SHELL_TYPE_CONCAT_IMPL(x, y)

typedef SHELL_TYPE_CONCAT(shellArgType_LSH_U, SHELL_PLAT_BITS) shellArgType_LSH_UINT_PLT;
typedef SHELL_TYPE_CONCAT(shellArgType_LSH_S, SHELL_PLAT_BITS) shellArgType_LSH_SINT_PLT;
#define shellArgParse_LSH_UINT_PLT SHELL_TYPE_CONCAT(shellArgParse_LSH_U, SHELL_PLAT_BITS)
#define shellArgParse_LSH_SINT_PLT SHELL_TYPE_CONCAT(shellArgParse_LSH_S, SHELL_PLAT_BITS)

typedef SHELL_TYPE_CONCAT(shellArgType_LSH_S, SHELL_INT_BITS) shellArgType_LSH_INT;
#define shellArgParse_LSH_INT SHELL_TYPE_CONCAT(shellArgParse_LSH_S, SHELL_INT_BITS)

int shellArgParse_LSH_U8(int *pargc, char **pargv[], uint8_t *pout);
int shellArgParse_LSH_U16(int *pargc, char **pargv[], uint16_t *pout);
int shellArgParse_LSH_U32(int *pargc, char **pargv[], uint32_t *pout);
int shellArgParse_LSH_U64(int *pargc, char **pargv[], uint64_t *pout);

int shellArgParse_LSH_S8(int *pargc, char **pargv[], int8_t *pout);
int shellArgParse_LSH_S16(int *pargc, char **pargv[], int16_t *pout);
int shellArgParse_LSH_S32(int *pargc, char **pargv[], int32_t *pout);
int shellArgParse_LSH_S64(int *pargc, char **pargv[], int64_t *pout);

int shellArgParse_LSH_CH(int *pargc, char **pargv[], char *pout);
int shellArgParse_LSH_STR(int *pargc, char **pargv[], char **pout);

/**
 * @brief 数字类型
 * 
 */
typedef enum
{
    NUM_TYPE_DEC,                                           /**< 十进制整型 */
    NUM_TYPE_BIN,                                           /**< 二进制整型 */
    NUM_TYPE_OCT,                                           /**< 八进制整型 */
    NUM_TYPE_HEX,                                           /**< 十六进制整型 */
    NUM_TYPE_FLOAT                                          /**< 浮点型 */
} ShellNumType;

#if SHELL_SUPPORT_ARRAY_PARAM == 1
typedef struct
{
    unsigned short size;
    unsigned char elementBytes;
} ShellArrayHeader;
#endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */

int shellExtParsePara(Shell *shell, char *string, char *type, size_t *result);
#if SHELL_SUPPORT_ARRAY_PARAM == 1
int shellGetArrayParamSize(void *param);
#endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */
int shellExtRun(Shell *shell, ShellCommand *command, int argc, char *argv[]);

#endif

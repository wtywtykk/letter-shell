/**
 * @file shell_ext.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell extensions
 * @version 3.0.0
 * @date 2019-12-31
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "shell.h"
#include "shell_ext.h"
#include "string.h"

extern ShellCommand* shellSeekCommand(Shell *shell,
                                      const char *cmd,
                                      ShellCommand *base,
                                      unsigned short compareLength);
extern int shellGetVarValue(Shell *shell, ShellCommand *command);

#if SHELL_SUPPORT_ARRAY_PARAM == 1
extern int shellSplit(char *string, unsigned short strLen, char *array[], char splitKey, short maxNum);

static int shellExtParseArray(Shell *shell, char *string, char *type, size_t *result);
static int shellExtCleanerArray(Shell *shell, char *type, void *param);
#endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */

/**
 * @brief 获取期待的参数个数
 * 
 * @param signature 函数签名
 * 
 * @return int 参数个数
 */
static int shellGetParamNumExcept(const char *signature)
{
    int num = 0;
    const char *p = signature;
    
    while (*p)
    {
    #if SHELL_SUPPORT_ARRAY_PARAM == 1
        if (*p == '[')
        {
            p++;
        }
    #endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */
        if (*p == 'L')
        {
            while (*p != ';' && *p != 0)
            {
                p++;
            }
            p++;
        }
        else
        {
            p++;
        }
        num++;
    }
    return num;
}

/**
 * @brief 判断数字进制
 * 
 * @param string 参数字符串
 * @return ShellNumType 进制
 */
static ShellNumType shellExtNumType(char *string)
{
    char *p = string;
    ShellNumType type = NUM_TYPE_DEC;

    if ((*p == '0') && ((*(p + 1) == 'x') || (*(p + 1) == 'X')))
    {
        type = NUM_TYPE_HEX;
    }
    else if ((*p == '0') && ((*(p + 1) == 'b') || (*(p + 1) == 'B')))
    {
        type = NUM_TYPE_BIN;
    }
    else if (*p == '0')
    {
        type = NUM_TYPE_OCT;
    }
    
    while (*p++)
    {
        if (*p == '.' && *(p + 1) != 0)
        {
            type = NUM_TYPE_FLOAT;
            break;
        }
    }

    return type;
}


/**
 * @brief 字符转数字
 * 
 * @param code 字符
 * @return char 数字
 */
static char shellExtToNum(char code)
{
    if ((code >= '0') && (code <= '9'))
    {
        return code -'0';
    }
    else if ((code >= 'a') && (code <= 'f'))
    {
        return code - 'a' + 10;
    }
    else if ((code >= 'A') && (code <= 'F'))
    {
        return code - 'A' + 10;
    }
    else
    {
        return 0;
    }
}


/**
 * @brief 解析字符参数
 * 
 * @param string 字符串参数
 * @return char 解析出的字符
 */
static char shellExtParseChar(char *string)
{
    char *p = (*string == '\'') ? (string + 1) : string;
    char value = 0;

    if (*p == '\\')
    {
        switch (*(p + 1))
        {
        case 'b':
            value = '\b';
            break;
        case 'r':
            value = '\r';
            break;
        case 'n':
            value = '\n';
            break;
        case 't':
            value = '\t';
            break;
        case '0':
            value = 0;
            break;
        default:
            value = *(p + 1);
            break;
        }
    }
    else
    {
        value = *p;
    }
    return value;
}


/**
 * @brief 解析字符串参数
 * 
 * @param string 字符串参数
 * @return char* 解析出的字符串
 */
static char* shellExtParseString(char *string)
{
    char *p = string;
    unsigned short index = 0;

    if (*string == '\"')
    {
        p = ++string;
    }

    while (*p)
    {
        if (*p == '\\')
        {
            *(string + index) = shellExtParseChar(p);
            p++;
        }
        else if (*p == '\"')
        {
            *(string + index) = 0;
        }
        else
        {
            *(string + index) = *p;
        }
        p++;
        index ++;
    }
    *(string + index) = 0;
    return string;
}


/**
 * @brief 解析数字参数
 * 
 * @param string 字符串参数
 * @return size_t 解析出的数字
 */
static uintptr_t shellExtParseNumber(char *string)
{
    ShellNumType type = NUM_TYPE_DEC;
    char radix = 10;
    char *p = string;
    char offset = 0;
    signed char sign = 1;
    size_t valueInt = 0;
    float valueFloat = 0.0;
    size_t devide = 0;

    if (*string == '-')
    {
        sign = -1;
    }

    type = shellExtNumType(string + ((sign == -1) ? 1 : 0));

    switch ((char)type)
    {
    case NUM_TYPE_HEX:
        radix = 16;
        offset = 2;
        break;
    
    case NUM_TYPE_OCT:
        radix = 8;
        offset = 1;
        break;

    case NUM_TYPE_BIN:
        radix = 2;
        offset = 2;
        break;
    
    default:
        break;
    }

    p = string + offset + ((sign == -1) ? 1 : 0);

    while (*p)
    {
        if (*p == '.')
        {
            devide = 1;
            p++;
            continue;
        }
        valueInt = valueInt * radix + shellExtToNum(*p);
        devide *= 10;
        p++;
    }
    if (type == NUM_TYPE_FLOAT && devide != 0)
    {
        valueFloat = (float)valueInt / devide * sign;
        return *(size_t *)(&valueFloat);
    }
    else
    {
        return valueInt * sign;
    }
}


/**
 * @brief 解析变量参数
 * 
 * @param shell shell对象
 * @param var 变量
 * @param result 解析结果
 *
 * @return int 0 解析成功 --1 解析失败
 */
static int shellExtParseVar(Shell *shell, char *var, size_t *result)
{
    ShellCommand *command = shellSeekCommand(shell,
                                             var + 1,
                                             shell->commandList.base,
                                             0);
    if (command)
    {
        *result = shellGetVarValue(shell, command);
        return 0;
    }
    else
    {
        return -1;
    }
}


/**
 * @brief 解析参数
 * 
 * @param shell shell对象
 * @param string 参数
 * @param type 参数类型
 * @param result 解析结果
 * 
 * @return int 0 解析成功 --1 解析失败
 */
int shellExtParsePara(Shell *shell, char *string, char *type, size_t *result)
{
    if (type == NULL || (*string == '$' && *(string + 1)))
    {
        if (*string == '\'' && *(string + 1))
        {
            *result = (size_t)shellExtParseChar(string);
            return 0;
        }
        else if (*string == '-' || (*string >= '0' && *string <= '9'))
        {
            *result = shellExtParseNumber(string);
            return 0;
        }
        else if (*string == '$' && *(string + 1))
        {
            return shellExtParseVar(shell, string, result);
        }
        else if (*string)
        {
            *result = (size_t)shellExtParseString(string);
            return 0;
        }
    }
    else
    {
        if (*string == '$' && *(string + 1))
        {
            return shellExtParseVar(shell, string, result);
        }
    #if SHELL_SUPPORT_ARRAY_PARAM == 1
        else if (type[0] == '[')
        {
            return shellExtParseArray(shell, string, type, result);
        }
    #endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */
        else if (strcmp("c", type) == 0)
        {
            *result = (size_t)shellExtParseChar(string);
            return 0;
        }
        else if (strcmp("q", type) == 0
                 || strcmp("h", type) == 0
                 || strcmp("i", type) == 0
                 || strcmp("f", type) == 0
                 || strcmp("p", type) == 0)
        {
            *result = shellExtParseNumber(string);
            return 0;
        }
        else if (strcmp("s", type) == 0)
        {
            *result = (size_t)shellExtParseString(string);
            return 0;
        }
        else
        {
            ShellCommand *command = shellSeekCommand(shell,
                                                     type,
                                                     shell->commandList.base,
                                                     0);
            if (command != NULL)
            {
            }
            else
            {
                shellWriteString(shell, "Can't find the param parser for type: ");
                shellWriteString(shell, type);
                shellWriteString(shell, "\r\n");
                return -1;
            }
        }
    }
    return -1;
}


#if SHELL_SUPPORT_ARRAY_PARAM == 1
/**
 * @brief 估算数组长度
 * 
 * @param string 数组参数
 * 
 * @return int 估算的数组长度
 */
static int shellEstimateArrayLength(char *string)
{
    int length = 0;
    char *p = string;
    while (*p)
    {
        if (*p == ',')
        {
            length++;
        }
        p++;
    }
    return length + 1;
}

/**
 * @brief 分割数组参数
 * 
 * @param string 数组参数
 * @param array 分割后的字符串数组
 * 
 * @return int 数组长度
 */
static int shellSplitArray(char *string, char ***array)
{
    int strLen = strlen(string);
    if (string[strLen - 1] == ']')
    {
        string[--strLen] = 0;
    }
    if (string[0] == '[')
    {
        --strLen;
        string++;
    }
    int size = shellEstimateArrayLength(string);
    *array = SHELL_MALLOC(size * sizeof(char *));
    return shellSplit(string, strLen, *array, ',', size);
}

/**
 * @brief 解析数组参数
 * 
 * @param shell shell 对象
 * @param string 数组参数
 * @param type 参数类型
 * @param result 解析结果
 * 
 * @return int 0 解析成功 -1 解析失败
 */
static int shellExtParseArray(Shell *shell, char *string, char *type, size_t *result)
{
    char **params;
    int size = shellSplitArray(string, &params);
    int elementBytes = sizeof(void *);

    if (strcmp(type + 1, "q") == 0)
    {
        elementBytes = sizeof(char);
    }
    else if (strcmp(type + 1, "h") == 0)
    {
        elementBytes = sizeof(short);
    }
    else if (strcmp(type + 1, "i") == 0)
    {
        elementBytes = sizeof(int);
    }

    ShellArrayHeader *header = SHELL_MALLOC(elementBytes * size + sizeof(ShellArrayHeader));
    *result = (size_t) ((size_t) header + sizeof(ShellArrayHeader));
    header->size = size;
    header->elementBytes = elementBytes;
    for (short i = 0; i < size; i++)
    {
        size_t value;
        if (shellExtParsePara(shell, params[i], type + 1, &value) != 0)
        {
            SHELL_FREE(header);
            SHELL_FREE(params);
            return -1;
        }
        memcpy((void *) ((size_t) *result + elementBytes * i), &value, elementBytes);
    }

    SHELL_FREE(params);
    return 0;
}

/**
 * @brief 获取数组大小
 * 
 * @param param 数组
 * 
 * @return int 数组大小
 */
int shellGetArrayParamSize(void *param)
{
    ShellArrayHeader *header = (ShellArrayHeader *) ((size_t) param - sizeof(ShellArrayHeader));
    return header->size;
}
#endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */


int shellArgParse_LSH_U8(int *pargc, char **pargv[], uint8_t *pout);
int shellArgParse_LSH_U16(int *pargc, char **pargv[], uint16_t *pout);
int shellArgParse_LSH_U32(int *pargc, char **pargv[], uint32_t *pout);
int shellArgParse_LSH_U64(int *pargc, char **pargv[], uint64_t *pout);

int shellArgParse_LSH_S8(int *pargc, char **pargv[], int8_t *pout);
int shellArgParse_LSH_S16(int *pargc, char **pargv[], int16_t *pout);
int shellArgParse_LSH_S32(int *pargc, char **pargv[], int32_t *pout)
{
    if (*pargc <= 0)
        return -1;

    *pout = shellExtParseNumber(**pargv);
    (*pargc)--;
    (*pargv)++;
    return 0;
}

int shellArgParse_LSH_S64(int *pargc, char **pargv[], int64_t *pout)
{
    if (*pargc <= 0)
        return -1;

    *pout = shellExtParseNumber(**pargv);
    (*pargc)--;
    (*pargv)++;
    return 0;
}


int shellArgParse_LSH_CH(int *pargc, char **pargv[], char *pout)
{
    if (*pargc <= 0)
        return -1;

    if (*((**pargv) + 1) != 0)
        return -1;
    *pout = ***pargv;
    (*pargc)--;
    (*pargv)++;
    return 0;
}

int shellArgParse_LSH_STR(int *pargc, char **pargv[], char** pout)
{
    if(*pargc <= 0) return -1;
    
    *pout = **pargv;
    (*pargc)--;
    (*pargv)++;
    return 0;
}

/**
 * @file shell.h
 * @author Letter (NevermindZZT@gmail.com)
 * @brief letter shell
 * @version 3.0.0
 * @date 2019-12-30
 * 
 * @copyright (c) 2020 Letter
 * 
 */

#ifndef     __SHELL_H__
#define     __SHELL_H__

#ifdef SHELL_CFG_USER
#include SHELL_CFG_USER
#endif
#include "shell_cfg.h"

#include <stdint.h>
#include <limits.h>

#define     SHELL_VERSION               "3.2.4"                 /**< 版本号 */

#if UINTPTR_MAX == UINT8_MAX
#define SHELL_PLAT_BITS 8
#elif UINTPTR_MAX == UINT16_MAX
#define SHELL_PLAT_BITS 16
#elif UINTPTR_MAX == UINT32_MAX
#define SHELL_PLAT_BITS 32
#elif UINTPTR_MAX == UINT64_MAX
#define SHELL_PLAT_BITS 64
#else
#error "Unsupported platform width"
#endif

#if INT_MAX == INT8_MAX
#define SHELL_INT_BITS 8
#elif INT_MAX == INT16_MAX
#define SHELL_INT_BITS 16
#elif INT_MAX == INT32_MAX
#define SHELL_INT_BITS 32
#elif INT_MAX == INT64_MAX
#define SHELL_INT_BITS 64
#error "Unsupported int width"
#endif

/**
 * @brief shell 断言
 * 
 * @param expr 表达式
 * @param action 断言失败操作
 */
#define     SHELL_ASSERT(expr, action) \
            if (!(expr)) { \
                action; \
            }

#if SHELL_USING_LOCK == 1
#define     SHELL_LOCK(shell)           shell->lock(shell)
#define     SHELL_UNLOCK(shell)         shell->unlock(shell)
#else
#define     SHELL_LOCK(shell)
#define     SHELL_UNLOCK(shell)
#endif /** SHELL_USING_LOCK == 1 */
/**
 * @brief shell 命令权限
 * 
 * @param permission 权限级别
 */
#define     SHELL_CMD_PERMISSION(permission) \
            (permission & 0x000000FF)

/**
 * @brief shell 命令类型
 * 
 * @param type 类型
 */
#define     SHELL_CMD_TYPE(type) \
            ((type & 0x0000000F) << 8)

/**
 * @brief 使能命令在未校验密码的情况下使用
 */
#define     SHELL_CMD_ENABLE_UNCHECKED \
            (1 << 12)

/**
 * @brief 禁用返回值打印
 */
#define     SHELL_CMD_DISABLE_RETURN \
            (1 << 13)

/**
 * @brief 只读属性(仅对变量生效)
 */
#define     SHELL_CMD_READ_ONLY \
            (1 << 14)

/**
 * @brief 命令参数数量
 */
#define     SHELL_CMD_PARAM_NUM(num) \
            ((num & 0x0000000F)) << 16

#if SHELL_USING_CMD_EXPORT == 1

#ifndef SHELL_SECTION
    #if defined(__CC_ARM) || defined(__CLANG_ARM)
        #define SHELL_SECTION(x)                __attribute__((section(x), aligned(1)))
    #elif defined (__IAR_SYSTEMS_ICC__)
        #define SHELL_SECTION(x)                @ x
    #elif defined(__GNUC__)
        #define SHELL_SECTION(x)                __attribute__((section(x), aligned(1)))
    #else
        #define SHELL_SECTION(x)
    #endif
#endif

#else

#ifdef SHELL_SECTION
#error "SHELL_SECTION is used only when using SHELL_USING_CMD_EXPORT"
#endif

#endif

#ifndef SHELL_USED
    #if defined(__CC_ARM) || defined(__CLANG_ARM)
        #define SHELL_USED                      __attribute__((used))
    #elif defined (__IAR_SYSTEMS_ICC__)
        #define SHELL_USED                      __root
    #elif defined(__GNUC__)
        #define SHELL_USED                      __attribute__((used))
    #else
        #define SHELL_USED
    #endif
#endif

typedef struct shell_def *pShell;
typedef struct shell_command *pCommand;
typedef struct shell_user *pUser;

typedef int (*shellCompleteCb)(pShell shell, int argc, char *argv[]);
typedef int (*shellMainStyleCb)(int argc, char *argv[]);

#define SHELL_VA_COUNTING_IMPL(                            \
    _invalid, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,      \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30,      \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40,      \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,      \
    _51, _52, _53, _54, _55, _56, _57, _58, _59, _60,      \
    _61, _62, N, ...) N

/**
 * @brief 返回宏定义的参数数量
 */
#define SHELL_VA_COUNTING(...) SHELL_VA_COUNTING_IMPL(_invalid, ##__VA_ARGS__, 63, 62, 61,    \
                                                      60, 59, 58, 57, 56, 55, 54, 53, 52, 51, \
                                                      50, 49, 48, 47, 46, 45, 44, 43, 42, 41, \
                                                      40, 39, 38, 37, 36, 35, 34, 33, 32, 31, \
                                                      30, 29, 28, 27, 26, 25, 24, 23, 22, 21, \
                                                      20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
                                                      10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define SHELL_CONCAT_2_IMPL(p1, p2) p1##p2

/**
 * @brief 连接token
 */
#define SHELL_CONCAT_2(p1, p2) SHELL_CONCAT_2_IMPL(p1, p2)

/**
 * @brief shell 全功能命令定义
 *
 * @param _attr 命令属性
 * @param _name 命令名
 * @param _func 命令函数
 * @param _desc 命令描述
 * @param ... 其他参数
 */
#define SHELL_EXPORT_COMPLETE_CMD(_attr, _name, _func, _desc, ...) \
    static const char shellCmd##_name[] = #_name;                  \
    static const char shellDesc##_name[] = _desc;                  \
    SHELL_USED const ShellCommand                                  \
        shellCommand##_name SHELL_SECTION("shellCommand") =        \
            {                                                      \
                .attr.value = _attr,                               \
                .data.cmd.name = shellCmd##_name,                  \
                .data.cmd.function = _func,                        \
                .data.cmd.desc = shellDesc##_name,                 \
                ##__VA_ARGS__}

/**
 * @brief 命令的参数解析函数名
 */
#define SHELL_PARSER_FUNC_NAME(_func) SHELL_CONCAT_2(parser_, _func)

/**
 * @brief shell main风格命令定义
 *
 * @param _attr 命令属性
 * @param _name 命令名
 * @param _func 命令函数
 * @param _desc 命令描述
 * @param ... 其他参数
 */
#define SHELL_EXPORT_CMD(_attr, _name, _func, _desc, ...)                   \
    int SHELL_PARSER_FUNC_NAME(_func)(pShell shell, int argc, char *argv[]) \
    {                                                                       \
        (void)shell;                                                        \
        return _func(argc, argv);                                           \
    }                                                                       \
    SHELL_EXPORT_COMPLETE_CMD(_attr, _name, SHELL_PARSER_FUNC_NAME(_func), _desc, ##__VA_ARGS__);

#define SHELL_PARSE_ARG_INIT_ITEM(N, T) \
    SHELL_CONCAT_2(shellArgType_, T)    \
    p##N;                               \
    ret |= SHELL_CONCAT_2(shellArgParse_, T)(&argc, &argv, &p##N)

#define SHELL_PARSER_ARG_INIT_EXPAND_0()
#define SHELL_PARSER_ARG_INIT_EXPAND_1(T0) \
    SHELL_PARSER_ARG_INIT_EXPAND_0();      \
    SHELL_PARSE_ARG_INIT_ITEM(0, T0)
#define SHELL_PARSER_ARG_INIT_EXPAND_2(T0, T1) \
    SHELL_PARSER_ARG_INIT_EXPAND_1(T0);        \
    SHELL_PARSE_ARG_INIT_ITEM(1, T1)
#define SHELL_PARSER_ARG_INIT_EXPAND_3(T0, T1, T2) \
    SHELL_PARSER_ARG_INIT_EXPAND_2(T0, T1);        \
    SHELL_PARSE_ARG_INIT_ITEM(2, T2)
#define SHELL_PARSER_ARG_INIT_EXPAND_4(T0, T1, T2, T3) \
    SHELL_PARSER_ARG_INIT_EXPAND_3(T0, T1, T2);        \
    SHELL_PARSE_ARG_INIT_ITEM(3, T3)
#define SHELL_PARSER_ARG_INIT_EXPAND_5(T0, T1, T2, T3, T4) \
    SHELL_PARSER_ARG_INIT_EXPAND_4(T0, T1, T2, T3);        \
    SHELL_PARSE_ARG_INIT_ITEM(4, T4)
#define SHELL_PARSER_ARG_INIT_EXPAND_6(T0, T1, T2, T3, T4, T5) \
    SHELL_PARSER_ARG_INIT_EXPAND_5(T0, T1, T2, T3);            \
    SHELL_PARSE_ARG_INIT_ITEM(5, T5)
#define SHELL_PARSER_ARG_INIT_EXPAND_7(T0, T1, T2, T3, T4, T5) \
    SHELL_PARSER_ARG_INIT_EXPAND_6(T0, T1, T2, T3);            \
    SHELL_PARSE_ARG_INIT_ITEM(6, T5)
#define SHELL_PARSER_ARG_INIT_EXPAND_8(T0, T1, T2, T3, T4, T5) \
    SHELL_PARSER_ARG_INIT_EXPAND_7(T0, T1, T2, T3);            \
    SHELL_PARSE_ARG_INIT_ITEM(7, T5)

#define SHELL_PARSER_ARG_INIT_EXPAND_X(N, ...) SHELL_CONCAT_2(SHELL_PARSER_ARG_INIT_EXPAND_, N)(__VA_ARGS__)

#define SHELL_PARSER_ARG_INIT_EXPAND(...) SHELL_PARSER_ARG_INIT_EXPAND_X(SHELL_VA_COUNTING(__VA_ARGS__), ##__VA_ARGS__)

#define SHELL_PARSER_ARG_CALL_EXPAND_0
#define SHELL_PARSER_ARG_CALL_EXPAND_1  p0
#define SHELL_PARSER_ARG_CALL_EXPAND_2  p0, p1
#define SHELL_PARSER_ARG_CALL_EXPAND_3  p0, p1, p2
#define SHELL_PARSER_ARG_CALL_EXPAND_4  p0, p1, p2, p3
#define SHELL_PARSER_ARG_CALL_EXPAND_5  p0, p1, p2, p3, p4
#define SHELL_PARSER_ARG_CALL_EXPAND_6  p0, p1, p2, p3, p4, p5
#define SHELL_PARSER_ARG_CALL_EXPAND_7  p0, p1, p2, p3, p4, p5, p6
#define SHELL_PARSER_ARG_CALL_EXPAND_8  p0, p1, p2, p3, p4, p5, p6, p7

#define SHELL_PARSER_ARG_CALL_EXPAND_X(N, ...) SHELL_CONCAT_2(SHELL_PARSER_ARG_CALL_EXPAND_, N)

#define SHELL_PARSER_ARG_CALL_EXPAND(...) SHELL_PARSER_ARG_CALL_EXPAND_X(SHELL_VA_COUNTING(__VA_ARGS__), ##__VA_ARGS__)

#define SHELL_PARSER_RET_ASSIGN_int ret = 
#define SHELL_PARSER_RET_ASSIGN_void ret = 0;
#define SHELL_PARSER_RET_ASSIGN_T(T) SHELL_PARSER_RET_ASSIGN_ ## T

/**
 * @brief shell 带签名命令定义
 *
 * @param _attr 命令属性
 * @param _name 命令名
 * @param _func 命令函数
 * @param _desc 命令描述
 * @param _ret  命令函数返回值类型。仅限int或void。需严格保证名字相同。不允许typedef。
 * @param ...   命令签名， 需为ShellParamType中的选项。需严格保证名字相同。不允许typedef或数值替换。
 */
#define SHELL_EXPORT_CMD_SIGN(_attr, _name, _func, _desc, _ret, ...)        \
    int SHELL_PARSER_FUNC_NAME(_func)(pShell shell, int argc, char *argv[]) \
    {                                                                       \
        int ret = 0;                                                        \
        if (argc < 1)                                                       \
            return -1;                                                      \
        argc--;                                                             \
        argv++;                                                             \
        SHELL_PARSER_ARG_INIT_EXPAND(__VA_ARGS__);                          \
        if (ret)                                                            \
        {                                                                   \
            shellWriteParamErrMessage(shell, ret);                          \
            return ret;                                                     \
        }                                                                   \
        SHELL_PARSER_RET_ASSIGN_T(_ret)                                     \
        _func(SHELL_PARSER_ARG_CALL_EXPAND(__VA_ARGS__));                   \
        return ret;                                                         \
    }                                                                       \
    SHELL_EXPORT_COMPLETE_CMD(_attr, _name, SHELL_PARSER_FUNC_NAME(_func), _desc);

    /**
     * @brief shell 变量定义
     * 
     * @param _attr 变量属性
     * @param _name 变量名
     * @param _value 变量值
     * @param _desc 变量描述
     */
    #define SHELL_EXPORT_VAR(_attr, _name, _value, _desc) \
            const char shellCmd##_name[] = #_name; \
            const char shellDesc##_name[] = _desc; \
            SHELL_USED const ShellCommand \
            shellVar##_name SHELL_SECTION("shellCommand") =  \
            { \
                .attr.value = _attr, \
                .data.var.name = shellCmd##_name, \
                .data.var.value = (void *)_value, \
                .data.var.desc = shellDesc##_name \
            }

    /**
     * @brief shell 用户定义
     * 
     * @param _attr 用户属性
     * @param _name 用户名
     * @param _password 用户密码
     * @param _desc 用户描述
     */
    #define SHELL_EXPORT_USER(_attr, _name, _password, _desc) \
            const char shellCmd##_name[] = #_name; \
            const char shellPassword##_name[] = #_password; \
            const char shellDesc##_name[] = #_desc; \
            SHELL_USED const ShellCommand \
            shellUser##_name SHELL_SECTION("shellCommand") =  \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_USER), \
                .data.user.name = shellCmd##_name, \
                .data.user.password = shellPassword##_name, \
                .data.user.desc = shellDesc##_name \
            }

    /**
     * @brief shell 按键定义
     * 
     * @param _attr 按键属性
     * @param _value 按键键值
     * @param _func 按键函数
     * @param _desc 按键描述
     */
    #define SHELL_EXPORT_KEY(_attr, _value, _func, _desc) \
            const char shellDesc##_value[] = #_desc; \
            SHELL_USED const ShellCommand \
            shellKey##_value SHELL_SECTION("shellCommand") =  \
            { \
                .attr.value = _attr|SHELL_CMD_TYPE(SHELL_TYPE_KEY), \
                .data.key.value = _value, \
                .data.key.function = _func, \
                .data.key.desc = shellDesc##_value \
            }

/**
 * @brief shell 按键代理函数名
 */
#define SHELL_KEY_AGENCY_FUNC_NAME(_func) SHELL_CONCAT_2(key_agency_, _func)

/**
 * @brief shell 代理按键定义
 *
 * @param _attr 按键属性
 * @param _value 按键键值
 * @param _func 按键函数
 * @param _desc 按键描述
 * @param ... 代理参数
 */
#define SHELL_EXPORT_KEY_AGENCY(_attr, _value, _func, _desc, ...) \
    void SHELL_KEY_AGENCY_FUNC_NAME(_func)(Shell * p1)            \
    {                                                             \
        (void)p1;                                                 \
        _func(__VA_ARGS__);                                       \
    }                                                             \
    SHELL_EXPORT_KEY(_attr, _value, SHELL_KEY_AGENCY_FUNC_NAME(_func), _desc)

/**
 * @brief shell command类型
 */
typedef enum
{
    SHELL_TYPE_CMD = 0,                                         /**< main形式命令 */
    SHELL_TYPE_VAR_INT,                                         /**< int型变量 */
    SHELL_TYPE_VAR_SHORT,                                       /**< short型变量 */
    SHELL_TYPE_VAR_CHAR,                                        /**< char型变量 */
    SHELL_TYPE_VAR_STRING,                                      /**< string型变量 */
    SHELL_TYPE_VAR_POINT,                                       /**< 指针型变量 */
    SHELL_TYPE_VAR_NODE,                                        /**< 节点变量 */
    SHELL_TYPE_KEY,                                             /**< 按键 */
} ShellCommandType;


/**
 * @brief Shell定义
 */
typedef struct shell_def
{
    struct
    {
        const struct shell_user *user;                          /**< 当前用户 */
        int activeTime;                                         /**< shell激活时间 */
        char *path;                                             /**< 当前shell路径 */
    #if SHELL_USING_COMPANION == 1
        struct shell_companion_object *companions;              /**< 伴生对象 */
    #endif
    #if SHELL_KEEP_RETURN_VALUE == 1
        int retVal;                                             /**< 返回值 */
    #endif
    } info;
    struct
    {
        unsigned short length;                                  /**< 输入数据长度 */
        unsigned short cursor;                                  /**< 当前光标位置 */
        char *buffer;                                           /**< 输入缓冲 */
        char *param[SHELL_PARAMETER_MAX_NUMBER];                /**< 参数 */
        unsigned short bufferSize;                              /**< 输入缓冲大小 */
        unsigned short paramCount;                              /**< 参数数量 */
        int keyValue;                                           /**< 输入按键键值 */
    } parser;
#if SHELL_HISTORY_MAX_NUMBER > 0
    struct
    {
        char *item[SHELL_HISTORY_MAX_NUMBER];                   /**< 历史记录 */
        unsigned short number;                                  /**< 历史记录数 */
        unsigned short record;                                  /**< 当前记录位置 */
        signed short offset;                                    /**< 当前历史记录偏移 */
    } history;
#endif /** SHELL_HISTORY_MAX_NUMBER > 0 */
    struct
    {
        pCommand base;                                          /**< 命令表基址 */
        unsigned short count;                                   /**< 命令数量 */
    } commandList;
    struct
    {
        pUser base;                                             /**< 用户表基址 */
        unsigned short count;                                   /**< 用户数量 */
    } userList;
    struct
    {
        unsigned char isChecked : 1;                            /**< 密码校验通过 */
        unsigned char isActive : 1;                             /**< 当前活动Shell */
        unsigned char tabFlag : 1;                              /**< tab标志 */
    } status;
    signed short (*read)(char *, unsigned short);               /**< shell读函数 */
    signed short (*write)(char *, unsigned short);              /**< shell写函数 */
#if SHELL_USING_LOCK == 1
    int (*lock)(struct shell_def *);                              /**< shell 加锁 */
    int (*unlock)(struct shell_def *);                            /**< shell 解锁 */
#endif
} Shell;


/**
 * @brief shell command定义
 */
typedef struct shell_command
{
    union
    {
        struct
        {
            unsigned char permission : 8;                       /**< command权限 */
            ShellCommandType type : 4;                          /**< command类型 */
            unsigned char enableUnchecked : 1;                  /**< 在未校验密码的情况下可用 */
            unsigned char disableReturn : 1;                    /**< 禁用返回值输出 */
            unsigned char readOnly : 1;                         /**< 只读 */
            unsigned char reserve : 1;                          /**< 保留 */
            unsigned char paramNum : 4;                         /**< 参数数量 */
        } attrs;
        int value;
    } attr;                                                     /**< 属性 */
    union
    {
        struct
        {
            const char *name;                                   /**< 命令名 */
            const char *desc;                                   /**< 命令描述 */
            shellCompleteCb function;                           /**< 命令执行函数 */
        } cmd;                                                  /**< 命令定义 */
        struct
        {
            const char *name;                                   /**< 变量名 */
            void *value;                                        /**< 变量值 */
            const char *desc;                                   /**< 变量描述 */
        } var;                                                  /**< 变量定义 */
        struct
        {
            int value;                                          /**< 按键键值 */
            void (*function)(Shell *);                          /**< 按键执行函数 */
            const char *desc;                                   /**< 按键描述 */
        } key;                                                  /**< 按键定义 */
    } data;
} ShellCommand;

/**
 * @brief shell user定义
 */
typedef struct shell_user
{
    unsigned char permission;                       /**< command权限 */
    const char *name;                                   /**< 用户名 */
    const char *password;                               /**< 用户密码 */
    const char *desc;                                   /**< 用户描述 */
} ShellUser;
/**
 * @brief shell节点变量属性
 */
typedef struct
{
    void *var;                                                  /**< 变量引用 */
    int (*get)();                                               /**< 变量get方法 */
    int (*set)();                                               /**< 变量set方法 */
} ShellNodeVarAttr;


#define shellSetPath(_shell, _path)     (_shell)->info.path = _path
#define shellGetPath(_shell)            ((_shell)->info.path)

#define shellDeInit(shell)              shellRemove(shell)

void shellInit(Shell *shell, char *buffer, unsigned short size);
void shellRemove(Shell *shell);
unsigned short shellWriteString(Shell *shell, const char *string);
void shellWriteParamErrMessage(Shell *shell, int err);
void shellPrint(Shell *shell, const char *fmt, ...);
void shellScan(Shell *shell, char *fmt, ...);
Shell* shellGetCurrent(void);
void shellHandler(Shell *shell, char data);
void shellWriteEndLine(Shell *shell, char *buffer, int len);
void shellTask(void *param);
int shellRun(Shell *shell, const char *cmd);



#if SHELL_USING_COMPANION == 1
/**
 * @brief shell伴生对象定义
 */
typedef struct shell_companion_object
{
    int id;                                                     /**< 伴生对象ID */
    void *obj;                                                  /**< 伴生对象 */
    struct shell_companion_object *next;                        /**< 下一个伴生对象 */
} ShellCompanionObj;


signed char shellCompanionAdd(Shell *shell, int id, void *object);
signed char shellCompanionDel(Shell *shell, int id);
void *shellCompanionGet(Shell *shell, int id);
#endif

#endif

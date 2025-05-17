cmake_minimum_required(VERSION 3.0.0)
project(LetterShell VERSION 0.1.0 LANGUAGES C)

option(USE_STATIC_LETTER_SHELL_LIBRARY "Build letter-shell static library." ON)
option(USE_SHARED_LETTER_SHELL_LIBRARY "Build letter-shell shared library." OFF)
set(LETTER_SHELL_CFG_USER "" CACHE FILEPATH "letter-shell config file (overrides default).")

if (NOT USE_STATIC_LETTER_SHELL_LIBRARY AND NOT USE_SHARED_LETTER_SHELL_LIBRARY)
    message(FATAL_ERROR "Need to choose static or shared letter-shell build!")
endif()

if(USE_SHARED_LETTER_SHELL_LIBRARY)
    set(SHARED_SWITCH SHARED)
else()
    set(SHARED_SWITCH STATIC)
endif()

add_library(
    letter-shell
    ${SHARED_SWITCH} 
    src/shell.c
    src/shell.h
    src/shell_cfg.h
    src/shell_cmd_list.c
    src/shell_companion.c
    src/shell_ext.c
    src/shell_ext.h
)

if(LETTER_SHELL_CFG_USER)
    target_compile_definitions(letter-shell PUBLIC SHELL_CFG_USER="${LETTER_SHELL_CFG_USER}")
endif()

target_include_directories(letter-shell PUBLIC ./src)

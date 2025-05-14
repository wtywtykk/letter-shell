add_library(
	letter-shell
	src/shell.c
	src/shell.h
	src/shell_cfg.h
	src/shell_cmd_list.c
	src/shell_companion.c
	src/shell_ext.c
	src/shell_ext.h
)

target_include_directories(letter-shell PUBLIC ./src)

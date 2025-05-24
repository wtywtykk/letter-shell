set(CMAKE_C_FLAGS "${CAMKE_C_FLAGS} -Wall -Wextra ")
set(CMAKE_C_FLAGS "${CAMKE_C_FLAGS} -g3 -O0 ")
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -g3 -O0 ")
#-Werror -fsanitize=undefined
set(LETTER_SHELL_CFG_USER "${CMAKE_CURRENT_SOURCE_DIR}/shell_cfg_user.h")
add_subdirectory(../.. ./shell)

add_executable(LetterShell
               main.c
               shell_port.c
               #shell_cpp.cpp
               #../../extensions/fs_support/shell_fs.c
               #../../extensions/log/log.c
               #../../extensions/telnet/telnetd.c
               #../../extensions/shell_enhance/shell_passthrough.c
               #../../extensions/shell_enhance/shell_cmd_group.c
               #../../extensions/shell_enhance/shell_secure_user.c
               #../../extensions/game/game.c
               #../../extensions/game/2048/2048.c
               #../../extensions/game/pushbox/pushbox.c
               )

target_link_libraries(LetterShell PRIVATE letter-shell)

target_include_directories(LetterShell PUBLIC 
                           "${PROJECT_BINARY_DIR}"
                           ./
                           ../../extensions/fs_support
                           ../../extensions/cpp_support
                           ../../extensions/log
                           ../../extensions/shell_enhance
                           ../../extensions/telnet
                           ) 

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
    -T \"${CMAKE_CURRENT_SOURCE_DIR}/shell.lds\" \
    -Wl,-Map=LetterShell.map")
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")

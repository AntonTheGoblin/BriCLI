cmake_minimum_required(VERSION 3.11)

# ---- Project Settings ----
project(Bricli_Example_Auth VERSION 1.0
                        DESCRIPTION "BriCLI Basic Authentication Example"
                        LANGUAGES C
)

set(BRICLI_DIR ${CMAKE_CURRENT_LIST_DIR}/../../Source)
set(INC_DIR ${BRICLI_DIR} ${BRICLI_DIR}/../Config)
set(SOURCES main.c ${BRICLI_DIR}/bricli.c)

add_executable(AuthCli ${SOURCES})
target_include_directories(AuthCli PRIVATE ${INC_DIR})
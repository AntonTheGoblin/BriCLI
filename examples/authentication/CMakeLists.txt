cmake_minimum_required(VERSION 3.11)

# ---- Project Settings ----
project(Bricli_Example_Auth VERSION 1.0
                        DESCRIPTION "BriCLI Basic Authentication Example"
                        LANGUAGES C
)

set(BRICLI_DIR ${CMAKE_CURRENT_LIST_DIR}/../../source)
set(INC_DIR ${BRICLI_DIR}/../include ${BRICLI_DIR}/../config)
set(SOURCES main.c ${BRICLI_DIR}/bricli.c)

add_executable(auth-cli ${SOURCES})
target_include_directories(auth-cli PRIVATE ${INC_DIR})
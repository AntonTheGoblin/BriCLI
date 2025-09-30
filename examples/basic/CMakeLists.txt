cmake_minimum_required(VERSION 3.11)

# ---- Project Settings ----
project(Bricli_Example VERSION 1.0
                        DESCRIPTION "BriCLI Generic Example."
                        LANGUAGES C
)

set(BRICLI_DIR ${CMAKE_CURRENT_LIST_DIR}/../../source)
set(INC_DIR ${BRICLI_DIR} ${BRICLI_DIR}/../config)
set(SOURCES main.c ${BRICLI_DIR}/bricli.c)

add_executable(SimpleCli ${SOURCES})
target_include_directories(SimpleCli PRIVATE ${INC_DIR})
/**
 * @file    colours.h
 * @brief   VT100 colour information
 * @date    25/03/2025
 * @version 1
 * @author  Anthony Wall
 *
 * Copyright (C) 2025 Anthony Wall.
 * All rights reserved.
 *
 **/

#ifndef BRICLI_COLOURS_H
#define BRICLI_COLOURS_H

//Regular text
#define BRICLI_TEXT_BLACK      "\e[0;30m"
#define BRICLI_TEXT_RED        "\e[0;31m"
#define BRICLI_TEXT_GREEN      "\e[0;32m"
#define BRICLI_TEXT_YELLOW     "\e[0;33m"
#define BRICLI_TEXT_BLUE       "\e[0;34m"
#define BRICLI_TEXT_MAGENTA    "\e[0;35m"
#define BRICLI_TEXT_CYAN       "\e[0;36m"
#define BRICLI_TEXT_WHITE      "\e[0;37m"

//Regular bold text
#define BRICLI_BOLD_BLACK      "\e[1;30m"
#define BRICLI_BOLD_RED        "\e[1;31m"
#define BRICLI_BOLD_GREEN      "\e[1;32m"
#define BRICLI_BOLD_YELLOW     "\e[1;33m"
#define BRICLI_BOLD_BLUE       "\e[1;34m"
#define BRICLI_BOLD_MAGENTA    "\e[1;35m"
#define BRICLI_BOLD_CYAN       "\e[1;36m"
#define BRICLI_BOLD_WHITE      "\e[1;37m"

//Regular underline text
#define BRICLI_UL_BLACK        "\e[4;30m"
#define BRICLI_UL_RED          "\e[4;31m"
#define BRICLI_UL_GREEN        "\e[4;32m"
#define BRICLI_UL_YELLOW       "\e[4;33m"
#define BRICLI_UL_BLUE         "\e[4;34m"
#define BRICLI_UL_MAGENTA      "\e[4;35m"
#define BRICLI_UL_CYAN         "\e[4;36m"
#define BRICLI_UL_WHITE        "\e[4;37m"

//Regular background
#define BRICLI_BKGND_BLACK     "\e[40m"
#define BRICLI_BKGND_RED       "\e[41m"
#define BRICLI_BKGND_GREEN     "\e[42m"
#define BRICLI_BKGND_YELLOW    "\e[43m"
#define BRICLI_BKGND_BLUE      "\e[44m"
#define BRICLI_BKGND_MAGENTA   "\e[45m"
#define BRICLI_BKGND_CYAN      "\e[46m"
#define BRICLI_BKGND_WHITE     "\e[47m"

#define BRICLI_COLOUR_RESET    "\e[0m"

// //High intensity background
// #define BRICLI_INTENSE_BKGND_BLKHB "\e[0;100m"
// #define REDHB "\e[0;101m"
// #define GRNHB "\e[0;102m"
// #define YELHB "\e[0;103m"
// #define BLUHB "\e[0;104m"
// #define MAGHB "\e[0;105m"
// #define CYNHB "\e[0;106m"
// #define WHTHB "\e[0;107m"

// //High intensty text
// #define HBLK "\e[0;90m"
// #define HRED "\e[0;91m"
// #define HGRN "\e[0;92m"
// #define HYEL "\e[0;93m"
// #define HBLU "\e[0;94m"
// #define HMAG "\e[0;95m"
// #define HCYN "\e[0;96m"
// #define HWHT "\e[0;97m"

// //Bold high intensity text
// #define BHBLK "\e[1;90m"
// #define BHRED "\e[1;91m"
// #define BHGRN "\e[1;92m"
// #define BHYEL "\e[1;93m"
// #define BHBLU "\e[1;94m"
// #define BHMAG "\e[1;95m"
// #define BHCYN "\e[1;96m"
// #define BHWHT "\e[1;97m"




#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// C functions and constants

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BRICLI_COLOURS_H

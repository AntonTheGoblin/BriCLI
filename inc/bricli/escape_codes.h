/**
 * @file    escape_codes.h
 * @brief   VT100 escape codes
 * @date    25/03/2025
 * @version 1
 * @author  Anthony Wall
 *
 * Copyright (C) 2025 Anthony Wall.
 * All rights reserved.
 *
 **/

#ifndef BRICLI_ESCAPE_CODES_H
#define BRICLI_ESCAPE_CODES_H

#define BRICLI_DELETE_CHAR     "\e[K"
#define BRICLI_CLEAR           "\e[H\e[J"

#define BRICLI_ARROW_LEN       2
#define BRICLI_UP_ARROW        "[A"
#define BRICLI_DOWN_ARROW      "[B"
#define BRICLI_RIGHT_ARROW     "[C"
#define BRICLI_LEFT_ARROW      "[D"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// C functions and constants

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BRICLI_ESCAPE_CODES_H

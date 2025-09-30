/**
 * @file    bricli.h
 * @brief   Header for the BriCLI library.
 * @author  Anthony Wall
 *
 * Copyright (C) 2025 Anthony Wall.
 * All rights reserved.
 *
 **/

#ifndef __BRICLI_H__
#define __BRICLI_H__

/* INCLUDES */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "bricli_config.h"

#include "auth.h"
#include "colours.h"
#include "command.h"
#include "error.h"
#include "escape_codes.h"
#include "handle.h"
#include "parser.h"
#include "writer.h"

#ifdef __cplusplus
extern "C" {
#endif

// ===================
// ===== Defines =====
// ===================

// Helper macro for calculating the size of statically typed array. This does NOT work on array parameters in C.
#define BRICLI_STATIC_ARRAY_SIZE(array) ( sizeof(array) / sizeof(array[0]) )

#ifndef BRICLI_SHOW_HELP_ON_ERROR
#define BRICLI_SHOW_HELP_ON_ERROR 1 // Set to 1 to have BriCLI print the help message on an unkown command.
#endif // BRICLI_SHOW_HELP_ON_ERROR

#ifndef BRICLI_SHOW_COMMAND_ERRORS
#define BRICLI_SHOW_COMMAND_ERRORS 1 // Set to 1 to have BriCLI automatically report command handler error codes.
#endif // BRICLI_SHOW_COMMAND_ERRORS

#ifndef BRICLI_USE_REENTRANT
#define BRICLI_USE_REENTRANT 1 // Set to 1 to have BriCLI use re-entrant safe strtok_r instead of strtok.
#endif // BRICLI_USE_REENTRANT

#ifndef BRICLI_USE_COLOUR
#define BRICLI_USE_COLOUR 1 // Set to 1 to allow the use of VT100 colour options.
#endif // BRICLI_USE_COLOUR

#ifndef BRICLI_MAX_COMMAND_LEN
#define BRICLI_MAX_COMMAND_LEN 10 // Sets the maximum command name length.
#endif // BRICLI_MAX_COMMAND_LEN

#ifndef BRICLI_ARGUMENT_BUFFER_LEN
#define BRICLI_ARGUMENT_BUFFER_LEN 70 // The maximum number of bytes available for argument storage.
#endif // BRICLI_ARGUMENT_BUFFER_LEN

#ifndef BRICLI_MAX_ARGUMENTS
#define BRICLI_MAX_ARGUMENTS 3 // Sets the maximum number of arguments that BriCLI can find.
#endif // BRICLI_MAX_ARGUMENTS

//#ifndef BRICLI_RX_BUFFER_LEN
//#define BRICLI_RX_BUFFER_LEN 80 // Sets the character
//#endif // BRICLI_RX_BUFFER_LEN

#ifndef BRICLI_PRINT_MESSAGE_SIZE
#define BRICLI_PRINT_MESSAGE_SIZE 80 // Sets the maximum length of a custom PrintF message.
#endif // BRICLI_PRINT_MESSAGE_SIZE

// VT100 colour options.
#if BRICLI_USE_COLOUR
#ifndef BRICLI_USE_TEXT_COLOURS
#define BRICLI_USE_TEXT_COLOURS    1 // Enables the use of text colour options.
#endif // BRICLI_USE_TEXT_COLOURS

#ifndef BRICLI_USE_BOLD
#define BRICLI_USE_BOLD            1 // Enables the use of bold text options.
#endif // BRICLI_USE_BOLD

#ifndef BRICLI_USE_UNDERLINE
#define BRICLI_USE_UNDERLINE       1 // Enables the use of underline text options.
#endif // BRICLI_USE_UNDERLINE

#ifndef BRICLI_USE_BACKGROUNDS
#define BRICLI_USE_BACKGROUNDS     1 // Enables the use of background colour options.
#endif // BRICLI_USE_BACKGROUNDS

#ifndef BRICLI_USE_INTENSE_BACKGROUNDS
// #define BRICLI_USE_INTENSE_BACKGROUNDS 1 // Enables the use of high intensity background colour options.
#endif // BRICLI_USE_INTENSE_BACKGROUNDS

#ifndef BRICLI_USE_INTENSE_TEXT
// #define BRICLI_USE_INTENSE_TEXT     1 // Enables the use of high intensity text colour options.
#endif // BRICLI_USE_INTENSE_TEXT

#ifndef BRICLI_USE_INTENSE_BOLD
// #define BRICLI_USE_INTENSE_BOLD     1 // Enables the use of high intensity bold text options.
#endif // BRICLI_USE_INTENSE_BOLD

#ifndef BRICLI_DEFAULT_EOL
#define BRICLI_DEFAULT_EOL				"\n" // The default EoL string to be used when none is provided to init
#endif // BRICLI_DEFAULT_EOL

#ifndef BRICLI_DEFAULT_PROMPT
#define BRICLI_DEFAULT_PROMPT			">> " // The default prompt string to be used when non is provided to init
#endif // BRICLI_DEFAULT_PROMPT

#endif // BRICLI_USE_COLOUR

// =================
// ===== Types =====
// =================

// ==============================
// ===== Exported Functions =====
// ==============================

#ifdef __cplusplus
}
#endif

#endif /* __BRICLI_H__ */

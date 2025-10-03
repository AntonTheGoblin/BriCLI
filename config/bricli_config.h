/**
 * @file    bricli.h
 * @brief   Header for the BriCLI library.
 * @author  Anthony Wall
 *
 * Copyright (C) 2025 Anthony Wall.
 * All rights reserved.
 *
 **/

#ifndef BRICLI_CONFIG_H
#define BRICLI_CONFIG_H

// ===========================
// ===== System Settings =====
// ===========================

// The length of the internal arguments buffer, default 70
#define BRICLI_ARGUMENT_BUFFER_LEN 70

// The maximum number of arguments BriCLI can parse, default 3
#define BRICLI_MAX_ARGUMENTS 3

// The maximum length any user command can be, default 10
#define BRICLI_MAX_COMMAND_LEN 10

// The maximum length a PrintF message can be, default 80
#define BRICLI_PRINT_MESSAGE_SIZE 80

// When on, BriCLI will use the thread safe strtok_r in place of strtok, default off
#define BRICLI_USE_REENTRANT 1

// ============================
// ===== Default Settings =====
// ============================

// The default EoL string to be used when none is provided to init
#define BRICLI_DEFAULT_EOL				"\n"

// The default prompt string to be used when non is provided to init
#define BRICLI_DEFAULT_PROMPT			">> "

// ============================
// ===== Display Settings =====
// ============================

// When on, BriCLI will automatically report command handler errors to the user, default on
#define BRICLI_SHOW_COMMAND_ERRORS 1

// When on, BriCLI will automatically show the help message when an unknown command is received, default on
#define BRICLI_SHOW_HELP_ON_ERROR 1

// ==============================
// ===== Logging Settings =======
// ==============================

/**
 * @brief Logging function for displaying internal behaviour
 */
#define BRICLI_LOG(...) printf(__VA_ARGS__)

// ===========================
// ===== Colour Settings =====
// ===========================

// When on, enables the use of VT100 colour commands, default on
#define BRICLI_USE_COLOUR 1

// Enables the use of VT100 background colours, default on
#define BRICLI_USE_BACKGROUNDS 1

// Enables the use of VT100 bold text colours, default on
#define BRICLI_USE_BOLD 1

// Enables the use of VT100 text colours, default on
#define BRICLI_USE_TEXT_COLOURS 1

// Enables the use of VT100 underline colours, default on
#define BRICLI_USE_UNDERLINE 1

#endif // BRICLI_CONFIG_H

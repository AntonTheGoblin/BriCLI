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

#define BRICLI_DELETE_CHAR     "\e[K"
#define BRICLI_CLEAR           "\e[H\e[J"

#define BRICLI_ARROW_LEN       2
#define BRICLI_UP_ARROW        "[A"
#define BRICLI_DOWN_ARROW      "[B"
#define BRICLI_RIGHT_ARROW     "[C"
#define BRICLI_LEFT_ARROW      "[D"

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

// =================
// ===== Types =====
// =================

typedef enum _BricliLastError_t
{
    BricliErrorNone,
    BricliErrorInternal,
    BricliErrorCommand
} BricliLastError_t;

typedef enum _BricliErrors_t
{
    BricliUnknown            = -8,
    BricliUnauthorized		 = -7,
	BricliReceivedNull       = -6,
    BricliCopyWouldOverflow  = -5,
    BricliBadCommand         = -4,
    BricliBadParameter       = -3,
    BricliBadHandle          = -2,
    BricliBadFunction        = -1,
    BricliOk                 = 0
} BricliErrors_t;

typedef enum _BricliAuthErrors_t
{
    BricliAuthDenied = -1,       // Request for auth denied
    BricliAuthGranted = 0,      // Request for auth elevation granted
    BricliAuthRevoked = 1       // Request for auth decrease granted
} _BricliAuthErrors_t;

/**
 * @brief Enumerated VT100 colour options
 */
typedef enum _BricliColours_t
{
    BricliTextBlack,
    BricliTextRed,
    BricliTextGreen,
    BricliTextYellow,
    BricliTextBlue,
    BricliTextMagenta,
    BricliTextCyan,
    BricliTextWhite,

    BricliTextBoldBlack,
    BricliTextBoldRed,
    BricliTextBoldGreen,
    BricliTextBoldYellow,
    BricliTextBoldBlue,
    BricliTextBoldMagenta,
    BricliTextBoldCyan,
    BricliTextBoldWhite,

    BricliUnderlineBlack,
    BricliUnderlineRed,
    BricliUnderlineGreen,
    BricliUnderlineYellow,
    BricliUnderlineBlue,
    BricliUnderlineMagenta,
    BricliUnderlineCyan,
    BricliUnderlineWhite,

    BricliBackgroundBlack,
    BricliBackgroundRed,
    BricliBackgroundGreen,
    BricliBackgroundYellow,
    BricliBackgroundBlue,
    BricliBackgroundMagenta,
    BricliBackgroundCyan,
    BricliBackgroundWhite,

    BricliColourReset
} BricliColours_t;

/**
 * @brief States that BriCLI can be in during execution.
 */
typedef enum _BricliStates_t
{
    BricliStateIdle,            // BriCLI is idle and receiving characters.
    BricliStateParsing,         // BriCLI is parsing a buffer looking a valid handler to run.
    BricliStateHandlerRunning,  // BriCLI is executing a command handler.
    BricliStateFinished
} BricliStates_t;

/**
 * @brief CLI Authorization scopes, represented as up to 32 bit flags
 */
typedef uint32_t BricliAuthScopes_t;

// Unauthorized scope, available to anyone without login
#define BricliScopeAll                  (0)

// User scope
#define BricliScopeUser                 (1 << 1)

// Admin scope
#define BricliScopeAdmin                (1 << 2)

typedef struct _BricliAuthEntry_t
{
	const char *Username;
	const char *Password;
	BricliAuthScopes_t Scopes;
} BricliAuthEntry_t;

/**
 * @brief BSP function for writing data.
 *
 * This should be a wrapper function for your underlying peripheral. This
 * may be a UART, USB or any other communication stack you have.
 *
 * @param length    The number of characters in \c data.
 * @param data      The data to be written.
 */
typedef int (*Bricli_BspWrite)(uint32_t length, const char* data);

/**
 * @brief Command handler function, one should be provided for every command.
 *
 * @param numberOfArgs The number of arguments found.
 * @param args         Array of argument string found.
 */
typedef int (*Bricli_CommandHandler)(uint32_t numberOfArgs, char* args[]);

/**
 * @brief StateChanged event callback. Used to notify an application of internal state changes.
 *
 * @param oldState The state of BriCLI before the change.
 * @param newState The state BriCLI changed to.
 */
typedef void (*Bricli_StateChanged)(BricliStates_t oldState, BricliStates_t newState);

/**
 * @brief Holds specific details for a command entry used by this CLI.
 *
 * @param Name          The command name.
 * @param Handler       Handler function for this command.
 * @param HelpMessage   Optional message to display with the built-in help command.
 */
typedef struct _BricliCommand_t
{
    const char*             Name;                   /*<< Command name. */
    Bricli_CommandHandler   Handler;                /*<< Handler function for this command. */
    const char*             HelpMessage;            /*<< Optional message to be displayed by the help command. */
    BricliAuthScopes_t      AuthScopesRequired;     /*<< The authorisation scopes required to access this command. */
} BricliCommand_t;

/**
 * @brief Initialistion options for a BriCLI instance
 */
typedef struct BricliInit_t
{
	const BricliAuthEntry_t *AuthList;
	char *Eol;
    char *SendEol;
    char *Prompt;
    Bricli_BspWrite BspWrite;
    char *RxBuffer;
    uint32_t RxBufferSize;
    BricliCommand_t* CommandList;
    uint32_t CommandListLength;
    Bricli_StateChanged OnStateChanged;
    bool LocalEcho;
} BricliInit_t;

/**
 * @brief Initializer for BriCLI to set the commands, bsp functions and EOL.
 *
 * @param LastError       Determines whether the last error came from BriCLI or a command.
 * @param CommandList     The list of CLI commands.
 * @param BspWrite        BSP function for writing out data.
 * @param Eol             The End of Line character BriCLI should look for.
 */
typedef struct _BricliHandle_t
{
	const BricliAuthEntry_t *AuthList;
	BricliAuthScopes_t      AuthScopes;
    Bricli_BspWrite        	BspWrite;
    BricliCommand_t*       	CommandList;
    uint32_t                CommandListLength;
    char*                   Eol;
    bool                    IsHandlingEscape;
    BricliLastError_t      	LastError;
    bool                    LocalEcho;
    uint32_t                PendingBytes;
    char*                   Prompt;
    char*                   RxBuffer;
    uint32_t                RxBufferSize;
    char *                  SendEol;
    BricliStates_t         	State;
    Bricli_StateChanged    	OnStateChanged;
} BricliHandle_t;

// ==============================
// ===== Exported Functions =====
// ==============================

extern BricliErrors_t Bricli_Init(BricliHandle_t *cli, const BricliInit_t *settings);
extern int Bricli_ParseCommand(BricliHandle_t* cli);
extern int Bricli_Parse(BricliHandle_t* cli);
extern BricliErrors_t Bricli_ReceiveCharacter(BricliHandle_t* cli, char rxChar);
extern BricliErrors_t Bricli_ReceiveIndexedArray(BricliHandle_t *cli, uint32_t index, uint32_t length, char *array);
extern bool Bricli_CheckForEol(BricliHandle_t* cli, bool replaceEol);
extern void Bricli_Backspace(BricliHandle_t* cli);
extern size_t Bricli_SplitOnEol(BricliHandle_t *cli);
extern int Bricli_PrintHelp(BricliHandle_t* cli);
extern int Bricli_PrintF(BricliHandle_t* cli, const char* format, ...);
extern void Bricli_SetColour(BricliHandle_t* cli, BricliColours_t colourId);
extern void Bricli_Reset(BricliHandle_t *cli);
extern void Bricli_ClearCommand(BricliHandle_t *cli);

/**
 * @brief Helper macro for calling Bricli_PrintF with colour support.
 *
 * @param cli Pointer to the CLI instance to use.
 * @param colour The colour to be used for data.
 * @param format Format string to be used for message generation.
 * @param va_args An additional variadic argument list to be used with Bricli_PrintF
 *
 * @return The error code from the instance's BSP write.
 */
#define BRICLI_PRINTF_COLOURED(cli, colour, format, ...) ({ Bricli_SetColour(cli, colour); int macroResult = Bricli_PrintF(cli, format, __VA_ARGS__); Bricli_SetColour(cli, BricliColourReset); macroResult; })

/**
* @brief Helper function for writing out data on the CLI's write function.
*
* @param cli Pointer to the CLI instance to use.
* @param length The number of characters in the buffer to be sent.
* @param data Pointer to the buffer to be sent.
*
* @return An error code, negative indicates a problem occurred.
*/
static inline int Bricli_Write(BricliHandle_t* cli, uint32_t length, const char* data)
{
    // Make sure we actually have a write function.
    if (cli != NULL && cli->BspWrite != NULL)
    {
        return cli->BspWrite(length, data);
    }
    else
    {
        cli->LastError = BricliErrorInternal;
        return BricliBadHandle;
    }
}

/**
 * @brief Helper function for writing out data with automatic EOL appending.
 * 
 * @param cli Pointer to the CLI instance to use.
 * @param length The number of characters in the buffer to be sent.
 * @param data Pointer to the buffer to be sent.
 * 
 * @return An error code, negative indicates a problem occurred. 
 */
static inline int Bricli_WriteLine(BricliHandle_t* cli, uint32_t length, const char* data)
{
    Bricli_Write(cli, length, data);

    if (cli->SendEol == NULL)
    {
        return Bricli_Write(cli, strlen(cli->Eol), cli->Eol);
    }
    else
    {
        return Bricli_Write(cli, strlen(cli->SendEol), cli->SendEol);
    }
    
}

/**
* @brief Helper function for writing out coloured data on the CLI's write function.
*
* @param cli    Pointer to the CLI instance to use.
* @param length The number of characters in the buffer to be sent.
* @param data   Pointer to the buffer to be sent.
* @param colour The colour to be used.
*
* @return An error code, negative indicates a problem occurred.
*/
static inline int Bricli_WriteColoured(BricliHandle_t* cli, uint32_t length, const char* data, BricliColours_t colour)
{
    Bricli_SetColour(cli, colour);
    int writeColouredResult = Bricli_Write(cli, length, data);
    Bricli_SetColour(cli, BricliColourReset);
    return writeColouredResult;
}

/**
* @brief Helper function for writing out coloured data with automatic EOL appending..
*
* @param cli    Pointer to the CLI instance to use.
* @param length The number of characters in the buffer to be sent.
* @param data   Pointer to the buffer to be sent.
* @param colour The colour to be used.
*
* @return An error code, negative indicates a problem occurred.
*/
static inline int Bricli_WriteColouredLine(BricliHandle_t* cli, uint32_t length, const char* data, BricliColours_t colour)
{
    Bricli_SetColour(cli, colour);
    int writeColouredResult = Bricli_WriteLine(cli, length, data);
    Bricli_SetColour(cli, BricliColourReset);
    return writeColouredResult;
}

/**
* @brief Helper function for writing out string data on the CLI's write function.
*
* @param cli Pointer to the CLI instance to use.
* @param length The number of characters in the buffer to be sent.
* @param data Pointer to the buffer to be sent.
*
* @return An error code, negative indicates a problem occurred.
*/
static inline int Bricli_WriteString(BricliHandle_t* cli, const char* data)
{
    return Bricli_Write(cli, strlen(data), data);
}

/**
 * @brief Helper function for writing out string data with automatic EOL appending.
 * 
 * @param cli Pointer to the CLI instance to use.
 * @param data Pointer to the buffer to be sent.
 * 
 * @return An error code, negative indicates a problem occurred. 
 */
static inline int Bricli_WriteStringLine(BricliHandle_t* cli, const char* data)
{
    return Bricli_WriteLine(cli, strlen(data), data);
}

/**
* @brief Helper function for writing out coloured string data on the CLI's write function.
*
* @param cli Pointer to the CLI instance to use.
* @param length The number of characters in the buffer to be sent.
* @param data Pointer to the buffer to be sent.
*
* @return An error code, negative indicates a problem occurred.
*/
static inline int Bricli_WriteStringColoured(BricliHandle_t* cli, const char* data, BricliColours_t colour)
{
    Bricli_SetColour(cli, colour);
    int result = Bricli_Write(cli, strlen(data), data);
    Bricli_SetColour(cli, BricliColourReset);
    return result;
}

/**
* @brief Helper function for writing out coloured string data with automatic EOL appending.
*
* @param cli Pointer to the CLI instance to use.
* @param length The number of characters in the buffer to be sent.
* @param data Pointer to the buffer to be sent.
*
* @return An error code, negative indicates a problem occurred.
*/
static inline int Bricli_WriteStringColouredLine(BricliHandle_t* cli, const char* data, BricliColours_t colour)
{
    Bricli_SetColour(cli, colour);
    int result = Bricli_WriteStringLine(cli, data);
    Bricli_SetColour(cli, BricliColourReset);
    return result;
}

/**
 * @brief Resets the RX Buffer.
 * @param cli Pointer to the BriCLI instance to use.
 */
static inline void Bricli_ClearBuffer(BricliHandle_t* cli)
{
    memset(cli->RxBuffer, 0, cli->RxBufferSize);
    cli->PendingBytes = 0;
}

/**
 * @brief Sends a Prompt string to the terminal.
 *
 * @param cli Pointer to the CLI instance to use.
 */
static inline void Bricli_SendPrompt(BricliHandle_t* cli)
{
    if (cli->Prompt != NULL)
    {
        Bricli_WriteString(cli, cli->Prompt);
    }
}

/**
 * @brief Sends a VT100 clear screen command (cursorhome + cleareos) to
 *      the remote terminal.
 *
 * @param cli Pointer to the CLI instance to use.
 */
static inline void Bricli_ClearScreen(BricliHandle_t* cli)
{
    Bricli_WriteString(cli, BRICLI_CLEAR);
}

/**
* @brief Helper function for receiving an array from the 0th index.
*
* @param cli Pointer to the CLI instance to use.
* @param length The number of characters in the array to be received.
* @param data Pointer to the array to be received.
*
* @return An error code, negative indicates a problem occurred.
*/
static inline BricliErrors_t Bricli_ReceiveArray(BricliHandle_t *cli, uint32_t length, char *array)
{
    return Bricli_ReceiveIndexedArray(cli, 0, length, array);
}

#ifdef __cplusplus
}
#endif

#endif /* __BRICLI_H__ */

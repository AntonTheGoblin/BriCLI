/**
 * @file    types.h
 * @brief   Type information for the Bricli library
 * @date    25/03/2025
 * @version 1
 * @author  Anthony Wall
 *
 * Copyright (C) 2025 Anthony Wall.
 * All rights reserved.
 *
 **/

#ifndef BRICLI_TYPES_H
#define BRICLI_TYPES_H

#include <stdint.h>
#include <bricli/options.h>

/**
 * @brief BSP function for writing data.
 *
 * This should be a wrapper function for your underlying peripheral. This
 * may be a UART, USB or any other communication stack you have.
 *
 * @param length    The number of characters in \c data.
 * @param data      The data to be written.
 * @return uint32_t The number of bytes successfully written
 */
typedef uint32_t (*BricliBspWrite)(uint32_t length, const uint8_t* data);

/**
 * @brief Command handler function, one should be provided for every command.
 *
 * @param numberOfArgs The number of arguments found.
 * @param args         Array of argument string found.
 */
typedef int (*BricliCommandHandler)(uint32_t numberOfArgs, uint8_t* args[]);

/**
 * Contains the overall error type last received by the CLI
 */
typedef enum _BricliLastError_t
{
    BricliErrorNone,
    BricliErrorInternal,
    BricliErrorCommand
} BricliLastError_t;

/**
 * Contains error types for use with the BriCLI library
 */
typedef enum _BricliErrors_t
{
    BricliErrorOk                 = 0,
    BricliErrorUnknown            = -1,
    BricliErrorBadFunction        = -2,
    BricliErrorBadHandle          = -3,
    BricliErrorBadParameter       = -4,
    BricliErrorBadCommand         = -5,
    BricliErrorCopyWouldOverflow  = -6,
    BricliErrorReceivedNull       = -7,
    BricliErrorNullArgument       = -8
} BricliError_t;

/**
 * Holds specific details for a command entry used by this CLI.
 */
typedef struct _BricliCommand_t
{
    const char*             Name;           // Command name
    BricliCommandHandler    Handler;        // Handler function for this command
    const char*             HelpMessage;    // Optional message to be displayed by the help command
} BricliCommand_t;

/**
 * Structure containing all initialisation values for the parser
 */
typedef struct _BricliInit_t
{
    const uint8_t *RxEol;
    const uint8_t *TxEol;
    BricliBspWrite BspWrite;
} BricliInit_t;

/**
 * Object representing a parser instance
 */
typedef struct _Bricli_t
{
    uint8_t                 Buffer[BRICLI_BUFFER_SIZE];     // The internal memory buffer
    uint8_t                 RxEol[BRICLI_EOL_SIZE];         // The current End-of-Line character for this parser
    const uint8_t*          TxEol;                          // End-of-Line character to be used with WriteLine functions
    uint32_t                PendingBytes;                   // The number of bytes waiting in the buffer
    BricliBspWrite          BspWrite;                       // User defined write function
    BricliLastError_t       LastError;
    // SnapCliCommand_t*       CommandList;
    // uint32_t                CommandListLength;
    // SnapCli_BspWrite        BspWrite;
    // char*                   Prompt;
    // bool                    IsHandlingEscape;
    // SnapCliStates_t         State;
    // SnapCli_StateChanged    OnStateChanged;
    // bool                    LocalEcho;

} Bricli_t;

#endif /* BRICLI_TYPES_H */
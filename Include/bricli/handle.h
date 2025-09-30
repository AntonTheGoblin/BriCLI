#ifndef BRICLI_HANDLE_H
#define BRICLI_HANDLE_H

#include <stdint.h>
#include <stdbool.h>
#include "command.h"
#include "error.h"

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
 * @brief StateChanged event callback. Used to notify an application of internal state changes.
 *
 * @param oldState The state of BriCLI before the change.
 * @param newState The state BriCLI changed to.
 */
typedef void (*Bricli_StateChanged)(BricliStates_t oldState, BricliStates_t newState);

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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern BricliErrors_t Bricli_Init(BricliHandle_t *cli, const BricliInit_t *settings);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BRICLI_HANDLE_H

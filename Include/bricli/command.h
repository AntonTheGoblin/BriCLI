#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>
#include "auth.h"
#include "handle.h"

/**
 * @brief Command handler function, one should be provided for every command.
 *
 * @param numberOfArgs The number of arguments found.
 * @param args         Array of argument string found.
 */
typedef int (*Bricli_CommandHandler)(uint32_t numberOfArgs, char* args[]);

/**
 * @brief Holds specific details for a command entry used by this CLI.
 *
 * @param                       The command name.
 * @param Handler               Handler function for this command.
 * @param HelpMessage           Optional message to display with the built-in help command.
 * @param AuthScopesRequired    The authorization scopes needed to run this command, 0 enables access to all users
 */
typedef struct _BricliCommand_t
{
    const char*             Name;                   /*<< Command name. */
    Bricli_CommandHandler   Handler;                /*<< Handler function for this command. */
    const char*             HelpMessage;            /*<< Optional message to be displayed by the help command. */
    BricliAuthScopes_t      AuthScopesRequired;     /*<< The authorisation scopes required to access this command. */
} BricliCommand_t;

/**
 * @brief Terminator for marking the end of a BriCLI command list
 */
#define BRICLI_COMMAND_LIST_TERMINATOR {NULL, NULL, NULL, 0}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern void Bricli_ClearCommand(BricliHandle_t *cli);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // COMMAND_H

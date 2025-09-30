/**
 * @file    bricli.c
 * @brief   BriCLI source, provides a lightweight CLI implementation.
 *          Supports VT100 ANSI escape codes.
 * @author  Anthony Wall
 *
 * Copyright (C) 2025 Anthony Wall.
 * All rights reserved.
 *
 **/

// ====================
// ===== Includes =====
// ====================

// Fix for "strtok_r" not found on default C99 GNU builds.
#ifdef __GNUC__
    #define _GNU_SOURCE
#endif // __GNUC__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <bricli/bricli.h>

// =====================
// ===== Constants =====
// =====================

#if BRICLI_USE_COLOUR
static const char *_colourReset = "\e[0m";

#if BRICLI_USE_TEXT_COLOURS
static const char *_colourTable[] =
{
    "\e[0;30m", // Black
    "\e[0;31m", // Red
    "\e[0;32m", // Green
    "\e[0;33m", // Yellow
    "\e[0;34m", // Blue
    "\e[0;35m", // Magenta
    "\e[0;36m", // Cyan
    "\e[0;37m"  // White
};
#endif // BRICLI_USE_TEXT_COLOURS

#if BRICLI_USE_BOLD
static const char *_boldTable[] =
{
    "\e[1;30m", // Bold Black
    "\e[1;31m", // Bold Red
    "\e[1;32m", // Bold Green
    "\e[1;33m", // Bold Yellow
    "\e[1;34m", // Bold Blue
    "\e[1;35m", // Bold Magenta
    "\e[1;36m", // Bold Cyan
    "\e[1;37m"  // Bold White
};
#endif // BRICLI_USE_BOLD

#if BRICLI_USE_UNDERLINE
static const char *_underlineTable[] =
{
    "\e[4;30m", // Underline Black
    "\e[4;31m", // Underline Red
    "\e[4;32m", // Underline Green
    "\e[4;33m", // Underline Yellow
    "\e[4;34m", // Underline Blue
    "\e[4;35m", // Underline Magenta
    "\e[4;36m", // Underline Cyan
    "\e[4;37m"  // Underline White
};
#endif // BRICLI_USE_UNDERLINE

#if BRICLI_USE_BACKGROUNDS
static const char *_backgroundTable[] =
{
    "\e[40m", // Black Background
    "\e[41m", // Red Background
    "\e[42m", // Green Background
    "\e[43m", // Yellow Background
    "\e[44m", // Blue Background
    "\e[45m", // Magenta Background
    "\e[46m", // Cyan Background
    "\e[47m"  // White Background
};
#endif // BRICLI_USE_BACKGROUNDS
#endif // BRICLI_USE_COLOUR

// ===========================
// ===== Local Functions =====
// ===========================

/**
 * @brief Extracts arguments from a given argument string. Arguments must be separated by spaces.
 *
 * @param arguments     Pointer to the argument string to look for arguments in.
 * @param output        Pointer to a char* array to store each of the found arguments in.
 *
 * @return The number of arguments found.
 */
static uint32_t Bricli_ExtractArguments(char *arguments, char *output[])
{
    char seperator[] = " ";
    uint8_t argumentsFound = 0;
    
#if BRICLI_USE_REENTRANT
    char *savePointer = NULL;
#endif // BRICLI_USE_REENTRANT

    // Make sure we actually have something to work with.
    if (arguments == NULL || output == NULL)
    {
        return 0;
    }

    // Split the arguments.
    #if BRICLI_USE_REENTRANT
        char *token = (char *)strtok_r(arguments, seperator, &savePointer);
    #else
        char *token = (char *)strtok(arguments, seperator);
    #endif // BRICLI_USE_REENTRANT
    while (token != NULL && argumentsFound < BRICLI_MAX_ARGUMENTS)
    {

        // This is actually a string argument, need to skip to the next quote mark.
        if (*token == '\"')
        {
            // Skip past the speech mark.
            token++;

            // If we aren't useing re-entrancy we need to calculate the next token point ahead of time.
            #if !BRICLI_USE_REENTRANT
                char* savePointer = token + strlen(token) + 1;
            #endif // !BRICLI_USE_REENTRANT

            // If strtok_r nulled out a space we need to re-add it.
            if (savePointer != NULL && *(savePointer - 1) == '\0')
            {
                *(savePointer - 1) = ' ';
            }

            // Look for the closing quote mark.
            char *quoteToken = strchr(savePointer, '\"');
            if (quoteToken == NULL)
            {
                // User didn't close out their speech mark so just bail out.
                return 0;
            }

            // Replace the closing mark with a null character and step forward the save pointer.
            *quoteToken = '\0';
            savePointer = quoteToken + 1;
        }

        // Store this token and increment the arguments counter.
        output[argumentsFound] = token;
        argumentsFound++;

        // Get the next token.
        #if BRICLI_USE_REENTRANT
            token = (char *)strtok_r(NULL, seperator, &savePointer);
        #else
            token = (char *)strtok(NULL, seperator);
        #endif // BRICLI_USE_REENTRANT
    }

    // Return how many arguments we were able to find.
    return argumentsFound;
}

/**
 * @brief Update the state of a given BriCLI handle, calling the event handler if set.
 * 
 * @param cli Pointer to the BriCLI instance to use.
 * @param newState The new state to set on the BriCLI instance.
 */
static inline void Bricli_ChangeState(BricliHandle_t* cli, BricliStates_t newState)
{
    BricliStates_t currentState = cli->State;

    // Update our state and call the OnStateChanged event if set.
    cli->State = newState;
    if (cli->OnStateChanged != NULL)
    {
        cli->OnStateChanged(currentState, newState);
    }
}

/**
 * @brief Validates whether the given command is available under the current auth scopes 
 * 
 * @param cli Pointer to the BriCLI instance to use
 * @param command Pointer to a command to verify
 * @return bool True if the command is within auth scope, false otherwise
 */
static bool Bricli_IsCommandInScope(BricliHandle_t* cli, BricliCommand_t *command)
{	
	if (command->AuthScopesRequired == BricliScopeAll)
	{
		// Command is available to all scopes
		return true;
	}
	else if ((cli->AuthScopes & command->AuthScopesRequired) != 0)
	{
		// Command is available under granted scopes
		return true;
	}
	else
	{
		// Command is not available
		return false;
	}
}

// ===========================
// ===== System Commands =====
// ===========================

/**
 * @brief Command handler function, one should be provided for every command.
 *
 * @param numberOfArgs The number of arguments found.
 * @param args         Array of argument string found.
 */
typedef int (*Bricli_SystemCommandHandler)(BricliHandle_t *cli, uint32_t numberOfArgs, char* args[]);

/**
 * @brief Internal command handler
 */
typedef struct _BricliSystemCommand_t
{
    const char*             Name;                   /*<< Command name. */
    Bricli_SystemCommandHandler   Handler;                /*<< Handler function for this command. */
    const char*             HelpMessage;            /*<< Optional message to be displayed by the help command. */
} BricliSystemCommand_t;

static int Bricli_SystemHandlerHelp(BricliHandle_t *cli, uint32_t numberOfArgs, char *args[]);
static int Bricli_SystemHandlerClear(BricliHandle_t *cli, uint32_t numberOfArgs, char *args[]);
static int Bricli_SystemHandlerLogin(BricliHandle_t *cli, uint32_t numberOfArgs, char *args[]);
static int Bricli_SystemHandlerLogout(BricliHandle_t *cli, uint32_t numberOfArgs, char *args[]);

/**
 * @brief Internal system command list
 */
static const BricliSystemCommand_t _systemCommands[] =
{
	{"help", Bricli_SystemHandlerHelp, "Displays this help message"},
	{"clear", Bricli_SystemHandlerClear, "Clears the terminal"},
	{"login", Bricli_SystemHandlerLogin, "Login to the terminal"},
	{"logout", Bricli_SystemHandlerLogout, "Logout from the terminal"},
	{NULL, NULL, NULL}
};

/**
 * @brief System command for displaying a help message
 * 
 * @param cli Pointer to the CLI instance
 * @param numberOfArgs The number of arguments received
 * @param args The arguments list
 * @return int 0 for success, negative for error
 */
static int Bricli_SystemHandlerHelp(BricliHandle_t *cli, uint32_t numberOfArgs, char *args[])
{
	// Unused parameters
	(void)numberOfArgs;
	(void)args;

	// Print the system commands first.
	const BricliSystemCommand_t *systemCommand = &_systemCommands[0];
	while(systemCommand->Name != NULL)
	{
		if (cli->SendEol == NULL)
		{
			Bricli_PrintF(cli, "%s - %s%s", systemCommand->Name, systemCommand->HelpMessage, cli->Eol);
		}
		else
		{
			Bricli_PrintF(cli, "%s - %s%s", systemCommand->Name, systemCommand->HelpMessage, cli->SendEol);
		}

		// Increment the pointer
		systemCommand++;
	}
	
	// Print all registered user commands.
	BricliCommand_t *command = &cli->CommandList[0];
	while(command->Name != NULL)
    {
		// Only display help if the command is available to the current auth scope
		if (Bricli_IsCommandInScope(cli, command))
		{
			// Send the help message for each command.
			if (command->HelpMessage == NULL)
			{
				if (cli->SendEol == NULL)
				{
					Bricli_PrintF(cli, "%s%s", command->Name, cli->Eol);
				}
				else
				{
					Bricli_PrintF(cli, "%s%s", command->Name, cli->SendEol);
				}
			}
			else
			{
				if (cli->SendEol == NULL)
				{
					Bricli_PrintF(cli, "%s - %s%s", command->Name, command->HelpMessage, cli->Eol);
				}
				else
				{
					Bricli_PrintF(cli, "%s - %s%s", command->Name, command->HelpMessage, cli->SendEol);
				}
			}
		}

		// Increment the pointer
		command++;
    }

	return BricliOk;
}

/**
 * @brief System command for clearing the buffer
 * 
 * @param cli Pointer to the CLI instance
 * @param numberOfArgs The number of arguments received
 * @param args The arguments list
 * @return int 0 for success, negative for error
 */
static int Bricli_SystemHandlerClear(BricliHandle_t *cli, uint32_t numberOfArgs, char *args[])
{
	// Unused parameters
	(void)numberOfArgs;
	(void)args;

	Bricli_ClearScreen(cli);
	return BricliOk;
}

/**
 * @brief System command for logging in as an authenticated user
 * 
 * @param cli Pointer to the CLI instance
 * @param numberOfArgs The number of arguments received
 * @param args The arguments list
 * @return int 0 for success, negative for error
 */
static int Bricli_SystemHandlerLogin(BricliHandle_t *cli, uint32_t numberOfArgs, char *args[])
{
	int result = -1;

	if (numberOfArgs < 2)
	{
		Bricli_WriteStringColouredLine(cli, "ERROR: login requires 2 arguments!", BricliTextRed);
		goto cleanup;
	}

	if (NULL == cli->AuthList)
	{
		Bricli_WriteStringColouredLine(cli, "ERROR: No authentication provider registered", BricliTextRed);
		goto cleanup;
	}

	// Helper pointers for our arguments
	const char *user = args[0];
	const char *pass = args[1];

	// Search the auth provider for a matching user
	const BricliAuthEntry_t *authEntry = &cli->AuthList[0];
	while (NULL != authEntry->Username)
	{
		// Search for a matching username
		if (strcmp(user, authEntry->Username) == 0)
		{
			// Search for a matching password
			if (strcmp(pass, authEntry->Password) == 0)
			{
				// Update our active scopes
				cli->AuthScopes = authEntry->Scopes;

				// Success
				result = 0;
				if (cli->SendEol == NULL)
				{
					Bricli_PrintF(cli, "Logged in as %s%s", authEntry->Username, cli->Eol);
				}
				else
				{
					Bricli_PrintF(cli, "Logged in as %s%s", authEntry->Username, cli->SendEol);
				}

				goto cleanup;
			}
			else
			{
				// Password mismatch
				result = -2;
				goto cleanup;
			}
		}
		
		// Increment the pointer
		authEntry++;
	}

	// If we get here then we didn't find a matching user
	result = -2;

cleanup:
	// If we have an error report back
	if (result == -2)
		Bricli_WriteStringColouredLine(cli, "ERROR: Invalid username or password", BricliTextRed);

	return result;
}

/**
 * @brief System command for logging out of the current authenticated user
 * 
 * @param cli Pointer to the CLI instance
 * @param numberOfArgs The number of arguments received
 * @param args The arguments list
 * @return int 0 for success, negative for error
 */
static int Bricli_SystemHandlerLogout(BricliHandle_t *cli, uint32_t numberOfArgs, char *args[])
{
	int result = 0;

	if (cli->AuthScopes != BricliScopeAll)
	{
		// Reset auth scopes
		cli->AuthScopes = BricliScopeAll;
		Bricli_WriteString(cli, "Logged out of session\n");
	}
	else
	{
		// Not logged in, so can't log out
		Bricli_WriteString(cli, "No active login session found\n");
	}

	return result;
}

// ==============================
// ===== Exported Functions =====
// ==============================

BricliErrors_t Bricli_Init(BricliHandle_t *cli, const BricliInit_t* settings)
{
	BricliErrors_t result = BricliUnknown;

	// Validate pointers
	if (NULL == cli || NULL == settings)
	{
		BRICLI_LOG("BriCLI: NULL pointer in %s\n", __func__);
		result = BricliBadParameter;
		goto cleanup;
	}

	// Validate buffer settings
	if (NULL == settings->RxBuffer || 0 == settings->RxBufferSize)
	{
		BRICLI_LOG("BriCLI: Invalid RX buffer in %s\n", __func__);
		result = BricliBadParameter;
		goto cleanup;
	}

	// Validate command list settings
	if (NULL == settings->CommandList || NULL == settings->CommandList[0].Name)
	{
		BRICLI_LOG("BriCLI: Invalid Command List in %s\n", __func__);
		result = BricliBadParameter;
		goto cleanup;
	}

	// Validate BSP Write function
	if (NULL == settings->BspWrite)
	{
		BRICLI_LOG("BriCLI: Invalid BspWrite in %s\n", __func__);
		result = BricliBadParameter;
		goto cleanup;
	}
	
	// Initialise the CLI to zero
	memset(cli, 0, sizeof(BricliHandle_t));

	// EOL string
	if (NULL != settings->Eol)
		cli->Eol = settings->Eol;
	else
		cli->Eol = BRICLI_DEFAULT_EOL;
	
	// Prompt string
	if (NULL != settings->Prompt)
		cli->Prompt = settings->Prompt;
	else
		cli->Prompt = BRICLI_DEFAULT_PROMPT;

	// RX Buffer settings
	cli->RxBuffer = settings->RxBuffer;
	cli->RxBufferSize = settings->RxBufferSize;

	// Command List settings
	cli->CommandList = settings->CommandList;

	// BSP settings
	cli->BspWrite = settings->BspWrite;
	
	// Event settings
	cli->OnStateChanged = settings->OnStateChanged;

	// Flag settings
	cli->LocalEcho = settings->LocalEcho;

	// Auth list
	if (NULL != settings->AuthList)
		cli->AuthList = settings->AuthList;

	// Success
	result = BricliOk;

cleanup:
	return result;
}

#if BRICLI_USE_COLOUR
/**
 * @brief Sets the various colour options of a VT100 terminal.
 *
 * @param colourId The enum ID of the colour option to be written.
 */
void Bricli_SetColour(BricliHandle_t *cli, BricliColours_t colourId)
{
    char *colourMessage = NULL;

    // Reset the VT100 terminal colour settings.
    if (colourId == BricliColourReset)
    {
        colourMessage = (char *)_colourReset;
    }
#if BRICLI_USE_TEXT_COLOURS
    // Text colours.
    else if (colourId <= BricliTextWhite)
    {
        colourMessage = (char *)_colourTable[colourId];
    }
#endif // BRICLI_USE_TEXT_COLOURSs
#if BRICLI_USE_BOLD
    // Bold colours
    else if (colourId <= BricliTextBoldWhite)
    {
        colourMessage = (char *)_boldTable[colourId - BricliTextWhite];
    }
#endif // BRICLI_USE_BOLD
#if BRICLI_USE_UNDERLINE
    // Underline colours.
    else if (colourId <= BricliUnderlineWhite)
    {
        colourMessage = (char *)_underlineTable[colourId - BricliTextBoldWhite];
    }
#endif // BRICLI_USE_UNDERLINE
#if BRICLI_USE_BACKGROUNDS
    // Background colours.
    else if (colourId <= BricliBackgroundWhite)
    {
        colourMessage = (char *)_backgroundTable[colourId - BricliUnderlineWhite];
    }
#endif // BRICLI_USE_BACKGROUNDS

    // Send the colour message if we have one.
    if (colourMessage != NULL)
    {
        Bricli_WriteString(cli, colourMessage);
    }
}
#endif // BRICLI_USE_COLOUR

int Bricli_ParseEscapeCode(BricliHandle_t *cli)
{
    //  // Check if this is an up arrow or not.
    //  if (cli->PendingBytes >= BRICLI_ARROW_LEN && (strncmp(cli->RxBuffer, BRICLI_UP_ARROW, cli->BRICLI_ARROW_LEN) ==0) )
    //    {
    //      // Up Arrow pressed.
    //    }
    (void)&cli;
    return BricliOk;
}

/**
 * @brief Removes a command from the receive buffer, moving any remaining commands to the front.
 * 
 * @param cli Pointer to a BriCLI instance.
 */
void Bricli_ClearCommand(BricliHandle_t *cli)
{
    size_t nextCommand = 0;
    
    // If there is another command it will always be EOL length past our old command.
    nextCommand = strlen(cli->RxBuffer) + strlen(cli->Eol);

    // If the next command is out of bounds or more than we have just clear the whole buffer.
    if (nextCommand >= cli->RxBufferSize || nextCommand >= cli->PendingBytes)
    {
        Bricli_ClearBuffer(cli);
    }
    else
    {
        // Remove the number of bytes we have handled.
        cli->PendingBytes -= nextCommand;
        
        // Shift next command to start of buffer
        memmove(cli->RxBuffer, &cli->RxBuffer[nextCommand], cli->PendingBytes);
        memset(&cli->RxBuffer[cli->PendingBytes], 0, nextCommand);
    }
}

/**
* @brief Default runner for performing common BriCLI functionality.
*
* @param cli Pointer to the CLI instance to use.
* @return The error code from Bricli_ParseCommand.
*/
int Bricli_Parse(BricliHandle_t *cli)
{
    size_t numberOfCommands;
    int result = BricliOk;

    // First do a non-invasive check for an EOL delimeter.
    if (!Bricli_CheckForEol(cli, false))
    {
        goto cleanup;
    }

    // Edge case: Eol has been sent on it's own
    // giving us a zero-length command.
    if (cli->PendingBytes == strlen(cli->Eol))
    {
        Bricli_SendPrompt(cli);
        Bricli_ClearBuffer(cli);
        goto cleanup;
    }

    // Look for an EOL, repeating for as long as we have commands in the buffer.
    numberOfCommands = Bricli_SplitOnEol(cli);
    while(numberOfCommands > 0)
    {
        // Handle the command.
        result = Bricli_ParseCommand(cli);

        // Remove the command we just handled
        Bricli_ClearCommand(cli);

        // Reset our internal state.
        Bricli_ChangeState(cli, BricliStateIdle);

        // Track that we have handled this command.
        numberOfCommands--;

        // If we just handled the last command send the CLI prompt.
        if (numberOfCommands == 0)
        {
            Bricli_SendPrompt(cli);
        }
    }

cleanup:
    return result;
}


/**
 * @brief Parses the given command against the provided CLI instance.
 *
 * @param cli Pointer to a BriCLI instance.
 * @param data The full command string received including any arguments.
 *
 * @return Pass through return from the given command handler.
 */
int Bricli_ParseCommand(BricliHandle_t *cli)
{
    char command[BRICLI_MAX_COMMAND_LEN + 1] = {0};
    char arguments[BRICLI_ARGUMENT_BUFFER_LEN] = {0};
	char *ArgumentsFound[BRICLI_MAX_ARGUMENTS] = {0};
    uint32_t commandLength = 0;
    uint32_t argumentLength = 0;

    // Error check our arguments.
    if (cli->RxBuffer == NULL)
    {
        cli->LastError = BricliErrorInternal;
        return BricliBadParameter;
    }
    else if (cli == NULL || cli->CommandList == NULL)
    {
        cli->LastError = BricliErrorInternal;
        return BricliBadHandle;
    }

    // Update our state.
    Bricli_ChangeState(cli, BricliStateParsing);

    // If this is actually an escape sequence handle it separately.
    if (cli->RxBuffer[0] == '\e')
    {
        return Bricli_ParseEscapeCode(cli);
    }

    // Look for arguments.
    char *argData = strchr(cli->RxBuffer, ' ');

    // Split the command and arguments if needed.
    if (argData != NULL)
    {
        // Calculate length of command and skip the first space in argData.
        commandLength = argData - cli->RxBuffer;
        argData++;

        // Copy the arguments into our local buffer, ensuring we don't overflow.
        argumentLength = strlen(argData);
        if (argumentLength > BRICLI_ARGUMENT_BUFFER_LEN)
        {
            argumentLength = BRICLI_ARGUMENT_BUFFER_LEN;
        }
        memcpy(arguments, argData, argumentLength);
    }
    else
    {
        commandLength = strlen(cli->RxBuffer);
    }

    // Limit the command length to prevent overflow.
    if (commandLength > BRICLI_MAX_COMMAND_LEN)
    {
        commandLength = BRICLI_MAX_COMMAND_LEN;
    }
    memcpy(command, (void *)cli->RxBuffer, commandLength);

	// Extract additional arguments.
	uint8_t numberOfArguments = Bricli_ExtractArguments(arguments, ArgumentsFound);

    // Check if this is a system command.
	const BricliSystemCommand_t *systemCommand = &_systemCommands[0];
	while (systemCommand->Name != NULL)
	{
		if (strcmp(command, systemCommand->Name) == 0)
		{
			// Call the command's handler function.
            Bricli_ChangeState(cli, BricliStateHandlerRunning);
			systemCommand->Handler(cli, numberOfArguments, ArgumentsFound);
            Bricli_ChangeState(cli, BricliStateFinished);

			return BricliOk;
		}

		// Increment the pointer
		systemCommand++;
	}

	// Not a system command so look to our command list for a match.
	BricliCommand_t *cliCommand = &cli->CommandList[0];
	while(cliCommand->Name != NULL)
    {

        // Check if we have found a match.
        if (strcmp(command, cliCommand->Name) == 0)
        {
			int result = BricliUnknown;

			// Check our authentication scopes
			if (Bricli_IsCommandInScope(cli, cliCommand))
			{
				// Call the command's handler function.
				Bricli_ChangeState(cli, BricliStateHandlerRunning);
				result = cliCommand->Handler(numberOfArguments, ArgumentsFound);
				Bricli_ChangeState(cli, BricliStateFinished);
	
				// Check the result code.
				if (result < 0)
				{
					// If enabled, display the error code to the user.
				#if BRICLI_SHOW_COMMAND_ERRORS
					if (cli->SendEol == NULL)
					{
						BRICLI_PRINTF_COLOURED(cli, BricliTextRed, "Command returned error: %d%s", result, cli->Eol);
					}
					else
					{
						BRICLI_PRINTF_COLOURED(cli, BricliTextRed, "Command returned error: %d%s", result, cli->SendEol);
					}
				#endif // BRICLI_SHOW_COMMAND_ERRORS
	
					cli->LastError = BricliErrorCommand;
				}
			}
			else
			{
				cli->LastError = BricliErrorInternal;
				result = BricliUnauthorized;

				if (cli->SendEol == NULL)
				{
					Bricli_PrintF(cli, "Unknown Command %s%s", command, cli->Eol);
				}
				else
				{
					Bricli_PrintF(cli, "Unknown Command %s%s", command, cli->SendEol);
				}
			}
			
            return result;
        }

		// Increment the pointer
		cliCommand++;
    }

    // If we get here then we failed to find a valid command in the list.
    if (cli->SendEol == NULL)
    {
        Bricli_PrintF(cli, "Unknown Command %s%s", command, cli->Eol);
    }
    else
    {
        Bricli_PrintF(cli, "Unknown Command %s%s", command, cli->SendEol);
    }

    // If enabled, print help on an unknown command.
#if BRICLI_SHOW_HELP_ON_ERROR
    Bricli_PrintHelp(cli);
#endif // BRICLI_SHOW_HELP_ON_ERROR

    // Return that this is an unknown command.
    cli->LastError = BricliErrorInternal;
    return BricliBadCommand;
}

/**
* @brief Checks a given buffer for an occurrence of the EoL string.
*
* @param cli        Pointer to the CLI instance to use.
* @param data       Pointer to the buffer to search.
* @param replaceEol True to replace the EOL with '\0', false to leave it alone.
*
* @return True when an EOL is found, false otherwise.
*/
bool Bricli_CheckForEol(BricliHandle_t *cli, bool replaceEol)
{
    char *eol = NULL;
    bool result = false;

    // Make sure our parameters are valid.
    if (cli == NULL || cli->Eol == NULL || cli->RxBuffer == NULL || cli->PendingBytes == 0)
    {
        goto cleanup;
    }

    // Look for the EOL substring in our data.
    eol = strstr(cli->RxBuffer, cli->Eol);
    if (eol == NULL)
    {
        // No EOL found.
        result = false;
    }
    else
    {
        // EOL found.
        result = true;

        // Inject null characters if needed.
        if (replaceEol)
        {
            memset(eol, '\0', strlen(cli->Eol));
        }
    }

cleanup:
    return result;
}

/**
* @brief For the given buffer, searches for all EOL strings and replaces them with null characters.
*
* @param cli Pointer to the CLI instance to use.
*
* @return The number of commands found in the buffer.
*/
size_t Bricli_SplitOnEol(BricliHandle_t *cli)
{
    uint16_t numberOfCommands = 0;
    char *token = NULL;
    #if BRICLI_USE_REENTRANT
        char *lastToken = NULL;
    #endif // BRICLI_USE_REENTRANT

    // Make sure our parameters are valid.
    if (cli == NULL || cli->Eol == NULL || cli->RxBuffer == NULL || cli->PendingBytes == 0)
    {
        goto cleanup;
    }

    // Iterate over the buffer looking for EOLs.
    #if BRICLI_USE_REENTRANT
        token = (char *)strtok_r(cli->RxBuffer, cli->Eol, &lastToken);
    #else
        token = (char *)strtok(cli->RxBuffer, cli->Eol);
    #endif // BRICLI_USE_REENTRANT
    
    while (token != NULL)
    {
        // Increment the number of commands and get the next token.
        numberOfCommands++;
        #if BRICLI_USE_REENTRANT
            token = (char *)strtok_r(NULL, cli->Eol, &lastToken);
        #else
            token = (char *)strtok(NULL, cli->Eol);
        #endif // BRICLI_USE_REENTRANT
    }

    // Edge case: If no delimiters were present the length of the buffer will match pending bytes exactly.
    // In this case an EOL was not found and the numberOfCommands is actually off by one.
    if (numberOfCommands == 1 && strlen(cli->RxBuffer) == cli->PendingBytes)
    {
        numberOfCommands--;
    }


cleanup:
    // Return how many commands we found.
    return numberOfCommands;

}

/**
 * @brief Places a character into the RX buffer and performs various character checks.
 *
 * @param cli Pointer to the CLI instance to use.
 * @param rxChar The character received.
 *
 * @return BricliCopyWouldOverflow if the RX buffer is full, BricliOk otherwise.
 */
BricliErrors_t Bricli_ReceiveCharacter(BricliHandle_t *cli, char rxChar)
{
    BricliErrors_t result = BricliUnknown;

    // Check for a null terminating character.
    if (rxChar == '\0')
    {
        result = BricliReceivedNull;
        goto cleanup;
    }

    // Check for an overflow.
    if (cli->PendingBytes >= cli->RxBufferSize)
    {
        result = BricliCopyWouldOverflow;
        goto cleanup;
    }

    // Store the received character.
    cli->RxBuffer[cli->PendingBytes] = rxChar;
    cli->PendingBytes++;

    // If we are currently in the process of handling an escape code.
    if (cli->IsHandlingEscape)
    {
        Bricli_ParseEscapeCode(cli);
    }

    // If we've received a backspace then we need to remove it and the previous character.
    if (rxChar == '\b')
    {
        Bricli_Backspace(cli);
        result = BricliOk;
    }

    // Echo the received character.
    if (cli->LocalEcho)
    {
        Bricli_Write(cli, 1, &rxChar);
    }

    // Success.
    result = BricliOk;

cleanup:
    return result;
}

/**
 * @brief Iterates through an array of received characters, parsing each in turn.
 *
 * @param cli       Pointer to the CLI instance to use.
 * @param index     The starting character in array to receive from.
 * @param length    The number of bytes to be received from the array.
 * @param arrray    Pointer to the array to be received.
 *
 * @return BricliCopyWouldOverflow if the RX buffer is full, BricliOk otherwise.
 */
BricliErrors_t Bricli_ReceiveIndexedArray(BricliHandle_t *cli, uint32_t index, uint32_t length, char *array)
{
    BricliErrors_t error = BricliUnknown;

    // Receive each character in turn.
    for (uint32_t i = index; i < (index + length); i++)
    {
        error = Bricli_ReceiveCharacter(cli, array[i]);

        // If something went wrong, exit immediately.
        if (error != BricliOk)
        {
            return error;
        }
    }
    return error;
}

/**
  * @brief Updates the pending bytes count and transmits a VT100 backspace response.
  * @param cli Pointer to the CLI instance to use.
  */
void Bricli_Backspace(BricliHandle_t *cli)
{   
    
    // If there is less than 2 bytes in the buffer there is nothing to be deleted.
    if (cli->PendingBytes < 2)
    {
        cli->PendingBytes = 0;
    }
    // If there is more than 2 bytes we must tell the VT100 terminal to delete a character.
    else
    {
        // Move the pending bytes value back by two characters and ensure the backsapce is nulled.
        cli->PendingBytes -= 2;
        cli->RxBuffer[cli->PendingBytes + 1] = '\0';

        // Send the backspace and the VT100 delete.
        Bricli_Write(cli, 1, "\b");
        Bricli_Write(cli, 3, BRICLI_DELETE_CHAR);
    }
}

/**
  * @brief Helper function for displaying help information.
  *
  * @param cli Pointer to a BriCLI instance.
  * @param format Format string to be used for message generation.
  *
  * @return The error code from the instance's BSP write.
  */
int Bricli_PrintHelp(BricliHandle_t *cli)
{
	return Bricli_SystemHandlerHelp(cli, 0, NULL);
}

/**
 * @brief Helper function for sending formatted messages through a CLI instance.
 *
 * @param cli Pointer to a BriCLI instance.
 * @param format Format string to be used for message generation.
 *
 * @return The error code from the instance's BSP write.
 */
int Bricli_PrintF(BricliHandle_t *cli, const char *format, ...)
{
    char message[BRICLI_PRINT_MESSAGE_SIZE] = {0};
    int bytesWritten = 0;
    int result = -1;

    // Format the message.
    va_list args;
    va_start(args, format);
    bytesWritten = vsnprintf(message, BRICLI_PRINT_MESSAGE_SIZE, format, args);
    va_end(args);

    // Send the formatted message if it was fully written.
    if (bytesWritten > 0 && bytesWritten < BRICLI_PRINT_MESSAGE_SIZE)
    {
        result = Bricli_Write(cli, bytesWritten, message);
    }

    return result;
}

/** @brief Helper function that clears the internal buffer and resets the CLI state.
 * 
 * @param cli Pointer to a BriCLI instance.
 */
void Bricli_Reset(BricliHandle_t *cli)
{
    Bricli_ClearBuffer(cli);
    Bricli_ChangeState(cli, BricliStateIdle);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bricli.h"

#define RX_BUFFER_SIZE 80
static char _rxBuffer[RX_BUFFER_SIZE];

static BricliHandle_t _cli;
static bool _exitRequested = false;

/**
 * @brief Ping command handler, returns Pong!
 * 
 * @param numberOfArgs The number of arguments received
 * @param args The string array of arguments.
 * @return A BriCLI Error code, 0 for success.
 */
static int Ping_Handler(uint32_t numberOfArgs, char *args[])
{
    return Bricli_WriteString(&_cli, "Pong!\n");
}

/**
 * @brief Exit command handler, quits the application!
 * 
 * @param numberOfArgs The number of arguments received
 * @param args The string array of arguments.
 * @return Always returns 0 for success.
 */
static int Exit_Handler(uint32_t numberOfArgs, char *args[])
{
    _exitRequested = true;
    return 0;
}

static BricliCommand_t _commandList[] =
{
    { "admin", Ping_Handler, BricliAuthAdmin },
    { "exit", Exit_Handler, "Exits the application" }
};

static int CustomWrite(uint32_t length, const char* data)
{
    return printf("%s", data);
}

int main(int argc, char const *argv[])
{
	// Setup the CLI
	BricliInit_t init = {0};
    init.BspWrite = CustomWrite;
    init.RxBuffer = _rxBuffer;
    init.RxBufferSize = RX_BUFFER_SIZE;
    init.CommandList = _commandList;
	init.CommandListLength = BRICLI_STATIC_ARRAY_SIZE(_commandList);
	Bricli_Init(&_cli, &init);

    // Send the initial prompt.
    Bricli_SendPrompt(&_cli);

    // Loop until the user requests an exit.
    while (!_exitRequested)
    {
        char rxChar = getchar();

        // Store the character.
        Bricli_ReceiveCharacter(&_cli, rxChar);

        // Check if we have found an EOL.
        Bricli_Parse(&_cli);
    }
    return 0;
}
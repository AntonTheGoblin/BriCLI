#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bricli.h"

#define RX_BUFFER_SIZE  80 // Size of the internal RX Buffer.

static BricliHandle_t _cli;
static char _rxBuffer[RX_BUFFER_SIZE];
static bool _exitRequested = false;

static int Ping_Handler(uint32_t numberOfArgs, char *args[]);
static int Add_Handler(uint32_t numberOfArgs, char *args[]);
static int Echo_Handler(uint32_t numberOfArgs, char *args[]);
static int Exit_Handler(uint32_t numberOfArgs, char *args[]);

static BricliCommand_t _commandList[] =
{
    { "ping", Ping_Handler, "Responds with Pong."                                },
    { "add",  Add_Handler,  "Adds two numbers together and prints the response." },
    { "echo", Echo_Handler, "Echoes what is sent"                                },
    { "exit", Exit_Handler, "Exits the application"                              }

};

static int CustomWrite(uint32_t length, const char* data)
{
    return printf("%s", data);
}

int main(int argc, char const *argv[])
{
    // Setup the CLI
    _cli.Eol = "\n";
    _cli.BspWrite = CustomWrite;
    _cli.RxBuffer = _rxBuffer;
    _cli.RxBufferSize = RX_BUFFER_SIZE;
    _cli.CommandList = _commandList;
    _cli.CommandListLength = BRICLI_STATIC_ARRAY_SIZE(_commandList);
    _cli.Prompt = ">> ";

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
 * @brief Add command handler, returns the sum of 2 numbers!
 * 
 * @param numberOfArgs The number of arguments received
 * @param args The string array of arguments.
 * @return A BriCLI Error code, 0 for success.
 */
static int Add_Handler(uint32_t numberOfArgs, char *args[])
{
    if (numberOfArgs < 2)
    {
        Bricli_WriteString(&_cli, "Must provide 2 arguments!\n");
        return -1;
    }

    // Convert the arguments to numbers.
    int a = strtol(args[0], NULL, 10);
    int b = strtol(args[1], NULL, 10);

    int result = a + b;
    return Bricli_PrintF(&_cli, "%d + %d = %d\n", a, b, result);
}

/**
 * @brief Echo command handler, returns whatever was sent!
 * 
 * @param numberOfArgs The number of arguments received
 * @param args The string array of arguments.
 * @return A BriCLI Error code, 0 for success.
 */
static int Echo_Handler(uint32_t numberOfArgs, char *args[])
{
    if (numberOfArgs < 1)
    {
        Bricli_WriteString(&_cli, "Must provide 1 argument!\n");
        return -1;
    }

    return Bricli_PrintF(&_cli, "You sent: %s\n", args[0]);
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

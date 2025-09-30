#ifndef BRICLI_WRITER_H
#define BRICLI_WRITER_H

#include "colours.h"
#include "escape_codes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern void Bricli_Backspace(BricliHandle_t* cli);
extern int Bricli_PrintF(BricliHandle_t* cli, const char* format, ...);
extern int Bricli_PrintHelp(BricliHandle_t* cli);

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

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BRICLI_WRITER_H

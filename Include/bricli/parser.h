#ifndef BRICLI_PARSER_H
#define BRICLI_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include "handle.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern bool Bricli_CheckForEol(BricliHandle_t* cli, bool replaceEol);
extern int Bricli_Parse(BricliHandle_t* cli);
extern int Bricli_ParseCommand(BricliHandle_t* cli);
extern BricliErrors_t Bricli_ReceiveCharacter(BricliHandle_t* cli, char rxChar);
extern BricliErrors_t Bricli_ReceiveIndexedArray(BricliHandle_t *cli, uint32_t index, uint32_t length, char *array);
extern void Bricli_Reset(BricliHandle_t *cli);
extern size_t Bricli_SplitOnEol(BricliHandle_t *cli);

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
#endif // __cplusplus
#endif // BRICLI_PARSER_H

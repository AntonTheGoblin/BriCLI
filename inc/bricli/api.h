/**
 * @file    api.h
 * @brief   Public API for the Bricli library
 * @date    25/03/2025
 * @version 1
 * @author  Anthony Wall
 *
 * Copyright (C) 2025 Anthony Wall.
 * All rights reserved.
 *
 **/

#ifndef BRICLI_API_H
#define BRICLI_API_H

#include <stddef.h>
#include <string.h>
#include <bricli/types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	BricliError_t Bricli_Init(Bricli_t *cli, BricliInit_t *init);
	BricliError_t Bricli_Parse(uint8_t *buffer, uint32_t length);


/**
* @brief Helper function for writing out data on the CLI's write function.
*
* @param cli Pointer to the CLI instance to use.
* @param length The number of characters in the buffer to be sent.
* @param data Pointer to the buffer to be sent.
*
* @return An error code, negative indicates a problem occurred.
*/
static inline int BricliWrite(Bricli_t* cli, uint32_t length, const uint8_t* data)
{
    // Make sure we actually have a write function.
    if (cli != NULL && cli->BspWrite != NULL)
    {
        return cli->BspWrite(length, data);
    }
    else
    {
        cli->LastError = BricliErrorInternal;
        return BricliErrorBadHandle;
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
static inline int BricliWriteLine(Bricli_t* cli, uint32_t length, const uint8_t* data)
{
    BricliWrite(cli, length, data);

    if (cli->TxEol == NULL)
    {
        return BricliWrite(cli, strlen((const char*)cli->RxEol), cli->RxEol);
    }
    else
    {
        return BricliWrite(cli, strlen((const char*)cli->TxEol), cli->TxEol);
    }
    
}


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BRICLI_API_H


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
 * Object representing a parser instance
 */
typedef struct _Bricli_t
{
	uint8_t Buffer[BRICLI_BUFFER_SIZE];	// The internal memory buffer
	uint8_t Eol[BRICLI_EOL_SIZE];		// The current End-of-Line character for this parser
} Bricli_t;

/**
 * Structure containing all initialisation values for the parser
 */
typedef struct _BricliInit_t
{
	const char *Eol;
} BricliInit_t;

/**
 * Contains error types for use with the BriCLI library
 */
typedef enum _BricliError_t
{
	BricliErrorOk,				// 0: Success
	BricliErrorUnknown,			// 1: An unexpected error occurred
	BricliErrorNullArgument,	// 2: A NULL value was incorrectly given as an argument
	BricliErrorInavlidArgument	// 3: An invalid parameter was given as an argument
} BricliError_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// C functions and constants

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* BRICLI_TYPES_H */
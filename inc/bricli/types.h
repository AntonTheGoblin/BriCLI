#ifndef BRICLI_TYPES_H
#define BRICLI_TYPES_H

/**
 * Contains error types for use with the BriCLI library
 */
typedef enum _BricliError_t
{
	BricliErrorOk,				// 0: Success
	BricliErrorUnknown,			// 1: An unexpected error occurred
	BricliErrorNullArgument,	// 2: A NULL value was incorrectly given as an argument
	BricliErrorInavlidArugment	// 3: An invalid parameter was given as an argument
} BricliError_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// C functions and constants

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* BRICLI_TYPES_H */
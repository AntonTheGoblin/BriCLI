#ifndef BRICLI_ERROR_H
#define BRICLI_ERROR_H

typedef enum _BricliLastError_t
{
    BricliErrorNone,
    BricliErrorInternal,
    BricliErrorCommand
} BricliLastError_t;

typedef enum _BricliErrors_t
{
    BricliUnknown            = -8,
    BricliUnauthorized		 = -7,
    BricliReceivedNull       = -6,
    BricliCopyWouldOverflow  = -5,
    BricliBadCommand         = -4,
    BricliBadParameter       = -3,
    BricliBadHandle          = -2,
    BricliBadFunction        = -1,
    BricliOk                 = 0
} BricliErrors_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// C functions and constants

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BRICLI_ERROR_H

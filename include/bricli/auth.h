/**
 * @file auth.h
 * @author Anthony Wall
 * @brief BriCLI AuthN/AuthZ header
 * 
 * @copyright Copyright (c) 2025 Anthony Wall.
 *            All rights reserved
 */
#ifndef BRICLI_AUTH_H
#define BRICLI_AUTH_H

#include <stdint.h>


typedef enum _BricliAuthErrors_t
{
    BricliAuthDenied = -1,       // Request for auth denied
    BricliAuthGranted = 0,      // Request for auth elevation granted
    BricliAuthRevoked = 1       // Request for auth decrease granted
} _BricliAuthErrors_t;

/**
 * @brief CLI Authorization scopes, represented as up to 32 bit flags
 */
typedef uint32_t BricliAuthScopes_t;

// Unauthorized scope, available to anyone without login
#define BricliScopeAll                  (0)

// User scope
#define BricliScopeUser                 (1 << 1)

// Admin scope
#define BricliScopeAdmin                (1 << 2)

typedef struct _BricliAuthEntry_t
{
    const char *Username;
    const char *Password;
    BricliAuthScopes_t Scopes;
} BricliAuthEntry_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BRICLI_AUTH_H

#ifndef BRICLI_H
#define BRICLI_H

#include "bricli_config.h"
#include <bricli/options.h>
#include <bricli/types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	BricliError_t Bricli_Init(Bricli_t *cli, BricliInit_t *init);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BRICLI_H
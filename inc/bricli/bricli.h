/**
 * @file    bricli.h
 * @brief   Header for the Bricli library
 * @date    25/03/2025
 * @version 1
 * @author  Anthony Wall
 *
 * Copyright (C) 2025 Anthony Wall.
 * All rights reserved.
 *
 **/

#ifndef BRICLI_H
#define BRICLI_H

#include "bricli_config.h"
#include <bricli/options.h>
#include <bricli/types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	BricliError_t Bricli_Init(Bricli_t *cli, BricliInit_t *init);
	BricliError_t Bricli_Parse(uint8_t *buffer, uint32_t length);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BRICLI_H
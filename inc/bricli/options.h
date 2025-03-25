/**
 * @file    options.h
 * @brief   Contains default options implementations and collates bricli_config.h access
 * @date    25/03/2025
 * @version 1
 * @author  Anthony Wall
 *
 * Copyright (C) 2025 Anthony Wall.
 * All rights reserved.
 *
 **/

#ifndef BRICLI_OPTIONS_H
#define BRICLI_OPTIONS_H

#include "bricli_config.h"

// Sets the size of the internal response buffer in bytes
#ifndef BRICLI_BUFFER_SIZE
#define BRICLI_BUFFER_SIZE 80
#endif // BRICLI_BUFFER_SIZE

// Sets the size of the EOL buffer including a space for the NULL terminator
#define BRICLI_EOL_SIZE 3

#endif /* BRICLI_OPTIONS_H */
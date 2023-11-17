/**
 * @file  memory_request.h
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef INCLUDE_MEMORY_REQUEST_H_
#define INCLUDE_MEMORY_REQUEST_H_

#include <stdio.h>
#include <stdlib.h>
#include "common.h"

void memory_request_init(MemoryRequest_t *memoryRequest, unsigned long time,
                         uint8_t core, uint8_t operation,
                         unsigned long long address);

#endif  // INCLUDE_MEMORY_REQUEST_H_
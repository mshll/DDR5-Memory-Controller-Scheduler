/**
 * @file  memory_request.h
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __MEMORY_REQUEST_H__
#define __MEMORY_REQUEST_H__

#include "common.h"

void memory_request_init(MemoryRequest_t *memoryRequest, unsigned long time,
                         uint8_t core, uint8_t operation,
                         unsigned long long address);

#endif  // __MEMORY_REQUEST_H__
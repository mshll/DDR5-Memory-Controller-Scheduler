/**
 * @file  memory_request.h
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __MEMORY_REQUEST_H__
#define __MEMORY_REQUEST_H__

#include "common.h"

void memory_request_init(MemoryRequest_t *memoryRequest, uint64_t time, uint8_t core, uint8_t operation, uint64_t address);
void log_memory_request(char *prefix, MemoryRequest_t *memory_request, uint64_t cycle);

#endif
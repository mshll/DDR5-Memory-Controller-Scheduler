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

typedef struct MemoryRequest {
  unsigned long time;
  uint8_t core;
  uint8_t operation;
  // unsigned long long address;
  uint8_t byte_select;  // 2 bits
  uint8_t column_low;   // 4 bits (column[3:0])
  uint8_t channel;      // 1 bit
  uint8_t bank_group;   // 3 bits
  uint8_t bank;         // 2 bits
  uint8_t column_high;  // 6 bits (column[9:4])
  uint32_t row;         // 16 bits
} MemoryRequest_t;

void memory_request_init(MemoryRequest_t *memoryRequest, unsigned long time,
                         uint8_t core, uint8_t operation,
                         unsigned long long address);

#endif  // INCLUDE_MEMORY_REQUEST_H_
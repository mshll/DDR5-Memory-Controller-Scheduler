/**
 * @file  memory_request.h
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __MEMORY_REQUEST_H__
#define __MEMORY_REQUEST_H__

#include "common.h"


typedef enum MemoryRequestState {
  PENDING,
  ACT0,
  ACT1,
  RD0,
  RD1,
  WR0,
  WR1,
  BUFFER,
  BURST,
  PRE,
  COMPLETE
} MemoryRequestState_t;


typedef struct __attribute__((__packed__)) MemoryRequest {
  uint64_t time;
  uint8_t core;
  uint8_t operation;
  uint16_t byte_select : 2;  // 2 bits
  uint16_t column_low : 4;   // 4 bits (column[3:0])
  uint16_t channel : 1;      // 1 bit
  uint16_t bank_group : 3;   // 3 bits
  uint16_t bank : 2;         // 2 bits
  uint16_t column_high : 6;  // 6 bits (column[9:4])
  uint16_t row : 16;         // 16 bits
  MemoryRequestState_t state;
  uint32_t aging;
} MemoryRequest_t;

void memory_request_init(MemoryRequest_t *memoryRequest, uint64_t time, uint8_t core, uint8_t operation, uint64_t address);
void log_memory_request(char *prefix, MemoryRequest_t *memory_request, uint64_t cycle);
uint16_t get_column(MemoryRequest_t *memory_request);

#endif
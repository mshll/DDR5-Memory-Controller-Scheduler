#ifndef __COMMON_H__
#define __COMMON_H__

/*** includes ***/
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*** macro(s) ***/
#ifdef DEBUG
#define LOG_DEBUG(format, ...) printf("%s:%d: " format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(...) /*** expands to nothing ***/
#define LOG(...)       /*** expands to nothing ***/
#endif

/*** enum(s) ***/
enum Operation {
  DATA_READ = 0,
  DATA_WRITE = 1,
  IFETCH = 2,
};

// TODO 'REF' not implemented yet
typedef enum MemoryRequestState {
  PENDING,
  REF,
  ACT0,
  ACT1,
  RW0,
  RW1,
  PRE,
  COMPLETE,
} MemoryRequestState_t;

/*** struct(s) ***/
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
} MemoryRequest_t;

#endif
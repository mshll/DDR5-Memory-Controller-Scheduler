#ifndef __COMMON_H__
#define __COMMON_H__

/*** includes ***/
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/*** macro(s), enum(s), struct(s) ***/
// #define OPEN_PAGE_POLICY  // comment out to use closed page policy
#ifdef DEBUG
#define LOG_DEBUG(format, ...) printf("%s:%d: " format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(...) /*** expands to nothing ***/
#define LOG(...)       /*** expands to nothing ***/
#endif


enum SchedulingAlgorithms {
  LEVEL_0,
  LEVEL_1,
  LEVEL_2,
  LEVEL_3
};

typedef enum Operation {
  DATA_READ = 0,
  DATA_WRITE = 1,
  IFETCH = 2,
} Operation_t;

typedef enum Commands {
  ACTIVATE,
  READ,
  WRITE,
  PRECHARGE,
  REFRESH
} Commands_t;

#endif
/**
 * @file  dram.h
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __DIMM_H__
#define __DIMM_H__

#include "common.h"
#include "memory_request.h"
#include "queue.h"

/*** macro(s) ***/
#define TRC 115
#define TRAS 76
#define TRRD_L 12
#define TRRD_S 8
#define TRP 39
#define TRFC 708 // 295ns
#define TCWD 38
#define TCL 40
#define TRCD 39
#define TWR 30
#define TRTP 18
#define TCCD_L 12
#define TCCD_S 8
#define TCCD_L_WR 48
#define TCCD_S_WR 8
#define TBURST 8
#define TCCD_L_RTW 16
#define TCCD_S_RTW 16
#define TCCD_L_WTR 70
#define TCCD_S_WTR 52 
#define TFAW 10 // CHANGE ME -- dont have datasheet to find this value
#define NUM_OF_TFAW_COUNTERS 4

#define NUM_BANKS 32
#define NUM_BANK_GROUPS 4
#define NUM_BANKS_PER_GROUP (NUM_BANKS / NUM_BANK_GROUPS)
#define NUM_CHANNELS 2
#define NUM_CHIPS_PER_CHANNEL 4

#define CACHE_LINE_BOUNDARY 64

/*** struct(s) ***/
typedef struct __attribute__((__packed__)) Bank {
  bool is_precharged;
  bool is_active;
  uint32_t active_row;
} Bank_t;

typedef struct BankGroup {
  Bank_t banks[NUM_BANKS_PER_GROUP];
} BankGroup_t;

typedef struct DRAM {
  BankGroup_t bank_groups[NUM_BANK_GROUPS];
  uint16_t tFAW_counters[NUM_OF_TFAW_COUNTERS];
} DRAM_t;

typedef struct Channel {
  DRAM_t DDR5_chip[NUM_CHIPS_PER_CHANNEL];
} Channel_t;

typedef struct __attribute__((aligned(CACHE_LINE_BOUNDARY))) DIMM {
  Channel_t channels[NUM_CHANNELS];
  FILE *output_file;
} DIMM_t;

/*** function declaration(s) ***/
void dimm_create(DIMM_t **dimm, char *output_file_name);
void dimm_destroy(DIMM_t **dimm);
void process_request(DIMM_t **dimm, Queue_t **q, uint64_t dimm_cycle, uint8_t scheduling_algorithm);

// TODO add additional functions as necessary

#endif
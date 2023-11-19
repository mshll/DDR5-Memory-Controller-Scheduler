/**
 * @file  dram.h
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __DIMM_H__
#define __DIMM_H__

#include "common.h"

/*** macro(s) ***/
#define NUM_BANKS 32
#define NUM_BANK_GROUPS 4
#define NUM_BANKS_PER_GROUP (NUM_BANKS / NUM_BANK_GROUPS)
#define NUM_CHANNELS 2
#define NUM_CHIPS_PER_CHANNEL 4

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
} DRAM_t;

typedef struct Channel {
  DRAM_t DDR5_chip[NUM_CHIPS_PER_CHANNEL];
} Channel_t;

typedef struct DIMM {
  Channel_t channels[NUM_CHANNELS];
} DIMM_t;

/*** function declaration(s) ***/
void dimm_create(DIMM_t **dimm);
void dram_init(DRAM_t *dram);
void activate_bank(DRAM_t *dram, MemoryRequest_t *request);
void precharge_bank(DRAM_t *dram, MemoryRequest_t *request);
bool is_row_hit(DRAM_t dram, MemoryRequest_t *request);

char *issue_cmd(char *cmd, MemoryRequest_t *request);
int process_request(DIMM_t **dimm, MemoryRequest_t *request);

// TODO add additional functions as necessary

#endif  
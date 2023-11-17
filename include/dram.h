/**
 * @file  dram.h
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __DRAM_H__
#define __DRAM_H__

#include "common.h"

/*** macro(s) ***/
#define NUM_BANKS 32
#define NUM_BANK_GROUPS 4
#define NUM_BANKS_PER_GROUP (NUM_BANKS / NUM_BANK_GROUPS)
#define NUM_CHANNELS 2

/*** struct(s) ***/
typedef struct Bank {
  bool is_precharged;
  bool is_active;
  uint32_t active_row;
} Bank_t;

typedef struct BankGroup {
  Bank_t banks[NUM_BANKS_PER_GROUP];

} BankGroup_t;

typedef struct Channel {
  BankGroup_t bank_groups[NUM_BANK_GROUPS];
} Channel_t;

typedef struct DRAM {
  Channel_t channels[NUM_CHANNELS];
} DRAM_t;

/*** function declaration(s) ***/
void dram_init(DRAM_t **dram);
void activate_bank(DRAM_t **dram, MemoryRequest_t *request);
void precharge_bank(DRAM_t **dram, MemoryRequest_t *request);
bool is_row_hit(DRAM_t **dram, MemoryRequest_t *request);

char *issue_cmd(char *cmd, MemoryRequest_t *request);
int process_request(DRAM_t **dram, MemoryRequest_t *request);

// TODO add additional functions as necessary

#endif  // __DRAM_H__
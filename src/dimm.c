/**
 * @file  dram.c
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "dimm.h"

void dimm_create(DIMM_t **dimm) {
  *dimm = malloc(sizeof(DIMM_t));

  if (dimm == NULL) {
    // error; malloc failed
  }

  for (int i = 0; i < NUM_CHANNELS; i++) {
    for (int j = 0; j < NUM_CHIPS_PER_CHANNEL; j++) {
      dram_init(&( (*dimm)->channels[i].DDR5_chip[j] ));
    }
  }
}

// Initialize the DRAM with all banks precharged
void dram_init(DRAM_t *dram) {

  for (int i = 0; i < NUM_BANK_GROUPS; i++) {
    for (int j = 0; j < NUM_BANKS_PER_GROUP; j++) {
      dram->bank_groups[i].banks[j].is_precharged = true;
      dram->bank_groups[i].banks[j].is_active = false;
      dram->bank_groups[i].banks[j].active_row = 0;
    }
  }

}

int process_request(DRAM_t **dram, MemoryRequest_t *request) {
  // TODO figure out how to process memory requests
    bool row_hit = is_row_hit(dram, request);
    if (!row_hit) {
        
        if ((*dram)->channels[request->channel].bank_groups[request->bank_group].banks[request->bank].is_active) {
            precharge_bank(dram, request);
        }
        
        activate_bank(dram, request);
    }
  switch (request->operation) {
    case DATA_READ:
      // Issue read command
      issue_cmd("RD", request);
      break;
    case DATA_WRITE:
      // Issue write command
      issue_cmd("WR", request);
      break;
    case INSTRUCTION_FETCH:
    
      break;
    default:
      //  invalid operation
      break;
  }
  return 0;
}

void activate_bank(DRAM_t *dram, MemoryRequest_t *request) {

  dram->bank_groups[request->bank_group].banks[request->bank].is_active = true;
  dram->bank_groups[request->bank_group].banks[request->bank].active_row = request->row;
}

void precharge_bank(DRAM_t *dram, MemoryRequest_t *request) {

  dram->bank_groups[request->bank_group].banks[request->bank].is_precharged = true;
  dram->bank_groups[request->bank_group].banks[request->bank].is_active = false;
  dram->bank_groups[request->bank_group].banks[request->bank].active_row = 0;
}

bool is_row_hit(DRAM_t dram, MemoryRequest_t *request) {

  bool hit_result = (
    dram.bank_groups[request->bank_group].banks[request->bank].is_active  &&
    dram.bank_groups[request->bank_group].banks[request->bank].active_row ==
    request->row
  );

  return hit_result;
}

/**
 * @brief Constructs a command string to be written to the output file.
 *
 * @param cmd     command string (ACT, PRE, RD, or WR)
 * @param request memory request
 * @return char*  command string to be written to the output file
 */
char *issue_cmd(char *cmd, MemoryRequest_t *request) {
  char *response = malloc(sizeof(char) * 100);

  sprintf(response, "%llu %u %4s ", request->time, request->channel, cmd);

  if (strncmp(cmd, "ACT", 3) == 0) {
    sprintf(response, "%u %u %u", request->bank_group, request->bank, request->row);

  } else if (strncmp(cmd, "PRE", 3) == 0) {
    sprintf(response, "%u %u", request->bank_group, request->bank);

  } else if (strncmp(cmd, "RD", 2) == 0 || strncmp(cmd, "WR", 2) == 0) {
    sprintf(response, "%u %u %u", request->bank_group, request->bank, request->column_high);
  }

  return response;
}

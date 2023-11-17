/**
 * @file  dram.c
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "dram.h"

// Initialize the DRAM with all banks precharged
void dram_init(DRAM_t **dram) {
  *dram = malloc(sizeof(DRAM_t));

  for (int i = 0; i < NUM_CHANNELS; i++) {
    for (int j = 0; j < NUM_BANK_GROUPS; j++) {
      for (int k = 0; k < NUM_BANKS_PER_GROUP; k++) {
        (*dram)->channels[i].bank_groups[j].banks[k].is_precharged = true;
        (*dram)->channels[i].bank_groups[j].banks[k].is_active = false;
        (*dram)->channels[i].bank_groups[j].banks[k].active_row = 0;
      }
    }
  }
}

int process_request(DRAM_t **dram, MemoryRequest_t *request) {
  // TODO figure out how to process memory requests
  return 0;
}

void activate_bank(DRAM_t **dram, MemoryRequest_t *request) {
  for (int i = 0; i < NUM_BANKS_PER_GROUP; i++) {
    (*dram)->channels[request->channel].bank_groups[request->bank_group].banks[i].is_precharged = false;
    (*dram)->channels[request->channel].bank_groups[request->bank_group].banks[i].is_active = false;
    (*dram)->channels[request->channel].bank_groups[request->bank_group].banks[i].active_row = 0;
  }
  (*dram)->channels[request->channel].bank_groups[request->bank_group].banks[request->bank].is_active = true;
  (*dram)->channels[request->channel].bank_groups[request->bank_group].banks[request->bank].active_row = request->row;
}

void precharge_bank(DRAM_t **dram, MemoryRequest_t *request) {
  (*dram)->channels[request->channel].bank_groups[request->bank_group].banks[request->bank].is_precharged = true;
  (*dram)->channels[request->channel].bank_groups[request->bank_group].banks[request->bank].is_active = false;
  (*dram)->channels[request->channel].bank_groups[request->bank_group].banks[request->bank].active_row = 0;
}

bool is_row_hit(DRAM_t **dram, MemoryRequest_t *request) {
  return ((*dram)->channels[request->channel].bank_groups[request->bank_group].banks[request->bank].is_active &&
          (*dram)->channels[request->channel].bank_groups[request->bank_group].banks[request->bank].active_row ==
              request->row);
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

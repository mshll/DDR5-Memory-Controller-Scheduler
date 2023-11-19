/**
 * @file  dram.c
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "dimm.h"

/*** helper function(s) ***/
bool is_bank_active(DRAM_t *dram, MemoryRequest_t *request) {
  bool active_result = dram->bank_groups[request->bank_group].banks[request->bank].is_active;
  return active_result;
}

bool is_bank_precharged(DRAM_t *dram, MemoryRequest_t *request) {
  bool precharged_result = dram->bank_groups[request->bank_group].banks[request->bank].is_precharged;
  return precharged_result;
}

bool is_row_hit(DRAM_t *dram, MemoryRequest_t *request) {
  bool hit_result = is_bank_active(dram, request) && dram->bank_groups[request->bank_group].banks[request->bank].active_row == request->row;
  return hit_result;
}

/*** function(s) ***/
void dimm_create(DIMM_t **dimm) {
  *dimm = malloc(sizeof(DIMM_t));

  if (*dimm == NULL) {
    // error; malloc failed
    perror("Error allocating memory for DIMM");
    exit(1);
  }

  for (int i = 0; i < NUM_CHANNELS; i++) {
    for (int j = 0; j < NUM_CHIPS_PER_CHANNEL; j++) {
      dram_init(&((*dimm)->channels[i].DDR5_chip[j]));
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

int process_request(DIMM_t **dimm, MemoryRequest_t *request) {
  DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);

  // Set the initial state before processing the request
  if (request->state == PENDING) {
    if (is_row_hit(dram, request)) {
      request->state = RW0;
    } else if (is_bank_precharged(dram, request)) {
      request->state = ACT0;
    } else {
      request->state = PRE;
    }
  }

  // Process the request (one state per cycle)
  // TODO write to output file instead of printing to stdout
  // TODO not sure how/when to do 'REF' command
  switch (request->state) {
    case PRE:
      precharge_bank(dram, request);
      printf("%s\n", issue_cmd("PRE", request));

      request->state++;
      break;

    case ACT0:
      activate_bank(dram, request);
      printf("%s\n", issue_cmd("ACT0", request));

      request->state++;
      break;

    case ACT1:
      printf("%s\n", issue_cmd("ACT1", request));

      request->state++;
      break;

    case RW0:
      // TODO What to do for instruction fetch? (currently treated as read)
      if (request->operation == DATA_READ || request->operation == IFETCH) {
        printf("%s\n", issue_cmd("RD0", request));
      } else if (request->operation == DATA_WRITE) {
        printf("%s\n", issue_cmd("WR0", request));
      }

      request->state++;
      break;

    case RW1:
      if (request->operation == DATA_READ || request->operation == IFETCH) {
        printf("%s\n", issue_cmd("RD1", request));
      } else if (request->operation == DATA_WRITE) {
        printf("%s\n", issue_cmd("WR1", request));
      }

      request->state++;
      break;

    case COMPLETE:
      break;

    case PENDING:
    default:
      // TODO error
      break;
  }

  return 0;
}

void activate_bank(DRAM_t *dram, MemoryRequest_t *request) {
  // Check if any other bank in the group is active and deactivate it
  // TODO not sure if this is correct
  for (int i = 0; i < NUM_BANKS_PER_GROUP; i++) {
    dram->bank_groups[request->bank_group].banks[i].is_active = false;
  }

  dram->bank_groups[request->bank_group].banks[request->bank].is_precharged = false;
  dram->bank_groups[request->bank_group].banks[request->bank].is_active = true;
  dram->bank_groups[request->bank_group].banks[request->bank].active_row = request->row;
}

void precharge_bank(DRAM_t *dram, MemoryRequest_t *request) {
  dram->bank_groups[request->bank_group].banks[request->bank].is_precharged = true;
  dram->bank_groups[request->bank_group].banks[request->bank].is_active = false;
  dram->bank_groups[request->bank_group].banks[request->bank].active_row = 0;
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
  char *temp = malloc(sizeof(char) * 100);

  sprintf(response, "%10llu %u %4s", request->time, request->channel, cmd);

  if (strncmp(cmd, "ACT", 3) == 0) {
    sprintf(temp, " %u %u %X", request->bank_group, request->bank, request->row);

  } else if (strncmp(cmd, "PRE", 3) == 0) {
    sprintf(temp, " %u %u", request->bank_group, request->bank);

  } else if (strncmp(cmd, "RD", 2) == 0 || strncmp(cmd, "WR", 2) == 0) {
    sprintf(temp, " %u %u %X", request->bank_group, request->bank, request->column_high);
  }

  strcat(response, temp);
  free(temp);

  return response;
}

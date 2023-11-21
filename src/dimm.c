/**
 * @file  dram.c
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "dimm.h"
#include "memory_request.h"

/*** helper function(s) ***/
bool is_bank_active(DRAM_t *dram, MemoryRequest_t *request) {
  bool active_result = dram->bank_groups[request->bank_group].banks[request->bank].is_active;
  return active_result;
}

bool is_bank_precharged(DRAM_t *dram, MemoryRequest_t *request) {
  bool precharged_result = dram->bank_groups[request->bank_group].banks[request->bank].is_precharged;
  return precharged_result;
}

bool is_page_hit(DRAM_t *dram, MemoryRequest_t *request) {
  bool result = is_bank_active(dram, request) && dram->bank_groups[request->bank_group].banks[request->bank].active_row == request->row;
  return result;
}

bool is_page_miss(DRAM_t *dram, MemoryRequest_t *request) {
  bool result = is_bank_active(dram, request) && dram->bank_groups[request->bank_group].banks[request->bank].active_row != request->row;
  return result;
}

bool is_page_empty(DRAM_t *dram, MemoryRequest_t *request) {
  bool result = is_bank_precharged(dram, request) && !is_bank_active(dram, request);
  return result;
}

/*** function(s) ***/
void dimm_create(DIMM_t **dimm, char *output_file_name) {
  *dimm = malloc(sizeof(DIMM_t));

  if (*dimm == NULL) {
    // error; malloc failed
    perror("Error allocating memory for DIMM");
    exit(1);
  }

  // opening the file
  (*dimm)->output_file = fopen(output_file_name, "w");
  if ((*dimm)->output_file == NULL) {
    perror("Error opening output file");
    exit(1);
  }

  for (int i = 0; i < NUM_CHANNELS; i++) {
    for (int j = 0; j < NUM_CHIPS_PER_CHANNEL; j++) {
      dram_init(&((*dimm)->channels[i].DDR5_chip[j]));
    }
  }
}

void dimm_destroy(DIMM_t **dimm) {
  if (*dimm != NULL) {
    // closing the file
    if ((*dimm)->output_file) {
      fclose((*dimm)->output_file);
    }

    free(*dimm);
    *dimm = NULL;  // remove dangler
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

int process_request(DIMM_t **dimm, MemoryRequest_t *request, uint64_t cycle) {
  DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);
  char *cmd = NULL;

  // Set the initial state before processing the request
  if (request->state == PENDING) {
#ifdef OPEN_PAGE_POLICY
    if (is_page_hit(dram, request)) {
      request->state = RW0;

    } else if (is_page_miss(dram, request)) {
      dram->bank_groups[request->bank_group].banks[request->bank].is_active = false;
      request->state = PRE;

    } else if (is_page_empty(dram, request)) {
      request->state = ACT0;
    }
#else  // closed page policy
    request->state = ACT0;
#endif
  }

  // Process the request (one state per cycle)
  switch (request->state) {
    case PRE:
      precharge_bank(dram, request);
      cmd = issue_cmd("PRE", request, cycle);

      request->state++;  // goes to ACT0 if open page policy, COMPLETE if closed page policy
      break;

    case ACT0:
      activate_bank(dram, request);
      cmd = issue_cmd("ACT0", request, cycle);

      request->state++;
      break;

    case ACT1:
      cmd = issue_cmd("ACT1", request, cycle);

      request->state++;
      break;

    case RW0:
      cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);

      request->state++;
      break;

    case RW1:
      cmd = issue_cmd(request->operation == DATA_WRITE ? "WR1" : "RD1", request, cycle);
#ifndef OPEN_PAGE_POLICY  // closed page policy
      dram->bank_groups[request->bank_group].banks[request->bank].is_active = false;
#endif

      request->state++;  // goes to PRE if open page policy, COMPLETE if closed page policy
      break;

    case COMPLETE:
      break;

    case PENDING:
    default:
      // TODO error
      break;
  }

  // writing commands to output file
  if (cmd != NULL) {
    fprintf((*dimm)->output_file, "%s\n", cmd);
    free(cmd);
  }

  return 0;
}

void activate_bank(DRAM_t *dram, MemoryRequest_t *request) {
  dram->bank_groups[request->bank_group].banks[request->bank].is_active = true;
  dram->bank_groups[request->bank_group].banks[request->bank].active_row = request->row;
}

void precharge_bank(DRAM_t *dram, MemoryRequest_t *request) {
  dram->bank_groups[request->bank_group].banks[request->bank].is_precharged = true;
}

/**
 * @brief Constructs a command string to be written to the output file.
 *
 * @param cmd     command string (ACT, PRE, RD, or WR)
 * @param request memory request
 * @return char*  command string to be written to the output file
 */
char *issue_cmd(char *cmd, MemoryRequest_t *request, uint64_t cycle) {
  char *response = malloc(sizeof(char) * 100);
  char *temp = malloc(sizeof(char) * 100);

  sprintf(response, "%10llu %u %4s", cycle, request->channel, cmd);

  if (strncmp(cmd, "ACT", 3) == 0) {
    sprintf(temp, " %u %u 0x%X", request->bank_group, request->bank, request->row);

  } else if (strncmp(cmd, "PRE", 3) == 0) {
    sprintf(temp, " %u %u", request->bank_group, request->bank);

  } else if (strncmp(cmd, "RD", 2) == 0 || strncmp(cmd, "WR", 2) == 0) {
    sprintf(temp, " %u %u 0x%X", request->bank_group, request->bank, get_column(request));
  }

  strcat(response, temp);
  free(temp);

  return response;
}

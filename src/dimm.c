/**
 * @file  dram.c
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "dimm.h"

uint16_t timing_attribute[NUM_TIMING_CONSTRAINTS] = {
  TRC,
  TRAS,
  TRRD_L,
  TRRD_S,
  TRP,
  TRFC,
  TCWD,
  TCL,
  TRCD,
  TWR,
  TRTP,
  TCCD_L,
  TCCD_S,
  TCCD_L_WR,
  TCCD_S_WR,
  TBURST,
  TCCD_L_RTW,
  TCCD_S_RTW,
  TCCD_L_WTR,
  TCCD_S_WTR
};

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

void activate_bank(DRAM_t *dram, MemoryRequest_t *request) {
  dram->bank_groups[request->bank_group].banks[request->bank].is_active = true;
  dram->bank_groups[request->bank_group].banks[request->bank].active_row = request->row;
}

void precharge_bank(DRAM_t *dram, MemoryRequest_t *request) {
  dram->bank_groups[request->bank_group].banks[request->bank].is_precharged = true;
}

char *issue_cmd(char *cmd, MemoryRequest_t *request, uint64_t cycle) {
  /**
   * @brief Constructs a command string to be written to the output file.
   *
   * @param cmd     command string (ACT, PRE, RD, or WR)
   * @param request memory request
   * @return char*  command string to be written to the output file
   */
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

void set_tfaw_counter(DRAM_t *dram) {
  for (int i = 0; i < NUM_TFAW_COUNTERS; i++) {
    if (dram->tFAW_counters[i] == 0) {
      dram->tFAW_counters[i] = TFAW;
      break;  // only want to set one counter at a time
    }
  }
}

void set_timing_constraint(DRAM_t *dram, MemoryRequest_t *request, TimingConstraints_t constraint_type) {
  dram->timing_constraints[request->bank_group][request->bank][constraint_type] = timing_attribute[constraint_type];
}

bool is_timing_constraint_met(DRAM_t *dram, MemoryRequest_t *request, TimingConstraints_t constraint_type) {
  bool result = dram->timing_constraints[request->bank_group][request->bank][constraint_type] == 0;
  return result;
}

void decrement_tfaw_counters(DRAM_t *dram) {
  for (int i = 0; i < NUM_TFAW_COUNTERS; i++) {
    if (dram->tFAW_counters[i] != 0) {
      dram->tFAW_counters[i]--;
    }
  }
}

void decrement_timing_constraints(DRAM_t *dram) {
  // TODO: need a table to keep track of active timers. 3 nested loop is too big of a hit on performance
  // works for now
  for (int i = 0; i < NUM_BANK_GROUPS; i++) {
    for (int j = 0; j < NUM_BANKS_PER_GROUP; j++) {
      for (int k = 0; k < NUM_TIMING_CONSTRAINTS; k++) {
        if (dram->timing_constraints[i][j][k] != 0) {
          dram->timing_constraints[i][j][k]--;
        }
      }
    }
  }
}

// Checks if tCCD_S or tCCD_L timing constraint is met
bool is_tccds_met(DRAM_t *dram, MemoryRequest_t *request) {
  if (dram->last_operation == DATA_WRITE && request->operation == DATA_WRITE) {  // write to write
    return dram->last_bank_group == request->bank_group ? is_timing_constraint_met(dram, request, tCCD_L_WR)
                                                        : is_timing_constraint_met(dram, request, tCCD_S_WR);
  } else if (dram->last_operation != DATA_WRITE && request->operation == DATA_WRITE) {  // read to write
    return dram->last_bank_group == request->bank_group ? is_timing_constraint_met(dram, request, tCCD_L_RTW)
                                                        : is_timing_constraint_met(dram, request, tCCD_S_RTW);
  } else if (dram->last_operation == DATA_WRITE && request->operation != DATA_WRITE) {  // write to read
    return dram->last_bank_group == request->bank_group ? is_timing_constraint_met(dram, request, tCCD_L_WTR)
                                                        : is_timing_constraint_met(dram, request, tCCD_S_WTR);
  } else {  // read to read
    return dram->last_bank_group == request->bank_group ? is_timing_constraint_met(dram, request, tCCD_L)
                                                        : is_timing_constraint_met(dram, request, tCCD_S);
  }
}

bool can_issue_act(DRAM_t *dram) {
  // if any counter is set to 0 then we can issue an ACT cmd
  // without violating the tFAW timing constraint
  for (int i = 0; i < NUM_TFAW_COUNTERS; i++) {
    if (dram->tFAW_counters[i] == 0) {
      return true;
    }
  }

  return false;
}

bool closed_page(DIMM_t **dimm, MemoryRequest_t *request, uint64_t clock) {
  DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);
  char *cmd = NULL;
  bool issued_cmd = false;

  // Set the initial state before processing the request
  LOG("cycle %llu, state %d \n", clock, request->state);

  if (request->state == PENDING) {
    if (!is_timing_constraint_met(dram, request, tRP)) {
      return issued_cmd;
    }
    request->state = ACT0;
  }

  // Process the request (one state per cycle)
  switch (request->state) {
    case ACT0:
      LOG("ACT0\n");
      cmd = issue_cmd("ACT0", request, clock);
      request->state = ACT1;

      break;

    case ACT1:
      LOG("ACT1\n");
      set_timing_constraint(dram, request, tRCD);
      activate_bank(dram, request);
      cmd = issue_cmd("ACT1", request, clock);
      request->state = RW0;

      break;

    case RW0:
      LOG("RW0\n");
      if (is_timing_constraint_met(dram, request, tRCD)) {
        cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, clock);
        request->state = RW1;
      }
      break;

    case RW1:
      LOG("RW1\n");
      set_timing_constraint(dram, request, tCL);
      set_timing_constraint(dram, request, tRTP);
      cmd = issue_cmd(request->operation == DATA_WRITE ? "WR1" : "RD1", request, clock);
      dram->bank_groups[request->bank_group].banks[request->bank].is_active = false;
      request->state = BURST;

      break;

    case BURST:
      LOG("BURST\n");
      if (is_timing_constraint_met(dram, request, tCL)) {
        set_timing_constraint(dram, request, tBURST);
        request->state = COMPLETE;
      }
      if (is_timing_constraint_met(dram, request, tRTP) && is_timing_constraint_met(dram, request, tRP)) {
        precharge_bank(dram, request);
        cmd = issue_cmd("PRE", request, clock);
        set_timing_constraint(dram, request, tRP);
      }
      break;

    case PRE:
    case COMPLETE:
      break;

    case PENDING:
    default:
      fprintf(stderr, "Error: Unknown state encountered\n");
      exit(EXIT_FAILURE);
  }

  // writing commands to output file
  if (cmd != NULL) {
    fprintf((*dimm)->output_file, "%s\n", cmd);
    free(cmd);
    issued_cmd = true;
  }

  return issued_cmd;
}

bool open_page(DIMM_t **dimm, MemoryRequest_t *request, uint64_t cycle) {
  DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);
  char *cmd = NULL;
  bool issued_cmd = false;

  LOG("cycle %llu, state %d \n", cycle, request->state);

  // Set the initial state before processing the request
  if (request->state == PENDING) {
    if (is_page_hit(dram, request)) {
      request->state = RW0;

    } else if (is_page_miss(dram, request)) {
      dram->bank_groups[request->bank_group].banks[request->bank].is_active = false;
      request->state = PRE;

    } else if (is_page_empty(dram, request)) {
      request->state = ACT0;

    } else {
      fprintf(stderr, "Error: Unknown page state encountered\n");
      exit(EXIT_FAILURE);
    }
  }

  // Process the request (one state per cycle)
  switch (request->state) {
    case PRE:
      precharge_bank(dram, request);
      cmd = issue_cmd("PRE", request, cycle);
      set_timing_constraint(dram, request, tRP);
      request->state = ACT0;
      break;

    case ACT0:
      if (!is_timing_constraint_met(dram, request, tRP)) {
        return issued_cmd;
      }

      if (!can_issue_act(dram)) {
        decrement_tfaw_counters(dram);
        return issued_cmd;
      }

      set_tfaw_counter(dram);
      cmd = issue_cmd("ACT0", request, cycle);
      request->state++;
      break;

    case ACT1:
      activate_bank(dram, request);
      cmd = issue_cmd("ACT1", request, cycle);
      set_timing_constraint(dram, request, tRCD);
      request->state = RW0;
      break;

    case RW0:
      if (!is_timing_constraint_met(dram, request, tRCD)) {
        return issued_cmd;
      }

      if (!is_tccds_met(dram, request)) {
        return issued_cmd;
      }

      cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);
      request->state++;
      break;

    case RW1:
      cmd = issue_cmd(request->operation == DATA_WRITE ? "WR1" : "RD1", request, cycle);

      // set tCCD timing constraints
      if (request->operation == DATA_WRITE) {
        set_timing_constraint(dram, request, tCCD_L_WR);
        set_timing_constraint(dram, request, tCCD_S_WR);
        set_timing_constraint(dram, request, tCCD_L_WTR);
        set_timing_constraint(dram, request, tCCD_S_WTR);
      } else {
        set_timing_constraint(dram, request, tCCD_L);
        set_timing_constraint(dram, request, tCCD_S);
        set_timing_constraint(dram, request, tCCD_L_RTW);
        set_timing_constraint(dram, request, tCCD_S_RTW);
      }
      dram->last_bank_group = request->bank_group;
      dram->last_operation = request->operation;
      request->state = COMPLETE;
      break;

    case COMPLETE:
      break;

    case PENDING:
    default:
      fprintf(stderr, "Error: Unknown state encountered\n");
      exit(EXIT_FAILURE);
  }

  // writing commands to output file
  if (cmd != NULL) {
    fprintf((*dimm)->output_file, "%s\n", cmd);
    free(cmd);
    issued_cmd = true;
  }

  decrement_tfaw_counters(dram);

  return issued_cmd;
}

void level_zero_algorithm(DIMM_t **dimm, Queue_t **q, uint64_t clock) {
  MemoryRequest_t *request = queue_peek(*q);
  DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);

  if (request && request->state != COMPLETE) {
    closed_page(dimm, request, clock);
  }

  if (request && request->state == COMPLETE) {
    log_memory_request("Dequeued:", request, clock);
    dequeue(q);
  }

  decrement_tfaw_counters(dram);
  decrement_timing_constraints(dram);
}

void bank_level_parallelism(DIMM_t **dimm, Queue_t **q, uint64_t clock) {
  bool is_cmd_issued = false;

  for (int index = 0; index < 16; index++) {
    MemoryRequest_t *request = queue_peek_at(*q, index);
    DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);

    // delete once done
    if (request->state == COMPLETE) {
      queue_delete_at(q, index);
      continue;
    }

    // skip if older process is in progess for the same BA/BG of current request
    if (index != 0 && is_bank_active(dram, request)) {
      continue;
    }

    is_cmd_issued = open_page(dimm, request, clock);

    if (is_cmd_issued) {
      break;
    }
  }
}

void out_of_order() {
  // TODO: implement out of order scheduling
}

void dram_init(DRAM_t *dram) {
  // Initialize the DRAM with all banks precharged
  for (int i = 0; i < NUM_BANK_GROUPS; i++) {
    for (int j = 0; j < NUM_BANKS_PER_GROUP; j++) {
      dram->bank_groups[i].banks[j].is_precharged = true;
      dram->bank_groups[i].banks[j].is_active = false;
      dram->bank_groups[i].banks[j].active_row = 0;
      for (int k = 0; k < NUM_TIMING_CONSTRAINTS; k++) {
        dram->timing_constraints[i][j][k] = 0;
      }
    }
  }
  dram->last_bank_group = -1;
  dram->last_operation = -1;
}

/*** function(s) ***/
void dimm_create(DIMM_t **dimm, char *output_file_name) {
  *dimm = malloc(sizeof(DIMM_t));

  if (*dimm == NULL) {
    fprintf(stderr, "%s:%d: malloc failed\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }

  // opening the file
  (*dimm)->output_file = fopen(output_file_name, "w");
  if ((*dimm)->output_file == NULL) {
    fprintf(stderr, "%s:%d: fopen failed\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
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

void process_request(DIMM_t **dimm, Queue_t **q, uint64_t clock, uint8_t scheduling_algorithm) {
  switch (scheduling_algorithm) {
    case LEVEL_0:
      level_zero_algorithm(dimm, q, clock);
      break;

    case LEVEL_1:
      break;

    case LEVEL_2:
      bank_level_parallelism(dimm, q, clock);
      break;

    case LEVEL_3:
      break;

    default:
      break;
  }
}

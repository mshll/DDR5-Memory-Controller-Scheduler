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
  TRP,
  TRFC,
  TCWL,
  TCL,
  TRCD,
  TWR,
  TRTP,
  TBURST
};

uint8_t consecutive_cmd_attribute[NUM_CONSECUTIVE_CMD_CONSTRAINTS] = {
  TRRD_L,
  TRRD_S,
  TCCD_L,
  TCCD_S,
  TCCD_L_WR,
  TCCD_S_WR,
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
  dram->bank_groups[request->bank_group].banks[request->bank].is_active = false;
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

void set_tfaw_timer(DRAM_t *dram) {
  for (int i = 0; i < NUM_TFAW_COUNTERS; i++) {
    if (dram->tFAW_timers[i] == 0) {
      dram->tFAW_timers[i] = TFAW;
      break;  // only want to set one counter at a time
    }
  }
}

void set_timing_constraint(DRAM_t *dram, MemoryRequest_t *request, TimingConstraints_t constraint_type) {
  dram->timing_constraints[request->bank_group][request->bank][constraint_type] = timing_attribute[constraint_type];
}

void set_consecutive_cmd_timers(DRAM_t *dram, ConsecutiveCmdConstraints_t constraint_type) {
  dram->consecutive_cmd_timers[constraint_type] = consecutive_cmd_attribute[constraint_type];
}

void set_trrd_timers(DRAM_t *dram) {
  dram->consecutive_cmd_timers[tRRD_L] = consecutive_cmd_attribute[tRRD_L];
  dram->consecutive_cmd_timers[tRRD_S] = consecutive_cmd_attribute[tRRD_S];
}

void set_tccd_timers(DRAM_t *dram) {
  dram->consecutive_cmd_timers[tCCD_L]     = consecutive_cmd_attribute[tCCD_L];
  dram->consecutive_cmd_timers[tCCD_S]     = consecutive_cmd_attribute[tCCD_S];
  dram->consecutive_cmd_timers[tCCD_L_WR]  = consecutive_cmd_attribute[tCCD_L_WR];
  dram->consecutive_cmd_timers[tCCD_S_WR]  = consecutive_cmd_attribute[tCCD_S_WR];
  dram->consecutive_cmd_timers[tCCD_L_RTW] = consecutive_cmd_attribute[tCCD_L_RTW];
  dram->consecutive_cmd_timers[tCCD_S_RTW] = consecutive_cmd_attribute[tCCD_S_RTW];
  dram->consecutive_cmd_timers[tCCD_L_WTR] = consecutive_cmd_attribute[tCCD_L_WTR];
  dram->consecutive_cmd_timers[tCCD_S_WTR] = consecutive_cmd_attribute[tCCD_S_WTR];
}

void decrement_tfaw_timers(DRAM_t *dram) {
  for (int i = 0; i < NUM_TFAW_COUNTERS; i++) {
    if (dram->tFAW_timers[i] != 0) {
      dram->tFAW_timers[i]--;
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

void decrement_consecutive_cmd_timers(DRAM_t *dram) {
  for (int i = 0; i < NUM_CONSECUTIVE_CMD_CONSTRAINTS; i++) {
    if (dram->consecutive_cmd_timers[i] != 0) {
      dram->consecutive_cmd_timers[i]--;
    }
  }
}

bool is_timing_constraint_met(DRAM_t *dram, MemoryRequest_t *request, TimingConstraints_t constraint_type) {
  bool result = dram->timing_constraints[request->bank_group][request->bank][constraint_type] == 0;
  return result;
}

bool is_trrd_met(DRAM_t *dram, ConsecutiveCmdConstraints_t constraint_type) {
  return dram->consecutive_cmd_timers[constraint_type] == 0;
}

bool is_tccds_met(DRAM_t *dram, ConsecutiveCmdConstraints_t constraint_type) {
  return dram->consecutive_cmd_timers[constraint_type] == 0;
}

bool can_issue_act(DRAM_t *dram) {
  // if any counter is set to 0 then we can issue an ACT cmd
  // without violating the tFAW timing constraint
  for (int i = 0; i < NUM_TFAW_COUNTERS; i++) {
    if (dram->tFAW_timers[i] == 0) {
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
    request->state = ACT0;
  }

  // Process the request (one state per cycle)
  switch (request->state) {
    case ACT0:
      LOG("ACT0\tBG:%hu BA:%hu\n", request->bank_group, request->bank);
      if (
        is_timing_constraint_met(dram, request, tRC) && 
        is_timing_constraint_met(dram, request, tRP)
      ) {
        cmd = issue_cmd("ACT0", request, clock);
        request->state = ACT1;
      }
      break;

    case ACT1:
      LOG("ACT1\tBG:%hu BA:%hu\n", request->bank_group, request->bank);
      activate_bank(dram, request);

      cmd = issue_cmd("ACT1", request, clock);

      set_timing_constraint(dram, request, tRCD);
      set_timing_constraint(dram, request, tRAS);
      
      // next state
      if (request->operation == DATA_WRITE) {
        request->state = WR0;
      }
      else {
        request->state = RD0;
      }
      
      break;

    case RD0:
      LOG("RD0\tBG:%hu BA:%hu\n", request->bank_group, request->bank);
      if (is_timing_constraint_met(dram, request, tRCD)) {
        cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, clock);
        request->state = RD1;
      }
      break;

    case RD1:
      LOG("RD1\tBG:%hu BA:%hu\n", request->bank_group, request->bank);
      // issue cmd
      cmd = issue_cmd(request->operation == DATA_WRITE ? "WR1" : "RD1", request, clock);

      // set timers
      set_timing_constraint(dram, request, tCL);
      set_timing_constraint(dram, request, tRTP);

      // nest state
      request->state = PRE;
      break;

    case WR0:
      if (is_timing_constraint_met(dram, request, tRCD)) {
        cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, clock);
        request->state = WR1;
      }
      break;
    
    case WR1:
      // issue cmd
      cmd = issue_cmd(request->operation == DATA_WRITE ? "WR1" : "RD1", request, clock);

      // set timers
      set_timing_constraint(dram, request, tCWL);
      set_timing_constraint(dram, request, tWR);

      // nest state
      request->state = PRE;
      break;

    case PRE:
      LOG("PRE\tBG:%hu BA:%hu\n", request->bank_group, request->bank);
      // can precharge before bursting data
      if (request->operation == DATA_WRITE) {
        if (is_timing_constraint_met(dram, request, tWR)) {
          precharge_bank(dram, request);

          cmd = issue_cmd("PRE", request, clock);

          set_timing_constraint(dram, request, tRP);

          request->state = BUFFER_WR;
        } 
      }
      else {
        if (is_timing_constraint_met(dram, request, tRTP)) {
          precharge_bank(dram, request);

          cmd = issue_cmd("PRE", request, clock);

          set_timing_constraint(dram, request, tRP);

          request->state = BUFFER_RD;
        }
      }
      
      break;

    case BUFFER_RD:
      // data is being stored in buffer while tCL is set
      LOG("BUFFER_RD\tBG:%hu BA:%hu\n", request->bank_group, request->bank);
      if (is_timing_constraint_met(dram, request, tCL)) {
        set_timing_constraint(dram, request, tBURST);
        request->state = BURST;
      }
      break;

    case BUFFER_WR:
      // data is being stored in buffer while tCL is set
      LOG("BUFFER_WR\tBG:%hu BA:%hu\n", request->bank_group, request->bank);
      if (is_timing_constraint_met(dram, request, tCWL)) {
        set_timing_constraint(dram, request, tBURST);
        request->state = BURST;
      }
      break;

    case BURST:
      LOG("BURST\tBG:%hu BA:%hu\n", request->bank_group, request->bank);
      if (is_timing_constraint_met(dram, request, tBURST)) {
        request->state = COMPLETE;
      }
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

  return issued_cmd;
}

bool open_page(DIMM_t **dimm, MemoryRequest_t *request, uint64_t cycle) {
  DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);
  char *cmd = NULL;
  bool issued_cmd = false;
  
  LOG("cycle %llu, state %d \n", cycle/2 - 1, request->state);

  // Set the initial state before processing the request
  if (request->state == PENDING) {
    if (is_page_hit(dram, request)) {
      if (request->operation == DATA_READ) {
        request->state = RD0;
      }
      else {
        request->state = WR0;
      }
      dram->bank_groups[request->bank_group].banks[request->bank].last_request_operation = request->operation;
    } 
    else if (is_page_miss(dram, request)) {
      request->state = PRE;
    } 
    else if (is_page_empty(dram, request)) {
      if (!can_issue_act(dram)) {
        return issue_cmd;
      }

      request->state = ACT0;
      dram->bank_groups[request->bank_group].banks[request->bank].last_request_operation = request->operation;
    } 
    else {
      fprintf(stderr, "Error: Unknown page state encountered\n");
      exit(EXIT_FAILURE);
    }
  }

  // Process the request (one state per cycle)
  switch (request->state) {
    case PRE:
      if (dram->bank_groups[request->bank_group].banks[request->bank].last_request_operation == DATA_READ) {
        if (is_timing_constraint_met(dram, request, tRTP) && is_timing_constraint_met(dram, request, tRAS)) {
          precharge_bank(dram, request);

          // issue cmd
          cmd = issue_cmd("PRE", request, cycle);
          dram->last_interface_cmd = PRECHARGE;
          dram->last_bank_group = request->bank_group;
          
          // set timers
          set_timing_constraint(dram, request, tRP);

          // next state
          request->state = ACT0;
        }
      }
      else {
        if (is_timing_constraint_met(dram, request, tWR) && is_timing_constraint_met(dram, request, tRAS)) {
          precharge_bank(dram, request);

          // issue cmd
          cmd = issue_cmd("PRE", request, cycle);
          dram->last_interface_cmd = PRECHARGE;
          dram->last_bank_group = request->bank_group;
          
          // set timers
          set_timing_constraint(dram, request, tRP);

          // next state
          request->state = ACT0;
        }
      }
      
      break;

    case ACT0:
      if (dram->last_interface_cmd == ACTIVATE) {
        if (dram->last_bank_group == request->bank_group) {
          if (
            is_timing_constraint_met(dram, request, tRC) &&
            is_timing_constraint_met(dram, request, tRP) &&
            is_trrd_met(dram, tRRD_L)
          ) {
            cmd = issue_cmd("ACT0", request, cycle);
            request->state = ACT1;
          }
        }
        else {
          if (
            is_timing_constraint_met(dram, request, tRC) &&
            is_timing_constraint_met(dram, request, tRP) &&
            is_trrd_met(dram, tRRD_S)
          ) {
            cmd = issue_cmd("ACT0", request, cycle);
            request->state = ACT1;
          }
        }
      }
      else {
        if (
          is_timing_constraint_met(dram, request, tRC) &&
          is_timing_constraint_met(dram, request, tRP) 
        ) {
          cmd = issue_cmd("ACT0", request, cycle);
          request->state = ACT1;
        }
      }
      
      break;

    case ACT1:
      if (!is_bank_active(dram, request)) {
        activate_bank(dram, request);
      }

      // issue cmd
      cmd = issue_cmd("ACT1", request, cycle);
      dram->last_interface_cmd = ACTIVATE;
      dram->last_bank_group = request->bank_group;

      // set timers
      set_timing_constraint(dram, request, tRCD);
      set_timing_constraint(dram, request, tRAS);
      set_trrd_timers(dram);
      set_tfaw_timer(dram);

      // next state
      if (request->operation == DATA_WRITE) {
        request->state = WR0;
      }
      else {
        request->state = RD0;
      } 

      break;

    case RD0:
      if (dram->last_interface_cmd == READ) {
        if (dram->last_bank_group == request->bank_group) {
          if (
            is_timing_constraint_met(dram, request, tRCD) &&
            is_tccds_met(dram, tCCD_L)
          ) {
            cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);
            request->state = RD1;
          }
        }
        else {
          if (
            is_timing_constraint_met(dram, request, tRCD) &&
            is_tccds_met(dram, tCCD_S)
          ) {
            cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);
            request->state = RD1;
          }
        }
      }
      else if (dram->last_interface_cmd == WRITE) {
        if (dram->last_bank_group == request->bank_group) {
          if (
            is_timing_constraint_met(dram, request, tRCD) &&
            is_tccds_met(dram, tCCD_L_WTR)
          ) {
            cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);
            request->state = RD1;
          }
        }
        else {
          if (
            is_timing_constraint_met(dram, request, tRCD) &&
            is_tccds_met(dram, tCCD_S_WTR)
          ) {
            cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);
            request->state = RD1;
          }
        }
      }

      break;

    case RD1:
      // issue cmd
      cmd = issue_cmd(request->operation == DATA_WRITE ? "WR1" : "RD1", request, cycle);
      dram->last_interface_cmd = READ;
      dram->last_bank_group = request->bank_group;

      // set timers
      set_timing_constraint(dram, request, tCL);
      set_timing_constraint(dram, request, tRTP);

      // nest state
      request->state = BUFFER_RD;
      break;

    case WR0:
      if (dram->last_interface_cmd == WRITE) {
        if (dram->last_bank_group == request->bank_group) {
          if (
            is_timing_constraint_met(dram, request, tRCD) &&
            is_tccds_met(dram, tCCD_L_WR)
          ) {
            cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);
            request->state = WR1;
          }
        }
        else {
          if (
            is_timing_constraint_met(dram, request, tRCD) &&
            is_tccds_met(dram, tCCD_S_WR)
          ) {
            cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);
            request->state = WR1;
          }
        }
      }
      else if (dram->last_interface_cmd == READ) {
        if (dram->last_bank_group == request->bank_group) {
          if (
            is_timing_constraint_met(dram, request, tRCD) &&
            is_tccds_met(dram, tCCD_L_RTW)
          ) {
            cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);
            request->state = WR1;
          }
        }
        else {
          if (
            is_timing_constraint_met(dram, request, tRCD) &&
            is_tccds_met(dram, tCCD_S_RTW)
          ) {
            cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);
            request->state = WR1;
          }
        }
      }
      break;

    case WR1:
      // issue cmd
      cmd = issue_cmd(request->operation == DATA_WRITE ? "WR1" : "RD1", request, cycle);
      dram->last_interface_cmd = WRITE;
      dram->last_bank_group = request->bank_group;

      // set timers
      set_timing_constraint(dram, request, tCWL);
      set_timing_constraint(dram, request, tWR);

      // nest state
      request->state = BUFFER_WR;
      break;

    case BUFFER_RD:
      if (is_timing_constraint_met(dram, request, tCL)) {
        set_timing_constraint(dram, request, tBURST);
        request->state = BURST;
      }
      break;

    case BUFFER_WR:
      if (is_timing_constraint_met(dram, request, tCWL)) {
        set_timing_constraint(dram, request, tBURST);
        request->state = BURST;
      }
      break;

    case BURST:
      if (is_timing_constraint_met(dram, request, tBURST)) {
        request->state = COMPLETE;
      }
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

  return issued_cmd;
}

void level_zero_algorithm(DIMM_t **dimm, Queue_t **q, uint64_t clock) {
  MemoryRequest_t *request = queue_peek(*q);
  DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);
  print_queue(*q);
  
  if (request && request->state != COMPLETE) {
    closed_page(dimm, request, clock);
  }

  if (request && request->state == COMPLETE) {
    log_memory_request("Dequeued:", request, clock);
    dequeue(q);
  }
  
  decrement_timing_constraints(dram);
}

void level_one_algorithm(DIMM_t **dimm, Queue_t **q, uint64_t clock) {
  MemoryRequest_t *request = queue_peek(*q);
  DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);

  if (request && request->state != COMPLETE) {
    open_page(dimm, request, clock);
  }

  if (request && request->state == COMPLETE) {
    log_memory_request("Dequeued:", request, clock);
    dequeue(q);
  }
  
  decrement_tfaw_timers(dram);
  decrement_timing_constraints(dram);
}

void bank_level_parallelism(DIMM_t **dimm, Queue_t **q, uint64_t clock) {
  bool is_cmd_issued = false;
  DRAM_t *dram0 = &((*dimm)->channels[0].DDR5_chip[0]);
  DRAM_t *dram1 = &((*dimm)->channels[1].DDR5_chip[0]);
  print_queue(*q);

  for (int index = 0; index < (*q)->size; index++) {
    MemoryRequest_t *request = queue_peek_at(*q, index);
    DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);

    // delete once done
    if (request->state == COMPLETE) {
      queue_delete_at(q, index);
      continue;
    }

    // skip if older process is in progess for the same BA/BG of current request
    if (is_bank_active(dram, request) && request->state == PENDING) {
      continue;
    }

    is_cmd_issued = open_page(dimm, request, clock);

    if (is_cmd_issued) {
      break;
    }
  }

  LOG(
    "TFAW0 %hu, %hu, %hu, %hu\n", 
    dram0->tFAW_timers[0],
    dram0->tFAW_timers[1],
    dram0->tFAW_timers[2],
    dram0->tFAW_timers[3]
  );
  LOG(
    "TFAW1 %hu, %hu, %hu, %hu\n", 
    dram1->tFAW_timers[0],
    dram1->tFAW_timers[1],
    dram1->tFAW_timers[2],
    dram1->tFAW_timers[3]
  );
  decrement_timing_constraints(dram0);
  decrement_timing_constraints(dram1);
  decrement_tfaw_timers(dram0);
  decrement_tfaw_timers(dram1);
  decrement_consecutive_cmd_timers(dram0);
  decrement_consecutive_cmd_timers(dram1);
}

void dram_init(DRAM_t *dram) {
  // Initialize the DRAM with all banks precharged
  for (int i = 0; i < NUM_BANK_GROUPS; i++) {
    for (int j = 0; j < NUM_BANKS_PER_GROUP; j++) {
      dram->bank_groups[i].banks[j].is_precharged = true;
      dram->bank_groups[i].banks[j].is_active = false;
      dram->bank_groups[i].banks[j].active_row = 0;

      // zero out bank timers
      for (int k = 0; k < NUM_TIMING_CONSTRAINTS; k++) {
        dram->timing_constraints[i][j][k] = 0;
      }
    }
  }

  // zero out dram timers
  for (int i = 0; i < NUM_CONSECUTIVE_CMD_CONSTRAINTS; i++) {
    dram->consecutive_cmd_timers[i] = 0;
  }

  // zero out tfaw timers
  for (int i = 0; i < NUM_TFAW_COUNTERS; i++) {
    dram->tFAW_timers[i] = 0;
  }
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
      level_one_algorithm(dimm, q, clock);
      break;

    case LEVEL_2:
    case LEVEL_3:
      bank_level_parallelism(dimm, q, clock);
      break;

    default:
      break;
  }
}

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

void set_tfaw_counter(DIMM_t **dimm) {
  for (int i = 0; i < NUM_OF_TFAW_COUNTERS; i++) {
    if ((*dimm)->tFAW_counters[i] == 0) {
      (*dimm)->tFAW_counters[i] = TFAW + 1; // add one because it gets decrement on the same clk cycle it gets set
      break; // only want to set one counter at a time
    }
  }
}

void decrement_tfaw_counters(DIMM_t **dimm) {
  for (int i = 0; i < NUM_OF_TFAW_COUNTERS; i++) {
    if ((*dimm)->tFAW_counters[i] != 0) {
      (*dimm)->tFAW_counters[i]--;
    }
  }
}

bool can_issue_act(DIMM_t **dimm) {
  // if any counter is set to 0 then we can issue an ACT cmd
  // without violating the tFAW timing constraint
  for (int i = 0; i < NUM_OF_TFAW_COUNTERS; i++) {
    if ((*dimm)->tFAW_counters[i] == 0) {
      return true;
    }
  }

  return false;
}

int closed_page(DIMM_t **dimm, MemoryRequest_t *request, uint64_t cycle) {
  DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);
  char *cmd = NULL;

  // Set the initial state before processing the request
  LOG("cycle %llu, state %d \n", cycle,request->state );

  if (request->state == PENDING) {
    if (!can_issue_act(dimm)) {
        // TODO: handle this case
    }
    request->timer = cycle;
    request->state = ACT0;
    set_tfaw_counter(dimm);
  }

  decrement_tfaw_counters(dimm);

  // Process the request (one state per cycle)
  switch (request->state) {
    case ACT0:
      if (cycle - request->timer >= TRAS) {
        activate_bank(dram, request);
        cmd = issue_cmd("ACT0", request, cycle);
        request->timer = cycle; //update timer
        request->state++;
      }
      break;

    case ACT1:
      cmd = issue_cmd("ACT1", request, cycle);
      request->timer = cycle;
      request->state++;
      break;

    case RW0:
      if (cycle - request->timer >= TRCD){
        cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);
        request->timer = cycle; //update timer
        request->state++;
      }
      break;

    case RW1:
      cmd = issue_cmd(request->operation == DATA_WRITE ? "WR1" : "RD1", request, cycle);
      dram->bank_groups[request->bank_group].banks[request->bank].is_active = false;
      request->timer = cycle;
      request->state = PRE;  
      break;

    case PRE:
      if (cycle - request->timer >= TRP) {
        precharge_bank(dram, request);
        cmd = issue_cmd("PRE", request, cycle);
        request->timer = cycle; //update timer
        request->state = COMPLETE; 
      }
      break;

    case COMPLETE:
      break;

    case PENDING:
    default:
      fprintf(stderr, "Error: Unknown state encountered\n");
      return -1; 
  }

  // writing commands to output file
  if (cmd != NULL) {
    fprintf((*dimm)->output_file, "%s\n", cmd);
    free(cmd);
  }

  return 0;
}

int open_page(DIMM_t **dimm, MemoryRequest_t *request, uint64_t cycle) {
  DRAM_t *dram = &((*dimm)->channels[request->channel].DDR5_chip[0]);
  char *cmd = NULL;

  // Set the initial state before processing the request
  LOG("cycle %llu, state %d \n", cycle,request->state );

  if (request->state == PENDING) {

    if (is_page_hit(dram, request)) {
      request->state = RW0;

    } else if (is_page_miss(dram, request)) {
      dram->bank_groups[request->bank_group].banks[request->bank].is_active = false;
      request->state = PRE;

    } else if (is_page_empty(dram, request)) {
      if (!can_issue_act(dimm)) {
        // TODO: handle this case
      }
      request->state = ACT0;
      set_tfaw_counter(dimm);
    }
    request->timer = cycle;

  }

  decrement_tfaw_counters(dimm);

  // Process the request (one state per cycle)
  switch (request->state) {
    case PRE:
      if (cycle - request->timer >= TRP) {
        precharge_bank(dram, request);
        cmd = issue_cmd("PRE", request, cycle);
        request->timer = cycle; //update timer
        request->state = ACT0;
      }
      break;

    case ACT0:
      if (cycle - request->timer >= TRAS) {
        activate_bank(dram, request);
        cmd = issue_cmd("ACT0", request, cycle);
        request->timer = cycle; //update timer
        request->state++;
      }
      break;

    case ACT1:
      cmd = issue_cmd("ACT1", request, cycle);
      request->timer = cycle;
      request->state++;
      break;

    case RW0:
    
      if (cycle - request->timer >= TRCD){
        cmd = issue_cmd(request->operation == DATA_WRITE ? "WR0" : "RD0", request, cycle);
        request->timer = cycle; //update timer
        request->state++;
      }
      break;

    case RW1:
      cmd = issue_cmd(request->operation == DATA_WRITE ? "WR1" : "RD1", request, cycle);
      dram->bank_groups[request->bank_group].banks[request->bank].is_active = false;

      request->timer = cycle;
      request->state = COMPLETE;  
      break;

    case COMPLETE:
      break;

    case PENDING:
    default:
      fprintf(stderr, "Error: Unknown state encountered\n");
      return -1; 
  }

  // writing commands to output file
  if (cmd != NULL) {
    fprintf((*dimm)->output_file, "%s\n", cmd);
    free(cmd);
  }

  return 0;
}

int bank_level_parallelism(DIMM_t **dimm, MemoryRequest_t *request, uint64_t cycle) {
  
}

void out_of_order() {

}

void dram_init(DRAM_t *dram) {
  // Initialize the DRAM with all banks precharged
  for (int i = 0; i < NUM_BANK_GROUPS; i++) {
    for (int j = 0; j < NUM_BANKS_PER_GROUP; j++) {
      dram->bank_groups[i].banks[j].is_precharged = true;
      dram->bank_groups[i].banks[j].is_active = false;
      dram->bank_groups[i].banks[j].active_row = 0;
    }
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

void process_request(DIMM_t **dimm, Queue_t **q, uint64_t dimm_cycle, uint8_t scheduling_algorithm) {
  switch (scheduling_algorithm) {
    case LEVEL_0:
      MemoryRequest_t *dimm_request = queue_peek(*q);
      
      if (dimm_request && dimm_request->state != COMPLETE) {
        closed_page(dimm, dimm_request, dimm_cycle);
      }

      if (dimm_request && dimm_request->state == COMPLETE) {
        log_memory_request("Dequeued:", dimm_request, dimm_cycle);
        dequeue(q);
      }
      break;
    
    case LEVEL_1:
      break;

    case LEVEL_2:
      break;

    case LEVEL_3:
      break;
    
    default:
      break;
  }
}

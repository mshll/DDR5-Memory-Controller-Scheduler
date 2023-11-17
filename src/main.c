/**
 * @file    main.c
 * @author  ECE485/585 Team 5
 *          members:
 *            Abdulaziz Alateeqi,
 *            Eduardo Sanchez Simancas,
 *            Gene Hu,
 *            Meshal Almutairi
 * @brief   This program simulates a memory controller scheduler for a 12-core
 *          4.8 GHz processor employing a 16GB PC5-38400 DIMM
 * @version 0.1
 * @date    2023-11-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "dram.h"
#include "memory_request.h"
#include "queue.h"

/*** macro(s), enum(s), and struct(s) ***/
#define LINE_LENGTH 256
#define MAX_QUEUE_SIZE 16
#define DEFAULT_INPUT_FILE "trace.txt"
#define DEFAULT_OUTPUT_FILE "dram.txt"

enum CommandLine {
  NO_INPUT = 1,
  VALID_INPUT = 2
};

/*** function declaration(s) ***/
FILE *open_file(char *file_name, char *mode);
MemoryRequest_t parse_line(char *line);

int main(int argc, char *argv[]) {
  FILE *file;
  char *file_name;
  char line[LINE_LENGTH];

  if (argc == NO_INPUT) {
    file_name = DEFAULT_INPUT_FILE;
  } else if (argc == VALID_INPUT) {
    file_name = argv[1];
  } else {
    fprintf(stderr, "Usage: %s [file_name]\n", argv[0]);
    return 1;
  }

  file = open_file(file_name, "r");

  unsigned long long clock_cycle = 0;  // tracking the clock cycle (CPU clock). DIMM clock cycle is 1/2.
  DRAM_t *dram = NULL;
  queue_t *main_queue = NULL;

  dram_init(&dram);                           // initialize DRAM
  queue_create(&main_queue, MAX_QUEUE_SIZE);  // create queue of size 16

  while (fgets(line, sizeof(line), file)) {
    MemoryRequest_t memory_request = parse_line(line);

    // DIMM clock cycle (happens every 2 CPU clock cycles)
    if (clock_cycle % 2 == 0) {
      // TODO
      // process requests
      LOG("DIMM clock cycle: %llu\n", clock_cycle / 2);
    }

    // CPU clock cycle
    if (memory_request.time <= clock_cycle) {
      // TODO
      // add `memory_request` to the queue
    }

    LOG("CPU clock cycle: %llu\n", clock_cycle);
    LOG("Memory request: time = %5llu, core = %2u, operation = %u, row = %5u, "
        "column = %2u, bank = %2u, bank group = %2u, channel = %2u, byte "
        "select = %2u\n",
        memory_request.time,
        memory_request.core,
        memory_request.operation,
        memory_request.row,
        memory_request.column_high,
        memory_request.bank,
        memory_request.bank_group,
        memory_request.channel,
        memory_request.byte_select);

    clock_cycle++;  // increment clock cycle
  }

  fclose(file);
  return 0;
}

FILE *open_file(char *file_name, char *mode) {
  FILE *file;
  file = fopen(file_name, mode);
  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }
  return file;
}

MemoryRequest_t parse_line(char *line) {
  unsigned long long time, address;
  unsigned core, operation;
  MemoryRequest_t memory_request;

  sscanf(line, "%llu %u %u %llx", &time, &core, &operation, &address);
  memory_request_init(&memory_request, time, core, operation, address);

  LOG(
      "Parsed: time = %5llu, core = %2u, operation = %u, address = %#016llX\n",
      time,
      core,
      operation,
      address);

  return memory_request;
}

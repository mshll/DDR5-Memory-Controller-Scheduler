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
#include "dimm.h"
#include "memory_request.h"
#include "parser.h"
#include "queue.h"

/*** macro(s), enum(s), and struct(s) ***/
#define MAX_QUEUE_SIZE 16
#define DEFAULT_INPUT_FILE "trace.txt"
#define DEFAULT_OUTPUT_FILE "dram.txt"

enum CommandLine {
  NO_INPUT = 1,
  VALID_INPUT = 2
};

int main(int argc, char *argv[]) {
  char *file_name;

  if (argc == NO_INPUT) {
    file_name = DEFAULT_INPUT_FILE;
  } else if (argc == VALID_INPUT) {
    file_name = argv[1];
  } else {
    fprintf(stderr, "Usage: %s [file_name]\n", argv[0]);
    return 1;
  }

  Parser_t *parser = parser_init(file_name);

  unsigned long long clock_cycle = 0;  // tracking the clock cycle (CPU clock). DIMM clock cycle is 1/2.
  DIMM_t *PC5_38400 = NULL;
  Queue_t *global_queue = NULL;

  dimm_create(&PC5_38400);
  queue_create(&global_queue, MAX_QUEUE_SIZE);  // create queue of size 16

  MemoryRequest_t *request_buffer = NULL;

  while (true) {
    if (request_buffer == NULL && !queue_is_full(global_queue)) {
      request_buffer = parser_next_request(parser, clock_cycle);  // only returns the request if the current cycle >= request's time

      if (request_buffer) LOG("Request: %s\n", memory_request_to_string(request_buffer));
    }

    // DIMM clock cycle
    if (clock_cycle % 2 == 0 && !queue_is_empty(global_queue)) {
      // TODO
      MemoryRequest_t request = dequeue(&global_queue);
      LOG("Dequeued: %s\n", memory_request_to_string(&request));
      process_request(&PC5_38400, &request);
    }

    // CPU clock cycle
    if (request_buffer != NULL) {
      enqueue(&global_queue, *request_buffer);
      LOG("Enqueued: %s\n", memory_request_to_string(request_buffer));
      request_buffer = NULL;
    }

    if (parser->status == END_OF_FILE && queue_is_empty(global_queue)) {
      LOG("END OF SIMULATION\n");
      break;
    }

    clock_cycle++;  // increment clock cycle
  }

  parser_destroy(parser);
  return 0;
}

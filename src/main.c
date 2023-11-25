/**
 * @file    main.c
 * @author  ECE485/585 Team 5
 *          members:
 *            Abdulaziz Alateeqi,
 *            Meshal Almutairi,
 *            Gene Hu,
 *            Eduardo Sanchez Simancas
 *
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
  NO_ARGS = 1,
  INPUT_ONLY = 2,
  INPUT_OUTPUT = 3,
};

/*** function prototype(s) ***/
void process_args(int argc, char *argv[], char **input_file, char **output_file);

/*** function(s) ***/
int main(int argc, char *argv[]) {
  char *input_file_name = NULL;
  char *output_file_name = NULL;
  process_args(argc, argv, &input_file_name, &output_file_name);

  Parser_t *parser = parser_init(input_file_name);
  DIMM_t *PC5_38400 = NULL;
  Queue_t *global_queue = NULL;

  dimm_create(&PC5_38400, output_file_name);    // create DIMM
  queue_create(&global_queue, MAX_QUEUE_SIZE);  // create queue of size 16

  unsigned long long clock_cycle = 0;  // tracking the clock cycle (CPU clock). DIMM clock cycle is 1/2.
  MemoryRequest_t *current_request = NULL;

  while (true) {
    if (current_request == NULL) {
      current_request = parser_next_request(parser, clock_cycle);  // only returns the request if the current cycle >= request's time
    }

    // DIMM clock cycle - only process request if there is one in the queue
    if (clock_cycle % 2 == 0 && !queue_is_empty(global_queue)) {
      process_request(&PC5_38400, &global_queue, clock_cycle, LEVEL_0);
    }

    // CPU clock cycle - enqueue if there is a request and queue is not full
    if (current_request != NULL && !queue_is_full(global_queue)) {
      enqueue(&global_queue, *current_request);
      log_memory_request("Enqueued:", current_request, clock_cycle);
      free(current_request);
      current_request = NULL;
    }

    if (parser->status == END_OF_FILE) {
      LOG("END OF SIMULATION\n");
      break;
    }

    clock_cycle++; 
  }

  if (!queue_is_empty(global_queue)) {
    LOG("ERROR: Queue is not empty by EOF\n");
  }

  parser_destroy(parser);
  queue_destroy(&global_queue);
  dimm_destroy(&PC5_38400);
  return 0;
}

void process_args(int argc, char *argv[], char **input_file, char **output_file) {
  switch (argc) {
    case NO_ARGS:
      *input_file = DEFAULT_INPUT_FILE;
      *output_file = DEFAULT_OUTPUT_FILE;
      break;
    case INPUT_ONLY:
      *input_file = argv[1];
      *output_file = DEFAULT_OUTPUT_FILE;
      break;
    case INPUT_OUTPUT:
      *input_file = argv[1];
      *output_file = argv[2];
      break;
    default:
      fprintf(stderr, "Usage: %s [input_file] [output_file]\n", argv[0]);
      exit(1);
  }
  LOG("Input file: %s\n", *input_file);
  LOG("Output file: %s\n", *output_file);
}
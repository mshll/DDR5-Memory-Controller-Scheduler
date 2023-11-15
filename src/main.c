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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "queue.h"
#include "common.h"

/*** macro(s), enum(s), and struct(s) ***/
#define LINE_LENGTH 256

int i = 0; // tracking how many operation we put in the queue

enum CommandLine {
  NO_INPUT = 1,
  VALID_INPUT = 2
};

/*** function declaration(s) ***/
void parse_line(char *line, struct Queue *queue); //added our queue

int main(int argc, char *argv[]) {
  FILE *file;
  char *file_name;
  char *default_file = "trace.txt";
  char line[LINE_LENGTH];

  if (argc == NO_INPUT) {
    file_name = default_file;
  } else if (argc == VALID_INPUT) {
    file_name = argv[1];
  } else {
    fprintf(stderr, "Usage: %s [file_name]\n", argv[0]);
    return 1;
  }

  file = fopen(file_name, "r");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }
  struct Queue *queue = create_queue(); // creating our queue, 16 is the default size
  while (fgets(line, sizeof(line), file) && i < 16 ) { // can use  ~(isfull())
    parse_line(line, queue);
    i++; //increment process counter
  }
  #ifdef DEBUG
  print_queue(queue); //prints the entire queue
  #endif

  fclose(file);
  return 0;
}

void parse_line(char *line, struct Queue *queue) { //added the struct
  uint64_t time = 0;
  uint32_t core = 0, operation = 0;
  uint64_t address = 0;
  struct Process data;
 
  sscanf(line, "%lu %d %d %lx", &data.time, &data.core, &data.operation, &data.address);
  enqueue(queue, data);
  LOG_DEBUG(
      "Parsed: time = %5lu, core = %2d, operation = %d, address = %#016lX\n",
      time,
      core,
      operation,
      address);
}

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/*** macro(s), enum(s), and struct(s) ***/
#define LINE_LENGTH 256

#ifdef DEBUG
#define LOG_DEBUG(format, ...) printf("%s:%d: " format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(...) /*** expands to nothing ***/ 
#define LOG(...) /*** expands to nothing ***/
#endif

enum command_line {
  NO_INPUT  = 1,
  ONE_INPUT = 2
};

/*** function declaration(s) ***/
void parse_line(char *line);

int main(int argc, char *argv[]) {
  FILE *file;
  char *file_name;
  char *default_file = "trace.txt";
  char line[LINE_LENGTH];

  if (argc == NO_INPUT) {
    file_name = default_file;
  }

  if (argc == ONE_INPUT) {
    file_name = argv[1];
  }

  file = fopen(file_name, "r");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  while (fgets(line, sizeof(line), file)) {
    parse_line(line);
  }

  fclose(file);
  return 0;
}

void parse_line(char *line) {
  unsigned long time;
  int core, operation;
  unsigned long long address;

  sscanf(line, "%lu %d %d %llx", &time, &core, &operation, &address);

  LOG_DEBUG(
    "Parsed: time = %5lu, core = %2d, operation = %d, address = %016llX\n", 
    time, 
    core, 
    operation, 
    address
  );

}

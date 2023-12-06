/**
 * @file  parser.c
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "parser.h"

/** helper function(s) **/
FILE *open_file(char *file_name, char *mode);
void parser_next_line(Parser_t *parser);
MemoryRequest_t parse_line(char *line);

Parser_t *parser_init(char *input_file) {
  Parser_t *parser = malloc(sizeof(Parser_t));

  if (parser == NULL) {
    perror("Error allocating memory for parser");
    exit(EXIT_FAILURE);
  }

  parser->file = open_file(input_file, "r");
  parser->next_request = NULL;

  parser_next_line(parser);

  return parser;
}

void parser_destroy(Parser_t *parser) {
  if (parser != NULL) {
    fclose(parser->file);
    free(parser);
  }
}

MemoryRequest_t *parser_next_request(Parser_t *parser, uint64_t cycle) {
  if (parser->status == OK && parser->next_request->time <= cycle) {
    MemoryRequest_t *request = parser->next_request;
    parser_next_line(parser);
    return request;
  }

  return NULL;
}

FILE *open_file(char *file_name, char *mode) {
  FILE *file = fopen(file_name, mode);
  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  return file;
}

void parser_next_line(Parser_t *parser) {
  if (fgets(parser->line, sizeof(parser->line), parser->file)) {
    // skip empty lines
    if (strlen(parser->line) == 1) {
      parser_next_line(parser);
      return;
    }

    parser->next_request = malloc(sizeof(MemoryRequest_t));

    if (parser->next_request == NULL) {
      perror("Error allocating memory for next request");
      exit(EXIT_FAILURE);
    }

    *parser->next_request = parse_line(parser->line);
    parser->status = OK;

  } else {
    parser->status = END_OF_FILE;
  }
}

MemoryRequest_t parse_line(char *line) {
  char time_str[21], core_str[4], operation_str[3], address_str[21];
  uint64_t time, address;
  uint8_t core, operation;
  MemoryRequest_t memory_request;

  if (sscanf(line, "%20s %3s %2s %20s", time_str, core_str, operation_str, address_str) != 4) {
    fprintf(stderr, "Error parsing line: '%s'\n", line);
    exit(EXIT_FAILURE);
  }

  // Check for negative numbers
  if (time_str[0] == '-' || core_str[0] == '-' || operation_str[0] == '-' || address_str[0] == '-') {
    fprintf(stderr, "Error: Negative number detected in input: '%s'\n", line);
    exit(EXIT_FAILURE);
  }

  time = strtoull(time_str, NULL, 10);
  core = strtoul(core_str, NULL, 10);
  operation = strtoul(operation_str, NULL, 10);
  address = strtoull(address_str, NULL, 16);

  // Check if core is out of range
  if (core > 11) {
    fprintf(stderr, "Error: core value out of range (0-11): %u\n", core);
    exit(EXIT_FAILURE);
  }

  // Check if operation is out of range
  if (operation > 2) {
    fprintf(stderr, "Error: operation value out of range (0-2): %u\n", operation);
    exit(EXIT_FAILURE);
  }

  // Check if address is more than 34 bits
  if (address > ((uint64_t)1 << 34) - 1) {
    fprintf(stderr, "Error: address is more than 34 bits: %" PRIx64 "\n", address);
    exit(EXIT_FAILURE);
  }

  memory_request_init(&memory_request, time, core, operation, address);

  // Check if channel is out of range
  if (memory_request.channel != 0) {
    fprintf(stderr, "Error: request at time %" PRIu64 " has channel %u != 0\n", memory_request.time, memory_request.channel);
    exit(EXIT_FAILURE);
  }

  // LOG("Parsed: time = %5" PRIu64 ", core = %2u, operation = %u, address = %#016llX\n", time, core, operation, address);

  return memory_request;
}

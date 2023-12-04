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
  uint64_t time, address;
  uint8_t core, operation;
  MemoryRequest_t memory_request;

  sscanf(line, "%" PRIu64 " %hhu %hhu %llx", &time, &core, &operation, &address);
  memory_request_init(&memory_request, time, core, operation, address);

  // LOG("Parsed: time = %5" PRIu64 ", core = %2u, operation = %u, address = %#016llX\n", time, core, operation, address);

  return memory_request;
}

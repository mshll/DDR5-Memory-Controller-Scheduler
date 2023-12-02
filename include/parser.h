/**
 * @file  parser.h
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include "common.h"
#include "memory_request.h"

#define LINE_LENGTH 256

typedef enum ParserStatus {
  OK,
  ERROR,
  END_OF_FILE,
} ParserStatus_t;

typedef struct Parser {
  FILE *file;
  char line[LINE_LENGTH];
  MemoryRequest_t *next_request;
  ParserStatus_t status;
} Parser_t;

/**
 * @brief Initialize the parser.
 *
 * @param input_file  The input file name
 * @return Parser_t*  The parser
 */
Parser_t *parser_init(char *input_file);

/**
 * @brief Destroy parser and free memory.
 *
 * @param parser  The parser
 */
void parser_destroy(Parser_t *parser);

/**
 * @brief Get the next request from the parser if the current cycle is greater than the request's time.
 *
 * @param parser  The parser
 * @param cycle  The current cpu cycle
 * @return MemoryRequest_t*  The memory request
 */
MemoryRequest_t *parser_next_request(Parser_t *parser, uint64_t cycle);

#endif
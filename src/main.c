#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LENGTH 256

void parse_line(char *line) {
  unsigned long time;
  int core, operation;
  unsigned long long address;

  sscanf(line, "%lu %d %d %llx", &time, &core, &operation, &address);

#if DEBUG
  printf("Parsed: time = %5lu, core = %2d, operation = %d, address = %llx\n", time, core, operation, address);
#endif
}

int main(int argc, char *argv[]) {
  FILE *file;
  char *filename = "trace.txt";
  char line[LINE_LENGTH];

  if (argc > 1) {
    filename = argv[1];
  }

  file = fopen(filename, "r");
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

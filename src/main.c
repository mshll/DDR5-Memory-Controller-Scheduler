#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define LINE_LENGTH 256

#ifdef DEBUG
#define LOG_DEBUG(format, ...) printf("%s:%d: " format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(...) /*** expands to nothing ***/ 
#define LOG(...) /*** expands to nothing ***/
#endif

void parse_line(char *line) {
  unsigned long time;
  int core, operation;
  unsigned long long address;

  sscanf(line, "%lu %d %d %llx", &time, &core, &operation, &address);

  LOG(
    "Parsed: time = %5lu, core = %2d, operation = %d, address = %llx\n", 
    time, 
    core, 
    operation, 
    address
  );

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

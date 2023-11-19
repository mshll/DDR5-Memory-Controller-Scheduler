/** 
 * trace_script.c
 * ECE585 Team 5
 *
 * Used to generate a trace file for test case use
*/

// Includes and Defines
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_FILE_NAME "trace.txt"
#define STRING_MAX 256
// #define DEBUG

// Prototypes
unsigned int set_value(
    unsigned int w, 
    int i, 
    int j, 
    unsigned int v
);

unsigned int get_value(unsigned int w, int i, int j);

void obtaining_line(
    FILE *fp
);
void writing_line(
    int val[], 
    FILE *fp
);

// Number of command line args followed by list of command line args
int main() {
    FILE *fp;
    char trace_name[STRING_MAX] = DEFAULT_FILE_NAME;

    // Ask for trace file name
    printf("Enter trace file name\n");
    scanf("%s", trace_name);
    #ifdef DEBUG
    printf("the trace file name is: %s\n", trace_name);
    #endif

    if((fp = fopen(trace_name, "w")) == NULL) {
        printf("Error opening or creating file\n");
        exit(1);
    }
    
    obtaining_line(fp);

    #ifdef DEBUG
        printf("Made it out of the function\n");
    #endif

    fclose(fp);
    return 0;
}

// Functions


// set value of bit field w[j:i] to v
unsigned int set_value(
    unsigned int w, 
    int i, 
    int j, 
    unsigned int v
) {
    return((w & ~(((1 << (j-i+1)) -1) << i)) | (v << i));
}

// return value of bit field from i to j  (w[j:i])
unsigned int get_value(
    unsigned int w, 
    int i, 
    int j
) {
    return((w >> i) & ((1 << (j-i+1)) -1));
}

/**
 * @brief Going to ask user for values to input into line of trace file
 * 
 * @param fp file pointer to trace file
 */
void obtaining_line (
    FILE *fp
) {
    // from index 0 to 6: time, core, operation, bank, bank group, row, column
    int input_vals[7];
    char str[STRING_MAX];
    char *token;
    char check;

    // Give input format
    printf("\nEnter input line in following format (all positive integers):\n\n"
           "time core operation bank bank_group row column\n\n");

    fflush(stdin);
    // Honestly don't know if all these fflush is necessary, but added them just in case
    while(fgets(str, sizeof(str), stdin) != NULL) {
        token = strtok(str, " ");
        int k = 0;
        while(token != NULL && (k < 7)) {
            input_vals[k] = atoi(token);

            #ifdef DEBUG
            printf("input_value[%d]: %d\n token:%s\n", k, input_vals[k], token);
            #endif

            token = strtok(NULL, " ");
            k++;
        }

        printf("You want to input the following values?\n"
               "time:       %d\n"
               "core:       %d\n"
               "operation:  %d\n"
               "bank:       %d\n"
               "bank group: %d\n"
               "row:        %d\n"
               "column:     %d\n"
               "Respond y/n to write into trace file\n", 
               input_vals[0],input_vals[1],input_vals[2],input_vals[3],
               input_vals[4],input_vals[5],input_vals[6]);

        fflush(stdin);
        check = getchar();
        if (check == 'y')
            writing_line(input_vals, fp);

        printf("Do you want to keep inputting lines, y/n?\n");
        fflush(stdin);
        check = getchar();
        if (check == 'n')
            break;
        fflush(stdin);

        printf("\nEnter input line in following format (all positive integers):\n\n"
               "time core operation bank bank_group row column\n\n");
        fflush(stdin);
    }
}

/**
 * @brief Write the values in val[] as a line into trace file in format
 *        time, core, operation, address
 *        address = row[33:18], col_h[17:12], BA[11:10], BG[9:7], channel[6], col_l[5:2], offset[1:0]
 * @param val array containing: (0)time, (1)core, (2)operation, (3)bank, (4)bank_group, (5)row, (6)column
 * @param fp file pointer to trace file
 */
void writing_line (
    int val[], 
    FILE *fp
) {
    unsigned int col_h, col_l;
    // Doing this for clarity on what the operations are working with
    unsigned int row = val[5], BA = val[3], BG = val[4];
    unsigned long long int address = 0x0;

    col_h = get_value(val[6], 4, 9);
    col_l = get_value(val[6], 0, 3);

    address = set_value(address, 18, 33, row);
    #ifdef DEBUG
        printf("setted row: %X\n", address);
    #endif
    address = set_value(address, 12, 17, col_h);
    #ifdef DEBUG
        printf("setted col_h: %X\n", address);
    #endif
    address = set_value(address, 10, 11, BG);
    #ifdef DEBUG
        printf("setted BG: %X\n", address);
    #endif
    address = set_value(address, 7, 9, BA);
    // Bit 6 is the channel bit, and we will only be working in channel 0
    // WARNING, pretty sure 8-byte aligned means the LSB of col should always be 0, but I am including for now until I get more confirmation 
    address = set_value(address, 2, 5, col_l);
    // Bit 0 and 1 are the offset which should stay 0

    fprintf(fp, "%8d %3d %2d 0x%llX\n", val[0], val[1], val[2], address);
}


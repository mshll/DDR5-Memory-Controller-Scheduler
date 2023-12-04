# ECE485/585 Final Project: Memory Controller Simulator

Team Members: Abdulaziz Alateeqi, Meshal Almutairi, Gene Hu, Eduardo Sanchez

## Project Overview

### Description
The project involves simulating the scheduler portion of a memory controller for a 12-core, 4.8 GHz processor using a single 16GB PC5-38400 DIMM. The simulation uses a relaxed consistency model. The DIMM configuration includes x8 devices with 1KB page size, 40-39-39-76 timing, 8 bank groups of 4 banks each, and employs 1N mode for two-cycle commands.

### Objectives
- Correctly schedule DRAM commands as per DDR5 timing.
- Handle input trace files specifying time, core, operation, and address.
- Generate a text file trace of all DRAM commands issued.
- Implement and simulate various scheduling algorithms.


## Getting Started

### Compiling the Program
- **Default**: Use `make` to compile the program with the standard configuration.
- **Debug**: Use `make debug` to compile the program with additional debugging information

> **Note**: You may need to run `make clean` before compiling with a different configuration.


### Running the Program
To run the program, use the following command:
```
./bin/main [-i input_file] [-o output_file] [-s scheduling_policy]
```

Where:
- `input_file` is the input file. If not specified, the program will default to `trace.txt`.
- `output_file` is the output file. If not specified, the program will default to `dram.txt`.
- `scheduling_policy` is the scheduling policy level to use (`0-3`). If not specified, the program will default to `0`.

Schedule Policy Levels:
- `0`: No bank-level parallelism, closed page policy
- `1`: No bank-level parallelism, open page policy
- `2`: Bank-level parallelism, open page policy
- `3`: Bank-level parallelism, open page policy, out-of-order scheduling

#### Example
```
./bin/main -i trace.txt -o out.txt -s 3
```

### Input File Format
The input file should be a text file with each line containing a memory request. Each line should follow the format:
```
<time> <core> <operation> <address>
```

Where:
- `time` is the time in CPU clock cycles.
- `core` is the core number between `0` and `11`.
- `operation` is the operation type (`0` for data read, `1` for data write, `2` for instruction fetch).
- `address` is an 8-byte aligned address in hexadecimal representation.

#### Example Input Format
```
30 0 2 01FF97080
31 1 2 10FFFFF00
32 2 0 10FFFFF80
40 0 1 01FF97000
```

### Output File Format
The output file will be a text file with each line containing a DRAM command. Each line will follow the format:
```
<time> <channel> <command>
```

Where:
- `time` is the time in DIMM clock cycles.
- `channel` is the channel number between `0` and `1`.
- `command` is the command type and its parameters.

#### Example Output Format
```
100 0 PRE   0 0
200 0 ACT0  0 0 03FF
204 0 ACT1  0 0 03FF
300 0 RD0   0 0 EF
304 0 RD1   0 0 EF
```

## Topological Address Mapping
The following table shows the topological address mapping for the DIMM configuration used in this project.
<div><table>
<tbody>
  <tr><td>33<td>32<td>31<td>30<td>29<td>28<td>27<td>26<td>25<td>24<td>23<td>22<td>21<td>20<td>19<td>18<td>17<td>16<td>15<td>14<td>13<td>12<td>11<td>10<td>9<td>8<td>7<td>6<td>5<td>4<td>3<td>2<td>1<td>0</td></tr>
  <tr>
    <td colspan="16" rowspan="2">Row</td>
    <td colspan="6" rowspan="2">Column [9:4]</td>
    <td colspan="2" rowspan="2">Bank</td>
    <td colspan="3" rowspan="2">Bank Group</td>
    <td rowspan="2">Channel</td>
    <td colspan="4" rowspan="2">Column [3:0]</td>
    <td colspan="2" rowspan="2">Byte Select</td>
  </tr>
</tbody>
</table></div>


## Design Overview

### Data Structures
The following data structures are used in the program:
- `MemoryRequest_t`: Contains the information for a single memory request along with its current state.
- `Queue_t`: Contains a doubly linked list and the size of the queue.
- `Parser_t`: Contains the file pointer, the current line, the next memory request, and the current status of the parser.
- `Bank_t`: Contains the state of a single bank.
- `BankGroup_t`: Contains an array of banks.
- `DRAM_t`: Contains an array of bank groups, timing constraints, timers, and the last bank group and interface command.
- `Channel_t`: Contains an array of DRAM chips.
- `DIMM_t`: Contains an array of channels and the output file pointer.

### Queue
The queue is implemented as a doubly linked list. The queue is used to store memory requests that are ready to be issued.

### Parser
The parser is responsible for reading the input file and parsing the lines into memory requests. The parser provides the next memory request when requested if the memory request is ready to be issued.

### DIMM
The DIMM is responsible for processing memory requests based on the scheduling policy and issuing the appropriate DRAM commands.

### Processing Memory Requests
The program uses a queue to store memory requests that are ready to be issued. Memory requests use a finite state machine to track their current state and transition to the next state when the appropriate conditions are met.


## Testing
See [tests/Test_Plan_Outline.md](tests/Test_Plan_Outline.md) for more information on testing.

## Coding Conventions
See [CONTRIBUTING.md](CONTRIBUTING.md) for more information on coding conventions.
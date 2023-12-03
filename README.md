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


## Topological Address Mapping
The following table shows the topological address mapping for the DIMM configuration used in this project.
<div><table>
<tbody>
  <tr>
    <td>33</td>
    <td>32</td>
    <td>31</td>
    <td>30</td>
    <td>29</td>
    <td>28</td>
    <td>27</td>
    <td>26</td>
    <td>25</td>
    <td>24</td>
    <td>23</td>
    <td>22</td>
    <td>21</td>
    <td>20</td>
    <td>19</td>
    <td>18</td>
    <td>17</td>
    <td>16</td>
    <td>15</td>
    <td>14</td>
    <td>13</td>
    <td>12</td>
    <td>11</td>
    <td>10</td>
    <td>9</td>
    <td>8</td>
    <td>7</td>
    <td>6</td>
    <td>5</td>
    <td>4</td>
    <td>3</td>
    <td>2</td>
    <td>1</td>
    <td>0</td>
  </tr>
  <tr>
    <td colspan="16" rowspan="2">Row</td>
    <td colspan="6" rowspan="2">Column [9:4]</td>
    <td colspan="2" rowspan="2">Bank</td>
    <td colspan="3" rowspan="2">Bank Group</td>
    <td rowspan="2">Channel</td>
    <td colspan="4" rowspan="2">Column [3:0]</td>
    <td colspan="2" rowspan="2">Byte Select</td>
  </tr>
  <tr>
  </tr>
</tbody>
</table></div>


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

### Example
```
./bin/main -i trace.txt -o out.txt -s 3
```

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
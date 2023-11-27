# Test Plan Document <!-- omit in toc -->
- [1. INTRODUCTION](#1-introduction)
  - [1.1. Project](#11-project)
  - [1.2. PROJECT DESCRIPTION](#12-project-description)
  - [1.3. TEST STRATEGY / METHODOLOGY](#13-test-strategy--methodology)
- [2. INPUT VALIDIFICATION](#2-input-validification)
- [3. QUEUE REQUESTS](#3-queue-requests)
- [4. POLICY IMPLEMENTED CORRECTLY](#4-policy-implemented-correctly)
  - [4.1. LEVEL 0](#41-level-0)
    - [4.1.1. CLOSED PAGE POLICY](#411-closed-page-policy)
    - [4.1.2. IN-ORDER SCHEDULING](#412-in-order-scheduling)
  - [4.2. LEVEL 1](#42-level-1)
    - [4.2.1. OPEN PAGE POLICY](#421-open-page-policy)
  - [4.3. LEVEL 2](#43-level-2)
    - [4.3.1. BANK LEVEL PARALLELISM](#431-bank-level-parallelism)
  - [4.4. LEVEL 3](#44-level-3)
    - [4.4.1. OUT-OF-ORDER SCHEDULING](#441-out-of-order-scheduling)
- [5. DRAM COMMANDS FUNCTIONALLY CORRECT](#5-dram-commands-functionally-correct)
  - [5.1. READ0, READ1](#51-read0-read1)
  - [5.2. WRITE0, WRITE1](#52-write0-write1)
  - [5.3. ACTIVATE0, ACTIVATE1](#53-activate0-activate1)
  - [5.4. PRECHARGE](#54-precharge)
- [6. DRAM TIMING CORRECT](#6-dram-timing-correct)
  - [6.1. tRCD](#61-trcd)
  - [6.2. tRTP](#62-trtp)
  - [6.3. tCL](#63-tcl)
  - [6.4. tRAS](#64-tras)
  - [6.5. tRP](#65-trp)
  - [6.6. tRRD\_S and tRRD\_L](#66-trrd_s-and-trrd_l)
  - [6.7. tFAW](#67-tfaw)
  - [6.8. tWR](#68-twr)
  - [6.9. tCCD\_S and tCCD\_L](#69-tccd_s-and-tccd_l)
  - [6.10. tCCD\_S\_WR and tCCD\_L\_WR](#610-tccd_s_wr-and-tccd_l_wr)
  - [6.11. tCCD\_S\_RTW and tCCD\_L\_RTW](#611-tccd_s_rtw-and-tccd_l_rtw)
  - [6.12. tCCD\_S\_WTR and tCCD\_L\_WTR](#612-tccd_s_wtr-and-tccd_l_wtr)


## 1. INTRODUCTION

### 1.1. Project

**Project Name:** 
>Team 05 ECE485/585 Final Project

**Last Updated:** 
>11/26/2023

### 1.2. PROJECT DESCRIPTION

See final_project_description.pdf under docs for detailed explanation

**Important Infomation**
>For extra credit this project team will add bank level parallelism, open page policy, and out of order scheduling. 

### 1.3. TEST STRATEGY / METHODOLOGY

Create a trace file as an input (ASCII text file) using a test case generator.
1) Test case generator will be in program(Python) or Excel spreadsheet
   - Enter {time, request, row, bank, bank group, col}
   - Macro generates request (combining fields into address)
     - **REMINDER**: The input's address need to be hexadecimal and 8-byte aligned
2) Test cases can be commented for documentation (will be removed by pre-processor)

## 2. INPUT VALIDIFICATION
>Input should be in the format: time, core, operation, address. Where time is in (absolute) CPU clocks, core is between 0 to 11. operation is 0, 1, or 2, and address is 34-bit address represented in hexadecimal and it is 8-byte aligned.

**HOW:**

- Address can be tested by making all of the one parameter's bits all 1's. For example, to test that the parsing is working for ROW address, make ROW=65535 and everything else 0. If the output shows ROW!=65535 and another parameter!=0, then the input is not being taken in correctly. Repeat for BA,BG,COL. Checkpoint2 shows examples of this. 
- Error checking should be made for time, core, and operation. 


## 3. QUEUE REQUESTS
>Only 16 outstanding requests are allowed in the simulation at a time. If there are more than 16 requests in the queue when a new request comes in, that new request will not enter the queue until an item is completed to free up a spot. A request will only be completed and removed from the queue when its last command is executed (ex. read request will leave once the READ command is completed; Total timing: tCL + tBURST -> request is done).

## 4. POLICY IMPLEMENTED CORRECTLY

### 4.1. LEVEL 0
#### 4.1.1. CLOSED PAGE POLICY
>Page is closed after a read/write command. Thus a precharge must always follow those two commands. Every page reference will page empty. 

**Test Cases**:
| \#  | OBJECTIVE | INPUT | EXPECTED RESULTS | Notes |
| --- | --------- | ----- | ---------------- | ----- |
|  1  | Test page empty on successive references. | Back to back references to same BG,B, same row, different column. | ACT -> READ -> PRE -> ACT -> READ -> PRE | Make the requests be reads |
|  2  | Repeat test 1, except make the requests writes. | Back to back references to same BG,B, same row, different column. | ACT -> READ -> PRE -> ACT -> READ -> PRE |

#### 4.1.2. IN-ORDER SCHEDULING
>Complete the first item in the queue before moving onto the next item. 

### 4.2. LEVEL 1
#### 4.2.1. OPEN PAGE POLICY
>Page is left open for a moment after a read/write command. It will stay open until a read/write is referencing its bank, bank group, but different row (page miss). This means page misses will incur a precharge before activate, and page hits do not need activate (but will need to keep track of tCCD).

**Test Cases**:
| \#  | OBJECTIVE | INPUT | EXPECTED RESULTS | Notes |
| --- | --------- | ----- | ---------------- | ----- |
|  1  | Test page hit. | Back to back references to same BG,B, same row, different column. | ACT -> READ -> READ |
|  2  | Test page miss followed by page hit. | Back to back references to same BG,B, different rows, and then second row gets referenced again. | ACT -> READ -> PRE -> ACT -> READ -> READ | 
|  3  | Test open page tracking when interlearving BG,B. | Back to back references that are intersected by a page access from a different BG,B, with different ROW. | ACT -> ACT -> READ -> READ -> READ | LEVEL 2+ only |
|  4  | Test open page tracking when intervleaving BG,B, while trying to trick it by making the ROW be the same. | Back to back references that are intersected by a page access from a different BG,B, with same ROW. | ACT -> ACT -> READ -> READ -> READ | Should be same as test 3 |

### 4.3. LEVEL 2
#### 4.3.1. BANK LEVEL PARALLELISM
>Scheduler is able to interleave commands to different banks. For example, after it issues an activate command to B0,BG0, it can issue another activate to a different bank before coming back and issuing the READ command.

**CASES**
1. When we are waiting for timing contraints on a command being executed in a bank, and there is a request to a different bank in the queue, start the new request. Repeat. 
   - Allowed to skip over requests that are waiting for a bank to be available (technically out of order)

### 4.4. LEVEL 3
#### 4.4.1. OUT-OF-ORDER SCHEDULING
>Scheduler is able to process requests out-of-order. Must incorporate "aging"/timeout so a request isn't sitting in queue forever.

**CASES**
1. READ over WRITES
   - Only when the addresses are different. If this is done when the addresses are the same, it will lead to reading stale data. 
2. Prioritize page hits over page misses

## 5. DRAM COMMANDS FUNCTIONALLY CORRECT
Note: In level 2+, the scheduler will always pick READ1, WRITE1, and ACT1 if they are available. This project is using 1n mode, so there is a 1 cycle delay between 0 -> 1. 

### 5.1. READ0, READ1
>READ0 selects the command being issued (due to MUX) and READ1 will get column address. Followed by tCL = 40. Output should show the bank, bank group, and column being accessed (same as the input column address for this project).

### 5.2. WRITE0, WRITE1
>WRITE0 selects the command being issued (due to MUX) and WRITE1 will get the column address. Followed by tCL = 40. Output should show the bank, bank group, and column being accessed (same as the input column address for this project).

### 5.3. ACTIVATE0, ACTIVATE1
>ACTIVATE0 selects the command being issued (due to MUX) and ACTIVATE1 will get the row address. Followed by tRCD = 39. Output should show the bank, bank group, and row being accessed.

### 5.4. PRECHARGE
>If a page is open, the data needs to return from the sense amplifiers so another activate can occur. In a closed page policy, every READ/WRITE command should be followed by PRE. In an open page policy, PRE will occur when requested row is not already open. Followed by tRP = 39. tRAS and tRTP must be satisfied before it can be issued. Output should show the bank, bank group being precharged. 

## 6. DRAM TIMING CORRECT

### 6.1. tRCD
>tRCD = 39. Cycles to open a closed row. Occurs between ACT -> READ/WRITE.

### 6.2. tRTP
>tRTP = 18. Read to precharge delay. Occurs between READ -> PRECHARGE. This means a PRE can be done before we start reading out data, but we still need to wait for tRAS to be satisfied

### 6.3. tCL
>tCL = 40. Cycles to send memory the column address. Occurs between READ -> DATA BURST.

### 6.4. tRAS
>tRAS = 76. Amount of cycles needed between ACT -> PRE. By doing the math from the previously mentioned timings, in level 0 we are able to issue the precharge command before tCL is satisfied and tRTP is satisfied. This should save 9 cycles (79 - 76 - 1 - 1 + tBURST). This won't be done in level 1+ unless we have foresight on the next request being page miss or page hit. 

### 6.5. tRP
>tRP = 39. Cycles to close an open row. Occurs between PRE -> ACT.

### 6.6. tRRD_S and tRRD_L
>tRRD_S = 8, tRRD_L = 12. When switching between bank groups use _S, switching inside a bank group use _L. Timing between back to back ACTIVATES. Level 2+ only. 

### 6.7. tFAW
>tFAW = 32. Amount of time needed between every 4 ACT commands. 

### 6.8. tWR
>tWR = 30. Write recovery time. 

### 6.9. tCCD_S and tCCD_L
>tCCD_S = 8, tCCD_L = 12. Read to read command delay. When switching between bank groups use _S, switching inside a bank group use _L. Can only occur in open page policy. Level 1+ only.

### 6.10. tCCD_S_WR and tCCD_L_WR
>tCCD_S_WR = 8, tCCD_L_WR = 48. Write to write command delay. _S for different bank group, _L for same bank group. 

### 6.11. tCCD_S_RTW and tCCD_L_RTW
>tCCD_S_RTW = 16, tCCD_L_RTW = 16. Read to write command delay. _S for different bank group, _L for same bank group. 

### 6.12. tCCD_S_WTR and tCCD_L_WTR
>tCCD_S_WTR = 70, tCCD_L_WTR = 52. Write to read command delay. _S for different bank gropu, _L for same bank in same bank group. 

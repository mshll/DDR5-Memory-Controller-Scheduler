# Test Plan Document <!-- omit in toc -->
- [1. INTRODUCTION](#1-introduction)
  - [1.1. PROJECT](#11-project)
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
  - [6.3. tWR](#63-twr)
  - [6.4. tCL](#64-tcl)
  - [6.5. tRAS](#65-tras)
  - [6.6. tRP](#66-trp)
  - [6.7. tFAW](#67-tfaw)
  - [6.8. tRRD\_S and tRRD\_L](#68-trrd_s-and-trrd_l)
  - [6.9. tCCD\_S and tCCD\_L](#69-tccd_s-and-tccd_l)
  - [6.10. tCCD\_S\_WR and tCCD\_L\_WR](#610-tccd_s_wr-and-tccd_l_wr)
  - [6.11. tCCD\_S\_RTW and tCCD\_L\_RTW](#611-tccd_s_rtw-and-tccd_l_rtw)
  - [6.12. tCCD\_S\_WTR and tCCD\_L\_WTR](#612-tccd_s_wtr-and-tccd_l_wtr)
- [copy paste thingy, remove later](#copy-paste-thingy-remove-later)


## 1. INTRODUCTION

### 1.1. PROJECT

**Project Name:** 
>Team 05 ECE485/585 Final Project

**Members:**
- Abdulaziz Alateeqi	
- Meshal Almutairi	
- Eduardo Simancas	
- Gene Hu

**Last Updated:** 
>11/27/2023

### 1.2. PROJECT DESCRIPTION

See final_project_description.pdf under docs for detailed explanation.

**Scheduling Aggressiveness**
>For extra credit this project team will _try_ to add open page policy, bank level parallelism, and out of order scheduling. 

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
|  1  | Test page empty on successive references. | Back to back references to same BG,BA, same row, different column. | ACT -> READ -> PRE -> ACT -> READ -> PRE | Read or write should not matter |
|  2  | Test page empty on two different references. | Back to back references to different BG,B,ROW. | ACT -> READ -> PRE -> ACT -> READ -> PRE | 

#### 4.1.2. IN-ORDER SCHEDULING
>Complete the first item in the queue before moving onto the next item. For closed page, output will always be in order: ACT,RD/WR,PRE. For open page, there is more variation depending on page hit or page miss, but it should still be clear if a request is being process once the previous one is completed. 

### 4.2. LEVEL 1
#### 4.2.1. OPEN PAGE POLICY
>Page is left open for a moment after a read/write command. It will stay open until a read/write is referencing its bank, bank group, but different row (page miss). This means page misses will incur a precharge before activate, and page hits do not need activate (but will need to be more wary of tCCD).

**Test Cases**:
| \#  | OBJECTIVE | INPUT | EXPECTED RESULTS | Notes |
| --- | --------- | ----- | ---------------- | ----- |
|  1  | Test page hit. | Back to back references to same BG,BA, same row, different column. | ACT -> READ -> READ |
|  2  | Test page empty followed by page miss followed by page hit. | Back to back references to same BG,BA, different rows, and then second row gets referenced again. | ACT -> READ -> PRE -> ACT -> READ -> READ | 
|  3  | Test open page tracking when interlearving BG,BA, while trying to trick it by making the BA number be the same. | Back to back references that are intersected by a page access from a different BG with different ROW. | ACT -> ACT -> READ -> READ -> READ | LEVEL 2+ only |
|  4  | Test open page tracking when interlearving BG,BA, while trying to trick it by making the BG number be the same. | Back to back references that are intersected by a page access from a different BA with different ROW. | ACT -> ACT -> READ -> READ -> READ | Same output as 3 |
|  5  | Test open page tracking when intervleaving BG,BA, while trying to trick it by making the ROW be the same. | Back to back references that are intersected by a page access from a different BG,BA, with same ROW. | ACT -> ACT -> READ -> READ -> READ | Same output as 3 |

### 4.3. LEVEL 2
#### 4.3.1. BANK LEVEL PARALLELISM
>Scheduler is able to interleave commands to different BG,BA. For example, after it issues an activate command to BA0,BG0, it can issue another activate to a different BX,BGX combination before coming back and issuing the READ command.

**CASES**
1. When we are waiting for timing contraints on a command being executed in a certain BG,B, and there is a request to a different BG,B in the queue, start the new request (if it does not violate other timings). Repeat. 
   - Allowed to skip over requests that are waiting for a bank to be available (technically out of order)

**Test Cases**:
| \#  | OBJECTIVE | INPUT | EXPECTED RESULTS | Notes |
| --- | --------- | ----- | ---------------- | ----- |
|  1  | BLP when each request is going to a different BA in the same BG. | 4 requests going to same BG, each with different BA. | ACT -> ACT -> ACT -> ACT -> RD -> RD -> RD -> RD | Testing BG 0, 3, 6 to check 0, even, and odd checks. |
|  2  | BLP is working correctly when there are two requests vying for same BG,BA. | 3 requests, first two will go to same BG,BA, and the third will go to different BG,BA. | ACT -> ACT -> RD -> RD -> PRE -> ACT -> RD | First two ACT are for request 1 and 3 respectively. Second request page miss. |
|  x  | On the same even _CPU_ cycle for when a command should be executed, add a request for a different BG,BA. This new request should not start in that exact cycle, instead it needs to wait for idle time. |       |                  | Not created yet |

### 4.4. LEVEL 3
#### 4.4.1. OUT-OF-ORDER SCHEDULING
>Scheduler is able to process requests out-of-order. Must incorporate "aging"/timeout so a request isn't sitting in queue forever.

**CASES**
1. READ over WRITES
   - Only when the addresses are different. If this is done when the addresses are the same, it will lead to reading stale data. 
2. Prioritize page hits over page misses

**Test Cases**:
| \#  | OBJECTIVE | INPUT | EXPECTED RESULTS | Notes |
| --- | --------- | ----- | ---------------- | ----- |
|  1  | Read over write when valid | 3 requests all going to the same BG,BA. R1 will be read, R2 will be write, and R3 will be read. All are going to different ROW,COL. | RD -> RD -> WR | Activates and precharge are omitted |
|  2  | No read over write when not valid (SAME addresses). | 3 requests all going to the same BG,BA. R1 will be read, R2 will be write, and R3 will be read. All are going to the SAME ROW,COL. | RD -> WR -> RD | Also tests how simulator handles when a page hit over page miss is possible, but invalid due to not being able to put read over write.  |
|  3  | Prioritize page hits over page misses. | 3 requests all going to the same BG,BA. R1 will be read, R2 will be read (different row from R1), R3 will be read(same row as R1).| ACT -> RD -> RD -> PRE -> ACT -> RD |       |

note: (R# = request number)

## 5. DRAM COMMANDS FUNCTIONALLY CORRECT
**Note**: In level 2+, the scheduler will always pick READ1, WRITE1, and ACT1 if they are available. This project is using 1n mode, so there is a 1 cycle delay between 0 -> 1. 

### 5.1. READ0, READ1
>READ0 selects the command being issued (due to MUX) and READ1 will get column address. Followed by tCL = 40. Output should show the bank, bank group, and column being accessed (same as the input column address for this project).

### 5.2. WRITE0, WRITE1
>WRITE0 selects the command being issued (due to MUX) and WRITE1 will get the column address. Followed by tCL = 40. Output should show the bank, bank group, and column being accessed (same as the input column address for this project).

### 5.3. ACTIVATE0, ACTIVATE1
>ACTIVATE0 selects the command being issued (due to MUX) and ACTIVATE1 will get the row address. Followed by tRCD = 39. Output should show the bank, bank group, and row being accessed.

### 5.4. PRECHARGE
>If a page is open, the data needs to return from the sense amplifiers so another activate can occur. In a closed page policy, every READ/WRITE command should be followed by PRE. In an open page policy, PRE will occur when the currently open page is not the request page. Followed by tRP = 39. tRAS and tRTP must be satisfied before it can be issued. Output should show the bank, bank group being precharged. 

## 6. DRAM TIMING CORRECT
**Note**: All the RRD and CCD timings are level 1+ or level 2+ only because the cases only occur when a page hit happens, or because our scheduler picks the required back to back commands during bank level parallelism.

### 6.1. tRCD
>tRCD = 39. Cycles to open a row. Occurs between ACT -> READ/WRITE.

### 6.2. tRTP
>tRTP = 18. Read to precharge delay. Occurs between READ -> PRECHARGE. This means a PRE can be done before we start reading out data, but we still need to wait for tRAS to be satisfied.

### 6.3. tWR
>tWR = 30. Write to precharge delay. Occurs between WRITE -> PRECHARGE. This means a PRE can be done before we start reading out data, but we still need to wait for tRAS to be satisfied.

### 6.4. tCL
>tCL = 40. Cycles to begin receiving data. Occurs between READ -> DATA BURST.

### 6.5. tRAS
>tRAS = 76. Amount of cycles needed between ACT -> PRE. By doing the math from the previously mentioned timings, in level 0 we are able to issue the precharge command before tCL is satisfied and while tRTP is satisfied. This should save 13 cycles (79 + 1 + 1 + 8 - 76). This won't be done in level 1+ unless we have foresight on the next request being page miss or page hit. 

Math explantion:
- 79 + 1 + 1 + 8 = tCL + tRCD + 1n + 1n + tBURST; Issue PRE after data is bursted.
- 76 = tRAS; Issue PRE before data is bursted.
- Total is cycles saved by issuing PRE before data burst.

### 6.6. tRP
>tRP = 39. Row precharge time. Occurs between PRE -> ACT.

### 6.7. tFAW
>tFAW = 32. Amount of time needed between every 4 ACT commands. Level 2+.

### 6.8. tRRD_S and tRRD_L
>tRRD_S = 8, tRRD_L = 12. Activate to activate command delay. When switching between bank groups use _S, switching inside a bank group use _L. Level 2+. 

### 6.9. tCCD_S and tCCD_L
>tCCD_S = 8, tCCD_L = 12. Read to read command delay. When switching between bank groups use _S, switching inside a bank group use _L. Level 1+.

### 6.10. tCCD_S_WR and tCCD_L_WR
>tCCD_S_WR = 8, tCCD_L_WR = 48. Write to write command delay. _S for different bank group, _L for same bank group. Level 1+.

### 6.11. tCCD_S_RTW and tCCD_L_RTW
>tCCD_S_RTW = 16, tCCD_L_RTW = 16. Read to write command delay. _S for different bank group, _L for same bank group. Level 1+.

### 6.12. tCCD_S_WTR and tCCD_L_WTR
>tCCD_S_WTR = 70, tCCD_L_WTR = 52. Write to read command delay. _S for different bank gropu, _L for same bank in same bank group. Level 1+.


## copy paste thingy, remove later
**Test Cases**:
| \#  | OBJECTIVE | INPUT | EXPECTED RESULTS | Notes |
| --- | --------- | ----- | ---------------- | ----- |
|  1  |           |       |                  |       |
|  2  |           |       |                  |       |
|  3  |           |       |                  |       |
|  4  |           |       |                  |       |
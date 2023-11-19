# Test Plan Document <!-- omit in toc -->
- [1. INTRODUCTION](#1-introduction)
  - [1.1. PRODUCT](#11-product)
  - [1.2. PROJECT DESCRIPTION](#12-project-description)
  - [1.3. TEST STRATEGY / METHODOLOGY](#13-test-strategy--methodology)
- [2. INPUT VALIDIFICATION](#2-input-validification)
- [3. QUEUE REQUESTS](#3-queue-requests)
- [4. POLICY IMPLEMENTED CORRECTLY](#4-policy-implemented-correctly)
  - [4.1. CLOSED PAGE POLICY](#41-closed-page-policy)
  - [4.2. IN-ORDER SCHEDULING](#42-in-order-scheduling)
  - [4.3. OPEN PAGE POLICY](#43-open-page-policy)
  - [4.4. BANK LEVEL PARALLELISM](#44-bank-level-parallelism)
  - [4.5. OUT-OF-ORDER SCHEDULING](#45-out-of-order-scheduling)
- [5. DRAM COMMANDS FUNCTIONALLY CORRECT](#5-dram-commands-functionally-correct)
  - [5.1. READ0, READ1](#51-read0-read1)
  - [5.2. WRITE0, WRITE1](#52-write0-write1)
  - [5.3. ACTIVATE0, ACTIVATE1](#53-activate0-activate1)
  - [5.4. PRECHARGE](#54-precharge)
- [6. DRAM TIMING CORRECT](#6-dram-timing-correct)
  - [6.1. tCL](#61-tcl)
  - [6.2. tRCD](#62-trcd)
  - [6.3. tRP](#63-trp)
  - [6.4. tRRD\_S and tRRD\_L](#64-trrd_s-and-trrd_l)
  - [6.5. tCCD\_S and tCCD\_L](#65-tccd_s-and-tccd_l)
  - [6.6. tRAS](#66-tras)
  - [6.7. tFAW](#67-tfaw)
- [7. Appendix](#7-appendix)


## 1. INTRODUCTION

### 1.1. PRODUCT

**Product Name:** 
>Team 05 ECE485/585 Final Project

**Last Updated:** 
>11/14/2023

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

## 3. QUEUE REQUESTS
>Only 16 outstanding requests are allowed in the simulation at a time. If there are more than 16 requests in the queue when a new request comes in, that new request will not enter the queue until an item is completed to free up a spot. A request will only be completed and removed from the queue when its last command is executed (ex. read request will leave once the READ command is completed; Total timing: tCL + tBURST -> request is done).

## 4. POLICY IMPLEMENTED CORRECTLY
### 4.1. CLOSED PAGE POLICY
>Page is closed after a read/write command. Thus a precharge must always follow those two commands. Every page reference will page empty. 

### 4.2. IN-ORDER SCHEDULING
>Complete the first item in the queue before moving onto the next item. 
 
### 4.3. OPEN PAGE POLICY
>Page is left open for a moment after a read/write command. It will only stay open if next read/write command is directed to the same bank group, bank and row as the open page. 

**Test Cases**:
| \#  | OBJECTIVE | INPUT | EXPECTED RESULTS | Notes |
| --- | --------- | ----- | ---------------- | ----- |
| 1   | Back to back references to same BG,B, same row | Access BG,B,ROW,COLx, followed by BG,B,ROW,COLy | ACT -> READ -> READ | ----- |
| 2   | Back to back references to same BG,B, different rows | Access BG,B,ROWx, followed by BG,B,ROWy | ACT -> READ -> PRE -> ACT -> READ | ----- |
| 3   | Back to back references to same BG,B, different rows, and then second row gets referenced again | Access BG,B,ROWx, followed by successive acceses to BG,B,ROWy | ACT -> READ -> PRE -> ACT -> READ -> READ | This overlaps with test case 2, decided to list it out to be clear |
| 4   | Back to back references that are intersected by a page access from a different BG,B, with same ROW | Access BGx,Bx,ROWx, followed by access to BGy,BY,ROWx, then access BGx,Bx,ROWx again | ACT -> ACT -> READ -> READ -> READ | Testing how open pages and row tracking interactcs |
| 5   | Back to back references that are intersected by a page access from a different BG,B, with different ROW | Access BGx,Bx,ROWx, followed by access to BGy,BY,ROWy, then access BGx,Bx,ROWx again | ACT -> ACT -> READ -> READ -> READ | Testing how open pages and bank/bankgroup tracking interacts |

### 4.4. BANK LEVEL PARALLELISM
>Scheduler is able to interleave commands to different bank/bank groups. For example, after it issues an activate command to B0,BG0, it can issue another activate to a different B/BG before coming back and issuing the READ command.

**IMPORTANT**
>Need to decide how we are implementing this. One way is having a staging area where it keeps track of which B/BG is idle, and a queue parser that will pop the item into the stage area when needed.

### 4.5. OUT-OF-ORDER SCHEDULING
>Scheduler is able to process request out-of-order. Able to use for bank level parallelism, reads before writes(if not same address). Must incorporate "aging"/timeout so a request isn't sitting in queue forever.

## 5. DRAM COMMANDS FUNCTIONALLY CORRECT
### 5.1. READ0, READ1
>READ0 selects the command being issued (due to MUX) and READ1 will get column address. This will take either 1 or 2 DIMM clocks between each command (going to confirm with prof if its either 1n or 2n). Followed by tCL = 40.

### 5.2. WRITE0, WRITE1
>WRITE0 selects the command being issued (due to MUX) and WRITE1 will get the column address. DIMM clock still need to be confirmed with prof. Followed by tCL = 40.

### 5.3. ACTIVATE0, ACTIVATE1
>ACTIVATE0 selects the command being issued (due to MUX) and ACTIVATE1 will get the row address. Followed by tRCD = 39. 

### 5.4. PRECHARGE
>If a page is open, the data needs to return from the sense amplifiers so another activate can occur. In a closed page policy, every READ/WRITE command should be followed by PRE. In an open page policy, PRE will occur when requested row is not already open. Followed by tRP = 39.

## 6. DRAM TIMING CORRECT
### 6.1. tCL
>tCL = 40. Cycles to send memory the column address. Occurs between READ -> DATA BURST.

### 6.2. tRCD
>tRCD = 39. Cycles to open a closed row. Occurs between ACT -> READ/WRITE.

### 6.3. tRP
>tRP = 39. Cycles to close an open row. Occurs between PRE -> ACT.

### 6.4. tRRD_S and tRRD_L
>tRRD_S = 8, tRRD_L = 12. When switching between bank groups use _S, switching inside a bank group use _L.

### 6.5. tCCD_S and tCCD_L
>tCCD_S = 8, tCCD_L = 12. When switching between bank groups use _S, switching inside a bank group use _L.

### 6.6. tRAS
>tRAS = 76. Amount of cycles needed between ACT -> PRE.

### 6.7. tFAW
>Amount of time needed between every 4 ACT commands. Can't find timing on project desc, gonna ask prof for it. 

## 7. Appendix

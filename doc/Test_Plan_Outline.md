# Test Plan Document <!-- omit in toc -->
- [1. INTRODUCTION](#1-introduction)
  - [1.1. PRODUCT](#11-product)
  - [1.2. PROJECT DESCRIPTION](#12-project-description)
  - [1.3. TEST STRATEGY / METHODOLOGY](#13-test-strategy--methodology)
- [2. POLICY IMPLEMENTED CORRECTLY](#2-policy-implemented-correctly)
  - [2.1. OPEN PAGE POLICY](#21-open-page-policy)
  - [2.2. BANK LEVEL PARALLELISM](#22-bank-level-parallelism)
  - [2.3. OUT-OF-ORDER SCHEDULING](#23-out-of-order-scheduling)
- [3. DRAM COMMANDS FUNCTIONALLY CORRECT](#3-dram-commands-functionally-correct)
  - [3.1. READ0, READ1](#31-read0-read1)
  - [3.2. WRITE0, WRITE1](#32-write0-write1)
  - [3.3. ACTIVATE0, ACTIVATE1](#33-activate0-activate1)
  - [3.4. PRECHARGE](#34-precharge)
- [4. DRAM TIMING CORRECT](#4-dram-timing-correct)
  - [4.1. tCL](#41-tcl)
  - [4.2. tRCD](#42-trcd)
  - [4.3. tRP](#43-trp)
  - [4.4. tRRD\_S and tRRD\_L](#44-trrd_s-and-trrd_l)
  - [4.5. tCCD\_S and tCCD\_L](#45-tccd_s-and-tccd_l)
  - [4.6. tRAS](#46-tras)
- [5. INPUT](#5-input)
- [6. Appendix](#6-appendix)


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

## 2. POLICY IMPLEMENTED CORRECTLY

### 2.1. OPEN PAGE POLICY
>Page is left open for a moment after a read/write command. It will only stay open if next read/write command is directed to the same bank group, bank and row as the open page. 

**Test Cases**:
| \#  | OBJECTIVE | INPUT | EXPECTED RESULTS | TEST DELIVERABLES |
| --- | --------- | ----- | ---------------- | ----------------- |
| 1   | Back to back references to same BG,B, same rows | Access BG,B,ROW,COL-X, followed by BG,B,ROW,COL-Y |ACT -> READ -> READ|                   |
| 2   | Back to back references to same BG,B, different rows, and the second reference is successive with different COL | Access BG,B,ROWX, followed by successive acceses to BG,B,ROWY | ACT -> READ -> ACT -> READ -> READ | ----------------- |
| 3   | Successive page access that is intersected by a page access from a different BG,B, with same ROW vs different ROW | Access BGX,BX,ROWX, followed by access to BGY,BY,ROWX/Y, then access BGX,BX,ROWX again | ACT -> ACT -> READ -> READ -> READ | ----------------- |

### 2.2. BANK LEVEL PARALLELISM
>Scheduler is able to interleave commands to different bank/bank groups. For example, after it issues an activate command to B0,BG0, it can issue another activate to a different B/BG before coming back and issuing the READ command.

**IMPORTANT**
>Need to decide how we are implementing this. One way is having a staging area where it keeps track of which B/BG is idle, and a queue parser that will pop the item into the stage area when needed.

### 2.3. OUT-OF-ORDER SCHEDULING
>Scheduler is able to process request out-of-order. Able to use for bank level parallelism, reads before writes(if not same address). Must incorporate "aging"/timeout so a request isn't sitting in queue forever.

## 3. DRAM COMMANDS FUNCTIONALLY CORRECT
### 3.1. READ0, READ1
### 3.2. WRITE0, WRITE1
### 3.3. ACTIVATE0, ACTIVATE1
### 3.4. PRECHARGE

## 4. DRAM TIMING CORRECT
### 4.1. tCL
### 4.2. tRCD
### 4.3. tRP
### 4.4. tRRD_S and tRRD_L
### 4.5. tCCD_S and tCCD_L
### 4.6. tRAS

## 5. INPUT VALIDIFICATION
>Input should be in the format: time, core, operation, address. Where time is in (absolute) CPU clocks, core is between 0 to 11. operation is 0, 1, or 2, and address is 34-bit address represented in hexadecimal and it is 8-byte aligned. 

## 6. Appendix

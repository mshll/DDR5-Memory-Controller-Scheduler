# Test Plan Document <!-- omit in toc -->
- [1. INTRODUCTION](#1-introduction)
  - [1.1. PRODUCT](#11-product)
  - [1.2. PROJECT DESCRIPTION](#12-project-description)
  - [1.3. TEST STRATEGY / METHODOLOGY](#13-test-strategy--methodology)
- [2. INPUT VALIDIFICATION](#2-input-validification)
- [3. POLICY IMPLEMENTED CORRECTLY](#3-policy-implemented-correctly)
  - [CLOSED PAGE POLICY](#closed-page-policy)
  - [](#)
  - [3.1. OPEN PAGE POLICY](#31-open-page-policy)
  - [3.2. BANK LEVEL PARALLELISM](#32-bank-level-parallelism)
  - [3.3. OUT-OF-ORDER SCHEDULING](#33-out-of-order-scheduling)
- [4. DRAM COMMANDS FUNCTIONALLY CORRECT](#4-dram-commands-functionally-correct)
  - [4.1. READ0, READ1](#41-read0-read1)
  - [4.2. WRITE0, WRITE1](#42-write0-write1)
  - [4.3. ACTIVATE0, ACTIVATE1](#43-activate0-activate1)
  - [4.4. PRECHARGE](#44-precharge)
- [5. DRAM TIMING CORRECT](#5-dram-timing-correct)
  - [5.1. tCL](#51-tcl)
  - [5.2. tRCD](#52-trcd)
  - [5.3. tRP](#53-trp)
  - [5.4. tRRD\_S and tRRD\_L](#54-trrd_s-and-trrd_l)
  - [5.5. tCCD\_S and tCCD\_L](#55-tccd_s-and-tccd_l)
  - [5.6. tRAS](#56-tras)
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

## 2. INPUT VALIDIFICATION
>Input should be in the format: time, core, operation, address. Where time is in (absolute) CPU clocks, core is between 0 to 11. operation is 0, 1, or 2, and address is 34-bit address represented in hexadecimal and it is 8-byte aligned. 

## 3. POLICY IMPLEMENTED CORRECTLY

### CLOSED PAGE POLICY
>Page is closed after a read/write command. Thus a precharge must always follow those two commands. Every page reference will page empty. 

### 
### 3.1. OPEN PAGE POLICY
>Page is left open for a moment after a read/write command. It will only stay open if next read/write command is directed to the same bank group, bank and row as the open page. 

**Test Cases**:
| \#  | OBJECTIVE | INPUT | EXPECTED RESULTS | Notes |
| --- | --------- | ----- | ---------------- | ----- |
| 1   | Back to back references to same BG,B, same row | Access BG,B,ROW,COLx, followed by BG,B,ROW,COLy | ACT -> READ -> READ | ----- |
| 2   | Back to back references to same BG,B, different rows | Access BG,B,ROWx, followed by BG,B,ROWy | ACT -> READ -> PRE -> ACT -> READ | ----- |
| 3   | Back to back references to same BG,B, different rows, and then second row gets referenced again | Access BG,B,ROWx, followed by successive acceses to BG,B,ROWy | ACT -> READ -> PRE -> ACT -> READ -> READ | This overlaps with test case 2, decided to list it out to be clear |
| 4   | Back to back references that are intersected by a page access from a different BG,B, with same ROW | Access BGx,Bx,ROWx, followed by access to BGy,BY,ROWx, then access BGx,Bx,ROWx again | ACT -> ACT -> READ -> READ -> READ | Testing how open pages and row tracking interactcs |
| 5   | Back to back references that are intersected by a page access from a different BG,B, with different ROW | Access BGx,Bx,ROWx, followed by access to BGy,BY,ROWy, then access BGx,Bx,ROWx again | ACT -> ACT -> READ -> READ -> READ | Testing how open pages and bank/bankgroup tracking interacts |

### 3.2. BANK LEVEL PARALLELISM
>Scheduler is able to interleave commands to different bank/bank groups. For example, after it issues an activate command to B0,BG0, it can issue another activate to a different B/BG before coming back and issuing the READ command.

**IMPORTANT**
>Need to decide how we are implementing this. One way is having a staging area where it keeps track of which B/BG is idle, and a queue parser that will pop the item into the stage area when needed.

### 3.3. OUT-OF-ORDER SCHEDULING
>Scheduler is able to process request out-of-order. Able to use for bank level parallelism, reads before writes(if not same address). Must incorporate "aging"/timeout so a request isn't sitting in queue forever.

## 4. DRAM COMMANDS FUNCTIONALLY CORRECT
### 4.1. READ0, READ1
### 4.2. WRITE0, WRITE1
### 4.3. ACTIVATE0, ACTIVATE1
### 4.4. PRECHARGE

## 5. DRAM TIMING CORRECT
### 5.1. tCL
### 5.2. tRCD
### 5.3. tRP
### 5.4. tRRD_S and tRRD_L
### 5.5. tCCD_S and tCCD_L
### 5.6. tRAS

## 6. Appendix

/**
 * @file  dram.h
 *
 * @note the following timing constrains are actually one dimm cycle longer:
 *            tRC -> 115
 *            tRP -> 39
 *            tRRD_L -> 12, tRRD_S -> 8
 *            tRCD -> 39
 *            tCCD_L -> 12, tCCD_S -> 8
 *            tCCD_L_WTR -> 70, tCCD_S_WTR -> 52
 *            tCCD_L_WR -> 48, tCCD_S_WR -> 8
 *            tCCD_L_RTW -> 16, tCCD_S_RTW -> 16
 * 
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __DIMM_H__
#define __DIMM_H__

#include "common.h"
#include "memory_request.h"
#include "queue.h"

/*** macro(s), enum(s), struct(s) ***/
#define TRC       114 // time interval between successive ACT commands to the same bank
#define TRAS       76 // time interval between a bank ACT command and issuing the PRE command
#define TRP        38 // time interval between a PRE command and a ACT command
#define TRFC      708 // 295ns; time it takes to complete a refresh command
#define TCWL       38 // aka tCWD; time interval between a WR command and the output of the first bit of data
#define TCL        40 // aka tCAS; time interval between a RD command and the output of the first bit of data
#define TRCD       38 // time interval between a ACT command and a RD/WR command
#define TWR        30 // time interval between writing data and issuing a PRE command
#define TRTP       18 // delay between internal RD command to PRE command within the same bank
#define TBURST      8 // delay between the start and end of RD/WR data 
#define NUM_TIMING_CONSTRAINTS 10 

#define TRRD_L     11 // time interval between consecutive ACT commands to the same bank group
#define TRRD_S      7 // time interval between consecutive ACT commands to different bank group
#define TCCD_L     11 // time interval between consecutive RD commands between different banks in the same bank group
#define TCCD_S      7 // time interval between consecutive RD commands between different banks in different bank group
#define TCCD_L_WR  47 // time interval between consecutive WR commands between different banks in the same bank group
#define TCCD_S_WR   7 // time interval between consecutive WR commands between different banks in different bank group
#define TCCD_L_RTW 15 // READ -> WRITE in the same bank group
#define TCCD_S_RTW 15 // READ -> WRITE in different bank group
#define TCCD_L_WTR 69 // WRITE -> READ in the same bank group
#define TCCD_S_WTR 51 // WRITE -> READ in different bank group
#define NUM_CONSECUTIVE_CMD_CONSTRAINTS 10

#define TFAW       39 // time window where there can be at most four ACT commands
#define NUM_TFAW_COUNTERS 4

#define NUM_BANKS 32
#define NUM_BANK_GROUPS 8
#define NUM_BANKS_PER_GROUP (NUM_BANKS / NUM_BANK_GROUPS)
#define NUM_CHANNELS 2
#define NUM_CHIPS_PER_CHANNEL 4

#define CACHE_LINE_BOUNDARY 64
#define BANK_ALIGN 8

extern uint16_t timing_attribute[NUM_TIMING_CONSTRAINTS];
extern uint8_t consecutive_cmd_attribute[NUM_CONSECUTIVE_CMD_CONSTRAINTS];

typedef enum TimingConstraints {
  tRC,
  tRAS,
  tRP,
  tRFC,
  tCWL,
  tCL,
  tRCD,
  tWR,
  tRTP,
  tBURST
} TimingConstraints_t;

typedef enum ConsecutiveCmdConstraints {
  tRRD_L,
  tRRD_S,
  tCCD_L,
  tCCD_S,
  tCCD_L_WR,
  tCCD_S_WR,
  tCCD_L_RTW,
  tCCD_S_RTW,
  tCCD_L_WTR,
  tCCD_S_WTR
} ConsecutiveCmdConstraints_t;


typedef struct __attribute__((__packed__)) Bank {
  bool is_precharged;
  bool is_active;
  Operation_t last_request_operation;
  uint32_t active_row;
} Bank_t;

typedef struct __attribute__((aligned(CACHE_LINE_BOUNDARY))) BankGroup {
  Bank_t banks[NUM_BANKS_PER_GROUP];
} BankGroup_t;

typedef struct DRAM {
  BankGroup_t bank_groups[NUM_BANK_GROUPS];
  uint16_t timing_constraints[NUM_BANK_GROUPS][NUM_BANKS_PER_GROUP][NUM_TIMING_CONSTRAINTS];
  uint8_t tFAW_timers[NUM_TFAW_COUNTERS];
  uint8_t consecutive_cmd_timers[NUM_CONSECUTIVE_CMD_CONSTRAINTS];
  uint8_t last_bank_group;
  Commands_t last_interface_cmd;
} DRAM_t;

typedef struct Channel {
  DRAM_t DDR5_chip[NUM_CHIPS_PER_CHANNEL];
} Channel_t;

typedef struct DIMM {
  Channel_t channels[NUM_CHANNELS];
  FILE *output_file;
} DIMM_t;

/*** function declaration(s) ***/
void dimm_create(DIMM_t **dimm, char *output_file_name);
void dimm_destroy(DIMM_t **dimm);
void process_request(DIMM_t **dimm, Queue_t **q, uint64_t dimm_cycle, uint8_t scheduling_algorithm);

#endif
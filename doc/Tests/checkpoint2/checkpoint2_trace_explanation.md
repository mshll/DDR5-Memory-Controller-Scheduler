## WHAT THIS IS TESTING
>The queue should only ever have 16 outstanding requests at a time. For checkpoint 2, a request will be inserted every CPU clock and removed every DIMM clock. This means if requests are continuously inserted every CPU clock, the queue will eventually get a request but be full. In this situation the simulation needs to somehow track that a request's insertion time has passed, and it needs to inserted into the queue as soon as there is room. 

>Another requirement is the output when a request is satisfied. On those DIMM clocks when it is removed, the simulator will output the DRAM commands used. These will always be ACT0->ACT1->READ0/WRITE0->READ1/WRITE1->PRE. The output will need to accompany these commands with the correct **bank**, **bank** **group**, **row**, and **column** being manipulated. 

## TRACE DOCUMENTATION
- There will be a request every CPU clock cycle starting from cycle 0 all the way until 33. 
  - The most important CPU time to take notice will be cycle 30, 31, 32, 33.
    - At CPU time 30, the queue becomes full on a DIMM falling edge
    - At CPU time 31, the queue will finish a request and then become full again
    - At CPU time 32, the queue is full and a request is available (should be noted but not added)
    - At CPU time 33, a request is satisfied and the delayed request being added should be from CPU time 32, not CPU time 33
- The first 4 request will test our parsing functions with following BA,BG,R,C:
  - 3,0,0,0     (All BA bits are 1, rest 0)
  - 0,7,0,0     (All BG bits are 1, rest 0)
  - 0,0,65535,0 (All Row bits are 1, rest 0)
  - 0,0,0,1023  (All Column bits are 1, rest 0)
  - If the DRAM commands are not 0 for all but one variable, clearly the parsing is shifted by n-bits
- Requests will be alternating reads and writes. 
- Cores will count from 0 to 11 and then back down: 0,1,2,...11,11,10,9,...1, etc.
- Requests 5 to 29 will loop the following pattern:
  - 0,0,0,1
  - 0,0,1,0
  - 0,1,0,0
  - 1,0,0,0
  - And the number will increase until it loops back to 1
- Request 30, 31, 32, 33 will use the same commands as the first four reqeusts for distinguishability.
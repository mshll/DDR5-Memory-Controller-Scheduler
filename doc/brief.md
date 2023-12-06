## ECE 485/585 Team 05 Final Project

### Members:
- Abdulaziz Alateeqi	
- Meshal Almutairi	
- Eduardo Simancas	
- Gene Hu

#### Discription:
This project is a simulator for the scheduling portion of a memory controller. 

#### Mapping
The address mapping used is a 34 bit address that is split into row-address[33-18], high-column-address[17-12], bank[11,10], bank-group[9-7], channel[6], lower-column-address[5-2], and byte select[1,0]. 
Visually shown:
## Topological Address Mapping
The following table shows the topological address mapping for this project.
<div><table>
<tbody>
  <tr><td>33<td>32<td>31<td>...<td>18<td>17<td>16<td>15<td>14<td>13<td>12<td>11<td>10<td>9<td>8<td>7<td>6<td>5<td>4<td>3<td>2<td>1<td>0</td></tr>
  <tr>
    <td colspan="5" rowspan="2">Row</td>
    <td colspan="6" rowspan="2">Column [9:4]</td>
    <td colspan="2" rowspan="2">Bank</td>
    <td colspan="3" rowspan="2">Bank Group</td>
    <td rowspan="2">Channel</td>
    <td colspan="4" rowspan="2">Column [3:0]</td>
    <td colspan="2" rowspan="2">Byte Select</td>
  </tr>
</tbody>
</table></div>
The reason for these decisions from LSB to MSB:

1) Byte select is the lowest 2 bits because we are using DDR5 which will output 32 bits simultaneously. 
2) The lower column address bits (4 bits) are next as burst selection bits. In a burst 16, we are able to choose where in that 16 to begin the burst.
3) The channel bit is next to increase dual channel efficiency thus increasing bandwidth.
4) Bank group bits are next to take advantage of _S timings.
5) Bank bits are next to minimize the chance of the next memory reference going to a bank that still waiting for precharge to finish.
6) Columns bits come before rows bits to take advantage of a potential open page policy.

#### Algorithm and Policies
This project will implement levels 0 through 3. This means it will have: 
- closed page policy and in order scheduler, 
- open page policy and in order scheduler, 
- open page policy with bank level parallelism and in order scheduler, 
- open page with bank level parallelism and out of order scheduler which will prioritize reads over writes (different addresses) and page hits over page misses. 

The level of the scheduler can be chosen on startup.
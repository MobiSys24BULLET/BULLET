
#ifndef FECONLY_FEC_ARRAY_H
#define FECONLY_FEC_ARRAY_H

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "math.h"

// interface
/**
 * @brief Get FEC parameters given network status
 *
 * @param loss packet loss rate, [0.0000, 0.5000]
 * @param group_size FEC group size, [5, 55]
 * @param beta_0 stores FEC rate for the first transmission, [0, 1]
 */
void get_fec_rate_feconly(double_t loss, uint8_t group_size, double_t * beta_0);

// FEC array
static const uint8_t fec_array_feconly[561] = {
0,0,0,0,0,0,0,0,0,0,0,3,4,4,5,5,5,6,6,6,6,7,4,5,5,6,6,7,7,8,8,9,9,4,5,6,7,8,8,9,
9,10,10,11,5,6,7,8,9,10,10,11,12,12,13,5,7,8,9,10,11,11,12,13,14,14,5,7,9,10,11,12,13,14,14,15,16,5,8,9,
11,12,13,14,15,16,17,18,5,8,10,11,13,14,15,16,17,18,19,5,9,11,12,14,15,16,17,19,20,20,5,10,11,13,15,16,17,19,20,21,
22,5,10,12,14,16,17,19,20,21,23,24,5,10,13,15,16,18,20,21,23,24,25,5,10,14,16,17,19,21,22,24,26,27,5,10,14,16,18,20,
22,24,25,27,29,5,10,15,17,19,21,23,25,27,28,30,5,10,15,18,20,22,24,26,28,30,31,5,10,15,19,21,23,26,28,30,32,33,5,10,
15,20,22,25,27,29,31,33,35,5,10,15,20,23,26,28,30,33,35,37,5,10,15,20,24,27,29,32,34,36,39,5,10,15,20,25,28,31,33,36,
38,40,5,10,15,20,25,29,32,35,37,40,42,5,10,15,20,25,30,33,36,39,41,44,5,10,15,20,25,30,35,37,40,43,46,5,10,15,20,25,
30,35,39,42,45,48,5,10,15,20,25,30,35,40,44,47,50,5,10,15,20,25,30,35,40,45,48,52,5,10,15,20,25,30,35,40,45,50,53,5,
10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,
45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,
25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,
5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,
40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,
20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,55,5,10,15,20,25,30,35,40,45,50,
55};

#endif  // FECONLY_FEC_ARRAY_H

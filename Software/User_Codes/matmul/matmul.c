// See LICENSE for license details.
#include <stdio.h>
#include "dataset.h"
#include "util.h"
#include <stddef.h>

unsigned long start_cycles, end_cycles;
unsigned long start_insn, end_insn;
unsigned long total_cycles, total_insn;
double cpi;
#pragma GCC optimize ("unroll-loops")

void matmul(const size_t coreid, const size_t ncores, const size_t lda,  const data_t A[], const data_t B[], data_t C[])
{
  size_t i, j, k;
  size_t block = lda / ncores;
  size_t start = block * coreid;

  for (i = 0; i < lda; i++) {
    for (j = start; j < (start+block); j++) {
      data_t sum = 0;
      for (k = 0; k < lda; k++)
        sum += A[j*lda + k] * B[k*lda + i];
      C[i + j*lda] = sum;
    }
  }
}

int main() {
size_t lda = DIM_SIZE; // Matrix dimension (using DIM_SIZE from dataset.h)
data_t C[ARRAY_SIZE];

// Run matrix multiplication
asm volatile ("rdcycle %0" : "=r"(start_cycles));
asm volatile ("rdinstret %0" : "=r"(start_insn));
matmul(0, 1, lda, input1_data, input2_data, C);
asm volatile ("rdcycle %0" : "=r"(end_cycles));
asm volatile ("rdinstret %0" : "=r"(end_insn));

total_cycles = end_cycles - start_cycles;
total_insn = end_insn - start_insn;
cpi = (double)total_cycles / (double)total_insn;

printf("\nMATMul Benchmark Results....\n");
printf("\n (Matrix Size: %zu x %zu):\n", lda, lda); // Print matrix size
printf("  Total Cycles: %lu\n", total_cycles);
printf("  Total Instructions: %lu\n", total_insn);
printf("  Cycles per Instruction (CPI) Integer: %d\n",(int)cpi); // Print CPI integer value

return 0;
}

#ifndef HAMMING_H
#define HAMMING_H

#include <stdio.h>
#include <string.h>

#define MAX 500   // maximum bits supported

typedef struct {
    int data[MAX];   // original data bits
    int code[MAX];   // codeword bits (with parity)
    int m;           // number of data bits
    int r;           // number of redundant bits
    int total;       // total codeword length (m + r)
} Hamming;

// Sender side functions
Hamming inputData(char *input);
int findRedundantBits(int m);
Hamming buildCodeword(Hamming h);
Hamming setParityBits(Hamming h);
void saveCodeword(Hamming h);

// Receiver side functions
Hamming loadCodeword();
int checkError(Hamming h);

#endif

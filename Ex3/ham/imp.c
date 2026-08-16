#include "head.h"
#include <stdio.h>
#include <string.h>

// Store input bits
Hamming inputData(char *input) {
    Hamming h;
    h.m = strlen(input);
    for (int i = 0; i < h.m; i++) {
        h.data[i] = input[i] - '0';
    }
    return h;
}

// Find r such that 2^r >= m + r + 1, with step-by-step display
int findRedundantBits(int m) {
    int r = 0;
    while ((1 << r) < (m + r + 1)) {
        printf("Checking r=%d: 2^%d = %d < %d (false)\n",
               r, r, (1 << r), m + r + 1);
        r++;
    }
    printf("Checking r=%d: 2^%d = %d >= %d (true)\n",
           r, r, (1 << r), m + r + 1);
    return r;
}

// Build codeword with placeholders for parity
Hamming buildCodeword(Hamming h) {
    h.total = h.m + h.r;
    int dataIndex = 0;
    for (int i = 1; i <= h.total; i++) {
        if ((i & (i - 1)) == 0) {
            h.code[i] = 0; // parity placeholder
        } else {
            h.code[i] = h.data[dataIndex++];
        }
    }
    return h;
}

// Compute parity bits using XOR block toggling
Hamming setParityBits(Hamming h) {
    for (int i = 0; i < h.r; i++) {
        int pos = 1 << i;
        int sum = 0;
        for (int j = pos; j <= h.total; j += 2*pos) {
            for (int k = j; k < j + pos && k <= h.total; k++) {
                sum ^= h.code[k];
            }
        }
        h.code[pos] = sum;
    }
    return h;
}

// Save codeword to file (with r at top)
void saveCodeword(Hamming h) {
    FILE *fp = fopen("codeword.txt", "w");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return;
    }

    fprintf(fp, "r=%d\n", h.r);
    for (int i = 1; i <= h.total; i++) {
        fprintf(fp, "%d", h.code[i]);
    }
    fprintf(fp, "\n");

    fclose(fp);
}

// Load codeword from file (reads r and codeword)
Hamming loadCodeword() {
    Hamming h;
    FILE *fp = fopen("codeword.txt", "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return h;
    }

    fscanf(fp, "r=%d\n", &h.r);

    char ch;
    int len = 0;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '0' || ch == '1') {
            h.code[++len] = ch - '0';
        }
    }
    fclose(fp);

    h.total = len;
    h.m = h.total - h.r;

    // Print loaded info
    printf("Loaded r=%d\n", h.r);
    printf("Received Codeword: ");
    for (int i = h.total; i >= 1; i--) {
        printf("%d", h.code[i]);
    }
    printf("\n");

    return h;
}

// Detect error position
int checkError(Hamming h) {
    int errorPos = 0;
    for (int i = 0; i < h.r; i++) {
        int pos = 1 << i;
        int sum = 0;
        for (int j = pos; j <= h.total; j += 2*pos) {
            for (int k = j; k < j + pos && k <= h.total; k++) {
                sum ^= h.code[k];
            }
        }
        if (sum != 0) errorPos += pos;
    }
    return errorPos;
}

#ifndef HEAD_H
#define HEAD_H

#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 8
#define MAX_BLOCKS 100

typedef struct
{
    char blocks[MAX_BLOCKS][9];
    int count;
    char checksum[9];
} Packet;

/* Sender */
int createPacket(char input[]);
void generateChecksum(Packet *p);

/* Receiver */
int checkChecksum();

/* Display */
void displayPacket();

#endif

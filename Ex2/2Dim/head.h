#ifndef HEAD_H
#define HEAD_H

#include <stdio.h>
#include <string.h>

#define COL_SIZE 7
#define MAX_ROWS 50

typedef struct
{
    /* +1 row for column parity, +1 column for row parity */
    char data[MAX_ROWS + 1][COL_SIZE + 1];
    int rows;

} Packet;

/* Function Prototypes */
void sender(char input[], Packet *p);
int receiver(Packet *p);

#endif

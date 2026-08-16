#ifndef HEAD_H
#define HEAD_H

#include <stdio.h>
#include <string.h>

#define MAX_DATA_SIZE 1000

#define FLAG    "01111110"
#define ADDRESS "00000000"
#define CONTROL "11111111"
#define FCS     "0000000000000000"

typedef struct
{
    char flag[9];
    char address[9];
    char control[9];
    char data[101];
    char fcs[17];
} Frame;

void bitStuff(char data[], char stuffed[]);
void bitDestuff(char stuffed[], char destuffed[]);

int createFrames(char input[], int frameSize);
int retrieveFrames(char output[]);

#endif
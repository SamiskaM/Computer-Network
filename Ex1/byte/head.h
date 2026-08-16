#ifndef HEAD_H
#define HEAD_H

#include <stdio.h>
#include <string.h>

#define MAX_DATA_SIZE 1000

/* HDLC Constants */
#define FLAG    "01111110"   /* Frame delimiter */
#define ESC     "01111101"   /* Escape sequence */
#define ADDRESS "00000000"   /* Example address field */
#define CONTROL "11111111"   /* Example control field */
#define FCS     "0000000000000000" /* 16-bit Frame Check Sequence */

/* Each DATA field is 8 bits (1 byte) */
#define FRAME_DATA_SIZE 32

/* HDLC Frame Structure */
typedef struct
{
    char flag[9];                 /* FLAG : 8 bits */
    char address[9];              /* ADDRESS : 8 bits */
    char control[9];              /* CONTROL : 8 bits */
    char data[FRAME_DATA_SIZE+1]; /* DATA : 8 bits (1 byte) */
    char fcs[17];                 /* FCS : 16 bits */
} Frame;

/* Byte Stuffing */
void byteStuff(char data[], char stuffed[]);

/* Byte Destuffing */
void byteDestuff(char stuffed[], char destuffed[]);

/* Create HDLC Frames (Sender side) */
int createFrames(char input[]);

/* Retrieve HDLC Frames (Receiver side) */
int retrieveFrames(char original[]);

#endif

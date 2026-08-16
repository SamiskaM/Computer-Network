#include "head.h"

/*----------------------------------------------------------
  Function : byteStuff()
----------------------------------------------------------*/
void byteStuff(char data[], char stuffed[])
{
    int i = 0, j = 0;

    while(data[i] != '\0')
    {
        if(strncmp(&data[i], FLAG, 8) == 0)
        {
            strncpy(&stuffed[j], ESC, 8);
            j += 8;
            strncpy(&stuffed[j], FLAG, 8);
            j += 8;
        }
        else if(strncmp(&data[i], ESC, 8) == 0)
        {
            strncpy(&stuffed[j], ESC, 8);
            j += 8;
            strncpy(&stuffed[j], ESC, 8);
            j += 8;
        }
        else
        {
            strncpy(&stuffed[j], &data[i], 8);
            j += 8;
        }
        i += 8;
    }
    stuffed[j] = '\0';
}

/*----------------------------------------------------------
  Function : byteDestuff()
----------------------------------------------------------*/
void byteDestuff(char stuffed[], char destuffed[])
{
    int i = 0, j = 0;

    while(stuffed[i] != '\0')
    {
        if(strncmp(&stuffed[i], ESC, 8) == 0 &&
           strncmp(&stuffed[i+8], FLAG, 8) == 0)
        {
            i += 8; // skip ESC
            strncpy(&destuffed[j], FLAG, 8);
            j += 8;
            i += 8;
        }
        else if(strncmp(&stuffed[i], ESC, 8) == 0 &&
                strncmp(&stuffed[i+8], ESC, 8) == 0)
        {
            i += 8; // skip ESC
            strncpy(&destuffed[j], ESC, 8);
            j += 8;
            i += 8;
        }
        else
        {
            strncpy(&destuffed[j], &stuffed[i], 8);
            j += 8;
            i += 8;
        }
    }
    destuffed[j] = '\0';
}

/*----------------------------------------------------------
  Function : createFrames()
----------------------------------------------------------*/
int createFrames(char input[])
{
    FILE *fp;
    Frame f;
    int len = strlen(input);
    int i, frameNo = 1;

    strcpy(f.flag, FLAG);
    strcpy(f.address, ADDRESS);
    strcpy(f.control, CONTROL);
    strcpy(f.fcs, FCS);

    fp = fopen("hdlc.txt", "w");
    if(fp == NULL) return 0;

    for(i = 0; i < len; i += FRAME_DATA_SIZE)
    {
        char temp[FRAME_DATA_SIZE+1];
        char stuffed[1000];
        int k = 0;

        for(int j = i; j < i+FRAME_DATA_SIZE && j < len; j++)
            temp[k++] = input[j];
        temp[k] = '\0';
        strcpy(f.data, temp);

        byteStuff(f.data, stuffed);

        printf("\n========================================");
        printf("\nFrame %d (Sender Side)", frameNo);
        printf("\n========================================");
        printf("\nOriginal Data : %s", f.data);
        printf("\nStuffed Data  : %s", stuffed);
        printf("\nHDLC Frame    : %s%s%s%s%s%s\n",
               f.flag, f.address, f.control,
               stuffed, f.fcs, f.flag);

        fprintf(fp, "%s%s%s%s%s%s",
                f.flag, f.address, f.control,
                stuffed, f.fcs, f.flag);

        frameNo++;
    }

    fclose(fp);
    return 1;
}

/*----------------------------------------------------------
  Function : retrieveFrames()
----------------------------------------------------------*/
int retrieveFrames(char original[])
{
    FILE *fp;
    char stream[10000];
    int len = 0, pos = 0, frameNo = 1;
    original[0] = '\0';

    fp = fopen("hdlc.txt", "r");
    if(fp == NULL) return 0;

    while((stream[len] = fgetc(fp)) != EOF) len++;
    stream[len] = '\0';
    fclose(fp);

    while(pos < len)
    {
        if(strncmp(stream+pos, FLAG, 8) != 0)
        {
            printf("Invalid HDLC Frame\n");
            return 0;
        }

        pos += 8; // FLAG
        pos += 8; // ADDRESS
        pos += 8; // CONTROL

        int dataStart = pos;

        // Find ending FLAG
        while(pos <= len-8)
        {
            if(strncmp(stream+pos, FLAG, 8) == 0)
                break;
            pos++;
        }
        if(pos > len-8) return 0;

        int stuffedLength = (pos - dataStart) - 16; // remove FCS
        if(stuffedLength < 0) return 0;

        char stuffed[1000], destuffed[1000];
        strncpy(stuffed, stream+dataStart, stuffedLength);
        stuffed[stuffedLength] = '\0';

        byteDestuff(stuffed, destuffed);

        printf("\n========================================");
        printf("\nFrame %d (Receiver Side)", frameNo);
        printf("\n========================================");
        printf("\nStuffed Data   : %s", stuffed);
        printf("\nDestuffed Data : %s\n", destuffed);

        strcat(original, destuffed);

        pos += 8; // skip ending FLAG
        frameNo++;
    }
    return 1;
}

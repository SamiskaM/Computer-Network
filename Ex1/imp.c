#include "head.h"

/*-------------------- Bit Stuffing --------------------*/
/* Rule : 011111 --> 0111110 */

void bitStuff(char data[], char stuffed[])
{
    int i, j = 0;

    for(i = 0; data[i] != '\0'; i++)
    {
        stuffed[j++] = data[i];

        if(i >= 5 &&
           data[i-5] == '0' &&
           data[i-4] == '1' &&
           data[i-3] == '1' &&
           data[i-2] == '1' &&
           data[i-1] == '1' &&
           data[i]   == '1')
        {
            stuffed[j++] = '0';
        }
    }

    stuffed[j] = '\0';
}

/*-------------------- Bit Destuffing --------------------*/

void bitDestuff(char stuffed[], char destuffed[])
{
    int i, j = 0;

    for(i = 0; stuffed[i] != '\0'; i++)
    {
        destuffed[j++] = stuffed[i];

        if(i >= 6 &&
           stuffed[i-6] == '0' &&
           stuffed[i-5] == '1' &&
           stuffed[i-4] == '1' &&
           stuffed[i-3] == '1' &&
           stuffed[i-2] == '1' &&
           stuffed[i-1] == '1' &&
           stuffed[i]   == '0')
        {
            j--;
        }
    }

    destuffed[j] = '\0';
}

/*-------------------- Create Frames --------------------*/

int createFrames(char input[], int frameSize)
{
    FILE *fp;
    Frame f;
    int len, i;
    int frameNo = 1;

    len = strlen(input);

    strcpy(f.flag, FLAG);
    strcpy(f.address, ADDRESS);
    strcpy(f.control, CONTROL);
    strcpy(f.fcs, FCS);

    fp = fopen("hdlc.txt", "w");

    if(fp == NULL)
        return 0;

    for(i = 0; i < len; i += frameSize)
    {
        char temp[101];
        char stuffed[200];
        int j, k = 0;

        for(j = i; j < i + frameSize && j < len; j++)
            temp[k++] = input[j];

        temp[k] = '\0';

        strcpy(f.data, temp);

        bitStuff(f.data, stuffed);

        /* Console Display */
        printf("\nFrame %d\n", frameNo);
        printf("Original Data : %s\n", f.data);
        printf("Stuffed Data  : %s\n", stuffed);

        printf("HDLC Frame    : %s%s%s%s%s%s\n",
               f.flag,
               f.address,
               f.control,
               stuffed,
               f.fcs,
               f.flag);

        /* Store in file (same structure) */
        fprintf(fp, "%s%s%s%s%s%s",
                f.flag,
                f.address,
                f.control,
                stuffed,
                f.fcs,
                f.flag);

        frameNo++;
    }

    fclose(fp);
    return 1;
}

/*-------------------- Retrieve Frames --------------------*/

int retrieveFrames(char output[])
{
    FILE *fp;
    char stream[5000];
    char stuffed[500];
    char destuffed[500];

    int len = 0;
    int pos = 0;

    output[0] = '\0';

    fp = fopen("hdlc.txt", "r");

    if(fp == NULL)
        return 0;

    while((stream[len] = fgetc(fp)) != EOF)
        len++;

    stream[len] = '\0';

    fclose(fp);

    while(pos < len)
    {
        char *flagPos;
        int stuffedLength;

        if(strncmp(stream + pos, FLAG, 8) != 0)
            return 0;

        pos += 8;
        pos += 8;
        pos += 8;

        flagPos = strstr(stream + pos, FLAG);

        if(flagPos == NULL)
            return 0;

        stuffedLength = (flagPos - (stream + pos)) - 16;

        strncpy(stuffed, stream + pos, stuffedLength);
        stuffed[stuffedLength] = '\0';

        bitDestuff(stuffed, destuffed);

        strcat(output, destuffed);

        pos = (flagPos - stream) + 8;
    }

    return 1;
}
#include "head.h"
/* Utility to print packet.txt contents */
void printPacketFile() {
    FILE *fp = fopen("packet.txt", "r");
    if (fp == NULL) {
        printf("\n[packet.txt not found]\n");
        return;
    }

    char ch;
    printf("\n--- Contents of packet.txt ---\n");
    while ((ch = fgetc(fp)) != EOF) {
        putchar(ch);
    }
    printf("\n--- End of packet.txt ---\n");
    fclose(fp);
}


int main()
{
    Packet p;
    int choice;
    int i, j;
    char input[500];

    while(1)
    {
        printf("\n===== TWO DIMENSIONAL PARITY =====\n");
        printf("\n1. Sender");
        printf("\n2. Receiver");
        printf("\n3. Exit");

        printf("\n\nEnter Choice : ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                printf("\nEnter Binary Data : ");
                scanf("%s", input);

                sender(input, &p);

                /* Print original data grouped into 7 bits */
                printf("\nOriginal Data (7-bit groups):\n");
                int len = strlen(input);
                for (int k = 0; k < len; k++) {
                    printf("%c ", input[k]);
                    if ((k+1) % COL_SIZE == 0) printf(" ");
                }
                printf("\n");

                printf("\n=========== SENDER SIDE ===========\n\n");
                printf("        C1 C2 C3 C4 C5 C6 C7 | RP\n");
                printf("-----------------------------------\n");

                for(i = 0; i < p.rows; i++) {
                    printf("Row %-2d : ", i + 1);
                    for(j = 0; j < COL_SIZE; j++)
                        printf(" %c ", p.data[i][j]);
                    printf("| %c\n", p.data[i][COL_SIZE]);
                }

                printf("-----------------------------------\n");
                printf("CP      : ");
                for(j = 0; j < COL_SIZE; j++)
                    printf(" %c ", p.data[p.rows][j]);
                printf("| %c\n", p.data[p.rows][COL_SIZE]);

                printf("\nPacket Sent Successfully.\n");
		printf("Packet.txt\n");
		printPacketFile();

                break;

            case 2:
                printf("\n=========== RECEIVER SIDE ===========\n");

                if(receiver(&p) == -1) {
                    printf("\nPacket File Not Found.\n");
                } else {
		    printf("Packet.txt\n");
		    printPacketFile();

                    /* Print received data grouped into 7 bits */
                    printf("\nReceived Data (7-bit groups):\n");
                    int bitCount = 0;
                    for (i = 0; i < p.rows; i++) {
                        for (j = 0; j < COL_SIZE; j++) {
                            printf("%c ", p.data[i][j]);
                            bitCount++;
                            if (bitCount % COL_SIZE == 0) printf(" ");
                        }
                    }
                    printf("\n");

                    printf("        C1 C2 C3 C4 C5 C6 C7 | RP\n");
                    printf("-----------------------------------\n");
                    for(i = 0; i < p.rows; i++) {
                        printf("Row %-2d : ", i + 1);
                        for(j = 0; j < COL_SIZE; j++)
                            printf(" %c ", p.data[i][j]);
                        printf("| %c\n", p.data[i][COL_SIZE]);
                    }

                    printf("-----------------------------------\n");
                    printf("CP      : ");
                    for(j = 0; j < COL_SIZE; j++)
                        printf(" %c ", p.data[p.rows][j]);
                    printf("| %c\n", p.data[p.rows][COL_SIZE]);

                    if(receiver(&p) == 0)
                        printf("\n\nNo Error Detected.\n");
                    else
                        printf("\n\nError Detected.\n");
                }
                break;

            case 3:
                return 0;

            default:
                printf("\nInvalid Choice.\n");
        }
    }
    return 0;
}

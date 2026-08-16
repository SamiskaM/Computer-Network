#include "head.h"

int main()
{
    char input[MAX_DATA_SIZE];
    char output[MAX_DATA_SIZE];
    int choice;

    while(1)
    {
        printf("\n========== BYTE STUFFING ==========\n");
        printf("1. Sender\n");
        printf("2. Receiver\n");
        printf("3. Exit\n");
        printf("Enter your choice : ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1: // Sender
                printf("\nEnter Binary Input : ");
                scanf("%s", input);

                printf("\nBinary Input : %s\n", input);

                if(createFrames(input))
                {
                    printf("\nStuffed Data Stored Successfully.\n");
                    printf("\nFrames stored in hdlc.txt\n");
                }
                else
                {
                    printf("\nError Opening File.\n");
                }
                break;

            case 2: // Receiver
                if(retrieveFrames(output))
                {
                    printf("\n========================================");
                    printf("\nReceiver Side");
                    printf("\n========================================");
                    printf("\nDestuffed Data : %s\n", output);
                }
                else
                {
                    printf("\nError Retrieving Frames.\n");
                }
                break;

            case 3: // Exit
                printf("\nThank You...\n");
                return 0;

            default:
                printf("\nInvalid Choice\n");
        }
    }

    return 0;
}

#include "head.h"

int main()
{
    char input[MAX_DATA_SIZE];
    char output[MAX_DATA_SIZE];
    int frameSize;
    int choice;

    while(1)
    {
        printf("\n--- HDLC Menu ---\n");
        printf("1. Sender (Stuff Data)\n");
        printf("2. Receiver (Destuff Data)\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1: // Sender
                printf("Enter Binary Input : ");
                scanf("%s", input);

                printf("Enter Frame Size   : ");
                scanf("%d", &frameSize);

                printf("\nInput Data : %s\n", input);

                if(createFrames(input, frameSize))
                    printf("\nFrames stored successfully in hdlc.txt\n");
                else
                    printf("Error opening file\n");
                break;

            case 2: // Receiver
                if(retrieveFrames(output))
                    printf("\nDestuffed Data : %s\n", output);
                else
                    printf("Error retrieving frames\n");
                break;

            case 3: // Exit
                printf("Exiting program...\n");
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

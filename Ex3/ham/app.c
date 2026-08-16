#include <stdio.h>
#include "head.h"

int main() {
    Hamming h;
    int choice;
    char input[MAX];

    do {
        printf("\n========== Hamming Code ==========\n");
        printf("1. Sender\n");
        printf("2. Receiver\n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            // Sender side
            printf("\nEnter binary data: ");
            scanf("%s", input);

            h = inputData(input);
            h.r = findRedundantBits(h.m);
            h = buildCodeword(h);
            h = setParityBits(h);

            // Print final codeword
            printf("Final Codeword: ");
            for (int i = h.total; i >= 1; i--) {
                printf("%d", h.code[i]);
            }
            printf("\n");

            saveCodeword(h);
            printf("Codeword stored in file successfully.\n");

        } else if (choice == 2) {
            // Receiver side
            h = loadCodeword();

            int error = checkError(h);
            if (error == 0) {
                printf("No error in received data.\n");
            } else {
                printf("Error detected at bit position: %d\n", error);

                // Correct the error by flipping the bit
                h.code[error] ^= 1;

                // Print corrected codeword
                printf("Corrected Codeword: ");
                for (int i = h.total; i >= 1; i--) {
                    printf("%d", h.code[i]);
                }
                printf("\n");

                // Print corrected data (ignoring parity positions)
                printf("Corrected Data: ");
                for (int i = 1; i <= h.total; i++) {
                    if ((i & (i - 1)) != 0) { // skip parity positions
                        printf("%d", h.code[i]);
                    }
                }
                printf("\n");
            }

        } else if (choice == 3) {
            printf("Exiting program...\n");
        } else {
            printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 3);

    return 0;
}

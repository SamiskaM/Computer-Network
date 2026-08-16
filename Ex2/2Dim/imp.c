#include "head.h"

/*=========================================================
  SENDER SIDE
=========================================================*/
void sender(char input[], Packet *p)
{
    int i, j, k = 0;
    int count;
    int length = strlen(input);

    /* Calculate number of rows */
    p->rows = length / COL_SIZE;
    if(length % COL_SIZE != 0)
        p->rows++;

    /* Fill data matrix */
    for(i = 0; i < p->rows; i++) {
        for(j = 0; j < COL_SIZE; j++) {
            if(k < length)
                p->data[i][j] = input[k++];
            else
                p->data[i][j] = '0';   /* Padding */
        }
    }

    /*-----------------------------------------------------
      Row Parity (stored in last column)
    -----------------------------------------------------*/
    for(i = 0; i < p->rows; i++) {
        count = 0;
        for(j = 0; j < COL_SIZE; j++) {
            if(p->data[i][j] == '1')
                count++;
        }
        /* ODD parity */
        if(count % 2 == 0)
            p->data[i][COL_SIZE] = '1';
        else
            p->data[i][COL_SIZE] = '0';
    }

    /*-----------------------------------------------------
      Column Parity (stored in last row)
    -----------------------------------------------------*/
    for(j = 0; j <= COL_SIZE; j++) {
        count = 0;
        for(i = 0; i < p->rows; i++) {
            if(p->data[i][j] == '1')
                count++;
        }
        /* ODD parity */
        if(count % 2 == 0)
            p->data[p->rows][j] = '1';
        else
            p->data[p->rows][j] = '0';
    }

    /*-----------------------------------------------------
      Store packet in file
    -----------------------------------------------------*/
    FILE *fp = fopen("packet.txt", "w");
    if(fp == NULL) return;

    fprintf(fp, "%d\n", p->rows);
    for(i = 0; i <= p->rows; i++) {
        for(j = 0; j <= COL_SIZE; j++)
            fprintf(fp, "%c", p->data[i][j]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

/*=========================================================
  RECEIVER SIDE
=========================================================*/
int receiver(Packet *p)
{
    int i, j, count;

    FILE *fp = fopen("packet.txt", "r");
    if(fp == NULL) return -1;

    fscanf(fp, "%d", &p->rows);

    /* Read complete matrix including parity row */
    for(i = 0; i <= p->rows; i++) {
        for(j = 0; j <= COL_SIZE; j++)
            fscanf(fp, " %c", &p->data[i][j]);
    }
    fclose(fp);

    /*-----------------------------------------------------
      Verify each data row against its row parity
    -----------------------------------------------------*/
    for(i = 0; i < p->rows; i++) {
        count = 0;
        for(j = 0; j < COL_SIZE; j++) {
            if(p->data[i][j] == '1')
                count++;
        }
        if(count % 2 == 0) {
            if(p->data[i][COL_SIZE] != '1') return 1;
        } else {
            if(p->data[i][COL_SIZE] != '0') return 1;
        }
    }

    /*-----------------------------------------------------
      Verify each data column against its column parity
    -----------------------------------------------------*/
    for(j = 0; j < COL_SIZE; j++) {
        count = 0;
        for(i = 0; i < p->rows; i++) {
            if(p->data[i][j] == '1')
                count++;
        }
        if(count % 2 == 0) {
            if(p->data[p->rows][j] != '1') return 1;
        } else {
            if(p->data[p->rows][j] != '0') return 1;
        }
    }

    /*-----------------------------------------------------
      Verify RP column parity (bottom-right corner)
    -----------------------------------------------------*/
    count = 0;
    for(i = 0; i < p->rows; i++) {
        if(p->data[i][COL_SIZE] == '1')
            count++;
    }
    if(count % 2 == 0) {
        if(p->data[p->rows][COL_SIZE] != '1') return 1;
    } else {
        if(p->data[p->rows][COL_SIZE] != '0') return 1;
    }

    /* No error */
    return 0;
}

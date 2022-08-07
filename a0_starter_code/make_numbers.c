// add headers as appropriate
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>




int main(int argc, char *argv[])
{

    // time_t t;
    
    if (argc != 4)
    {
        printf("wrong argc");
        return 1;
    }
    FILE *fp = fopen("log.txt", "w+");

    if (fp == NULL)
    {
        printf("Error! cannot open and write to file");
        return 1;
    }
    int n = atoi(argv[1]);
    int lw = atoi(argv[2]);
    int hi = atoi(argv[3]);
    if(lw > hi){printf("Error! lo > hi!"); return 1;}
    if(hi > INT_MAX || lw < INT_MIN) {printf("Error! lw/hi out of bound"); return 1;}
    int range = hi - lw + 1;
    // printf("rannge is %d \n", range);
    fprintf(fp, "%d \n", n);

//to add more time
    for (int i = 0; i < 1000000; ++i){
        for (int j = 0; j < 1000000; ++j){
            for (int k = 0; j < 1000000; ++j){
                int a = m3*80*20;
            }
        }
    }

    srand((unsigned) time(NULL));
    for (int i = 0; i < n; ++i)
    {
        int rd = rand();
        // printf("%d \n", rd);
        rd = rd % range + lw;
        fprintf(fp, "%d \n", rd);
        // fputs("\n", fp);
    }
    fclose(fp);
    return 0;
}

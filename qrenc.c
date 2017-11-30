#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qrencode.h"

int main(int argc, char **argv)
{
    int x, y;
    int ret;

    x = 1;
    if( argc > 2 )
        x = atoi(argv[2]);
    y = 6;
    if( argc > 1 )
        y = atoi(argv[1]);

    y = initecc(x,y);
    // if size is known, pick frame, 
    //iiteccsize( ecc, strlen(s) );

    ret = fread(strinbuf, 1, y, stdin);
    strinbuf[ret] = '\0';

    initframe();
    qrencode();


    /* data */
#ifdef PAD
    printf("P1\n%d %d\n", WD + 8, WD + 8);
    for (y = 0; y < 4; y++) {
        for (x = 0; x < WD + 8; x++)
            printf("0 ");
        printf("\n");
    }
#else
    printf("P1\n%d %d\n", WD, WD);
#endif
    for (y = 0; y < WD; y++) {
#ifdef PAD
        for (x = 0; x < 4; x++)
            printf("0 ");
#endif
        for (x = 0; x < WD; x++)
            printf("%d ", QRBIT(x,y) );
#ifdef PAD
        for (x = 0; x < 4; x++)
            printf("0 ");
#endif
        printf("\n");
    }
#ifdef PAD
    for (y = 0; y < 4; y++) {
        for (x = 0; x < WD + 8; x++)
            printf("0 ");
        printf("\n");
    }
#endif
    return 0;
}

#include <stdio.h>

unsigned char jpeg0[] = {
    0xff, 0xd8,
    0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00,
    0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00,

    0xFF, 0xDB, 0x00, 0x43, 0x00,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,

    0xff, 0xc1, 0x00, 0x0b, 0x08,
    // address 0x5e
    0x00, 0x80,                 // height
    0x00, 0x80,                 // width
    0x01, 0x01, 0x11, 0x00,
    // DC table
    0xff, 0xc4, 0x00, 0x15, 0x00,
    1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 0,
    // AC Table
    0xff, 0xc4, 0x00, 0x14, 0x10,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,
    // Image scan header
    0xff, 0xda, 0x00, 0x08, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x00
};

#include "qrencode.h"

#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    unsigned char x, y, v = 0, l = 1;
    char *c;
    unsigned width, height, j, k;
    int argp = 0;

    if( argc < 2 ) {
        printf( "Usage:\n\nqrjpeg [-v VERS_1_40] [-l ECCLVL_1_4] \"text to encode\" >output.jpg\n" );
        printf( "version defaults to auto, same as \"-v 0\"\necc level defaults to 1\n" );
        return 1;
    }
    c = "Test Message";

    while( ++argp < argc ) {
        if( argv[argp][0] == '-' ) {
            if( argv[argp][1] == 'v' )
                v = atoi( argv[++argp]);
            else if( argv[argp][1] == 'l' )
                l = atoi( argv[++argp]);
            else {
                printf( "Usage:\n\nqrjpeg [-v VERS_1_40] [-l ECCLVL_1_4] \"text to encode\" >output.jpg\n" );
                printf( "version defaults to auto, same as \"-v 0\"\necc level defaults to 1\n" );
                return 1;
            }
        }
        else
            c = argv[argp];
    }
    if( v > 40 ) {
        fprintf( stderr, "Bad version (size) parameter (should be 0 (auto) to 40)\n" );
        return -1;
    }
    if( l < 1 || l > 4 ) {
        fprintf( stderr, "Bad ECC level parameter (should be 1 to 4)\n" );
        return -1;
    }

    if( v )
        k = initecc(l, v);
    else
        k = initeccsize( l, strlen(c));

    initframe();
    strcpy((char *)strinbuf, c );
    qrencode();

    width = height = WD+8;
    // set height and width in header
    jpeg0[0x5e] = width >> 5;
    jpeg0[0x5f] = width << 3;
    jpeg0[0x60] = width >> 5;
    jpeg0[0x61] = width << 3;
    // write out header
    fwrite(jpeg0, 1, sizeof(jpeg0), stdout);
    // put half full scale, 3e for white, 40 for black
    putchar(0x40);
    for (j = 0; j < width * 4 + 3; j++) 
        putchar(0x80);
    for (y = 0; y < WD; y++)  {
        k = 0;
        for (x = 0; x < WD; x++)  {
            j = QRBIT(x, y);
            if (k == j) {
                putchar(0x80);      // code for no change
                continue;
            }
            putchar(j ? 0 : 0x7e);   // full scale flip
            k = j;
        }
        if (k != 0)
            putchar(0x7e);
        else
            putchar(0x80);
        for (j = 0; j < 7; j++) 
            putchar(0x80);
    }
    for (j = 0; j < width * 4 - 4; j++) 
        putchar(0x80);
    putchar(0x80);              // one last for EOF
    putchar(0xFF);              // end marker
    putchar(0xd9);
    return 0;
}

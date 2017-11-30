#include <string.h>

extern void initframe(void);
extern void initecc(unsigned char, unsigned char);
extern unsigned char neccblk1;
extern unsigned char neccblk2 ;
extern unsigned char datablkw;
extern unsigned char eccblkwid;
extern unsigned char VERSION;
extern unsigned char ECCLEVEL;
extern unsigned char WD, WDB; // filled in from verison by initframe
extern unsigned char *framebase;
extern unsigned char *framask;

#ifndef __AVR__
#define PROGMEM
#define memcpy_P memcpy
#define __LPM(x) x
#else
#include <avr/pgmspace.h>
#endif

#include "ecctable.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    unsigned char i, j, b;

    if( argc != 3 )
        printf( "params: Version (1-40) ECC-level (1-4)" );

    unsigned ecc = atoi(argv[2]);
    if( ecc < 1 || ecc > 4 )
        return -1;
    unsigned char vers = atoi(argv[1]);
    if( vers > 40 ) 
        return -1;

    initecc(ecc, vers);

    printf( "const unsigned char neccblk1 = %d;\n", neccblk1 );
    printf( "const unsigned char neccblk2 = %d;\n", neccblk2  );
    printf( "const unsigned char datablkw = %d;\n", datablkw );
    printf( "const unsigned char eccblkwid = %d;\n", eccblkwid );
    printf( "const unsigned char VERSION = %d;\n", VERSION );
    printf( "const unsigned char ECCLEVEL = %d;\n", ECCLEVEL );
    printf( "const unsigned char WD = %d;\n", WD );
    printf( "const unsigned char WDB = %d;\n", WDB );
    printf( "unsigned char strinbuf[%d];\n", WD*WDB );//(datablkw + eccblkwid) * (neccblk1 + neccblk2) + neccblk2 );
    printf( "unsigned char qrframe[%d];\n", WD*WDB < 600? 600 : WD*WDB );
    printf( "unsigned char rlens[%d];\n", WD+1 );

    printf( "#ifndef __AVR__\n#define PROGMEM\n#define memcpy_P memcpy\n#define __LPM(x) *x\n#else\n"
            "#include <avr/pgmspace.h>\n#endif\nconst unsigned char framebase[] PROGMEM = {\n" );

    initframe();

    for (j = 0; j < WD; j++) {
        for (i = 0; i < WDB; i++) {
            b = framebase[j*WDB+i];
            printf("0x%02x,",  b );
        }
        printf("\n");
    }
    printf( "};\n\nconst unsigned char framask[] PROGMEM = {" );
    unsigned tt, tri = WD*(WD+1)/2;
    tri = (tri+7)/8;
    for( tt = 0; tt < tri; tt++ ) {
        if( !(tt % WDB) )
            printf("\n");
        printf("0x%02x,",  framask[tt] );
    }
    printf( "\n};\n" );

    return 0;
}

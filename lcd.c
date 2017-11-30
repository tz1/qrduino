#include <avr/io.h>
#include <util/delay.h>
void LcdWrite(unsigned char dc, unsigned char data)
{
    int i;
    if( dc )
        PORTD |= 0x20;
    else
        PORTD &= ~0x20;
    PORTD &= ~0x80;
     for( i = 0 ; i < 8 ; i++ ) {
        if( (data >> (7 - i)) & 1 )
            PORTD |= 16;
        else
            PORTD &= ~16;
        PORTD |= 8;
        PORTD &= ~8;
     }
     PORTD |= 0x80;
     PORTB ^= 0x20;
}

#include "qrencode.h"

#include <string.h>
int main()
{
    DDRB |= 0x20;
    PORTB |= 0xf8;

    DDRD |= 0xf8;
    PORTD &= ~8;
    PORTD &= ~0x40;
    PORTD |= 0x40;
    unsigned char b = 0x55;
    unsigned i, k, t;
    LcdWrite(0, 0x22);
    LcdWrite(0, 0x0C);
    for ( k = 0; k < 84 ; k++, b ^= 0xff )
        for ( i = 0; i < 6 ; i++)
                LcdWrite(1, b);

    strcpy((char *)strinbuf, "http://harleyhacking.blogspot.com");
    qrencode();


    LcdWrite(0, 0x22);
    LcdWrite(0, 0x0C);
    for ( k = 0; k < 84 ; k++ )
        for ( i = 0; i < 6 ; i++)
                LcdWrite(1, 0);

    PORTB &= ~0x20;
    t = 0;
    b = 0;
    for ( k = 0; k < 84 ; k++) {
        for ( i = 0; i < 48 ; i++) {
            b >>= 1;
            if( i < WD && k < WD )
                if( QRBIT(WD-i-1,k) )
                    b |= 0x80;
            if( ++t > 7 ) {
                t = 0;
                LcdWrite(1, b);
                b = 0;
            }
        }
    }
    for(;;);
}

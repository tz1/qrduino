CFLAGS:=-Wall -g -O6
#CC=avr-gcc -mmcu=atmega328p
CC=gcc

all: qrencode qrjpeg

qrjpeg: qrjpeg.o qrencode.o qrframe.o

dofbit: dofbit.o qrframe.o

qrencode: qrenc.o qrencode.o qrframe.o

clean:
	rm -rf qrencode *.o qrjpeg dofbit

realclean: clean
	rm -rf *~ \#*


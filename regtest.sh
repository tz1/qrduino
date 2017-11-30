#!/bin/bash
make
#for i in 1 2 3 4 5 6 7 8 9 10 11 12; do 
i=1;
while [ $i -ne 41 ]; do
#for i in 9 10 11 12; do 
    for j in 1 2 3 4; do 
        echo $i $j
        echo "Hello" | ./qrencode $i $j >Hello$i$j.pbm
        diff -q Hello$i$j.pbm regout/
    done
i=$[$i+1]
done
echo
#md5sum Hello??.pbm base/Hello??.pbm | sort | uniq -c -w32 | sort -nr

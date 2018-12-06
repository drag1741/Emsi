# Emsi

***to get zlib.h to work***
must install zlibc from debian repository
LD_PRELOAD=/lib/uncompress.so
export LD_PRELOAD

must compile gcc with -lz switch to link zlib.h

gcc -lz main.c

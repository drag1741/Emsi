# Emsi

Program tested on fresh install of Debian 9.5.0 running as VM in KVM

apt install from deb repository: gcc, make, zlib1g-dev

To compile: run make

Included Makefile will compile from files in directory.

Input datafile is read as cmdline argument to ./a.out:

$ ./a.out datafile.gz

Output sqlite3 db will be called dataline.db. There are 3 tables created but the relevant one is called dataline. The summary for the requirements is sqlite3 calls to dataline table from main.c and printed to stdout.

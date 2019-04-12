all:
	gcc src/*.c -o shs.out -Wall -Wextra -Wpedantic -std=gnu11 -O2 -Iinclude -lALF_std -Llib

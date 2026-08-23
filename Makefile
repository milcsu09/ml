
all:
	cc -std=c11 -Wall -Wextra -Wpedantic -Wno-unused -O3 -oa $(wildcard src/*.c) -lm


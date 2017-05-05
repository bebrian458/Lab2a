CC = gcc
CFLAGS =


build: lab2_add

lab2_add: lab2_add.c
	$(CC) $< -o $@

tests: build

graphs:

dist:

clean:
	rm -f lab2_add


# build:

# tests: Makefile build
# 	./lab3_add thread=2 iterations=100 >> lab2_add.csv

# graphs: Makefile build
# 	gnuplot lab2_add.gp
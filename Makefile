CC = gcc
DEBUG = 
LFLAGS = -pthread

# source code
ADD = lab2_add.c
LIST = lab2_list.c SortedList.c

build: lab2_add lab2_list

lab2_add: $(ADD)
	$(CC) $(LFLAGS) $(DEBUG) -o $@ $(ADD)

lab2_list: $(LIST) SortedList.h
	$(CC) $(LFLAGS) $(DEBUG) -o $@ $(LIST)

tests: build

graphs: build

dist:

clean:
	rm -f lab2_add lab2_list


# build:

# tests: Makefile build
# 	./lab3_add thread=2 iterations=100 >> lab2_add.csv

# graphs: Makefile build
# 	gnuplot lab2_add.gp
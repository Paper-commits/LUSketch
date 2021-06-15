GCC = g++
CFLAGS = -std=gnu++11 -Wall -g
SEEFLAGS = -march=native -msse2
FILES = test.out

all:$(FILES)

test.out:test.cpp
	$(GCC) $(CFLAGS) $(SEEFLAGS) -g -o test.out test.cpp

debug:

clean:
	rm $(all) -f *~ *.o *.out *.txt

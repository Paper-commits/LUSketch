GCC = g++
CFLAGS = -std=gnu++11 -Wall -g
SEEFLAGS = -march=native -msse2
FILES = main.out

all:$(FILES)

main.out:main.cpp
	$(GCC) $(CFLAGS) $(SEEFLAGS) -g -o main.out main.cpp

debug:

clean:
	rm $(all) -f *~ *.o *.out *.txt

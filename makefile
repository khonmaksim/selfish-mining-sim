CC = gcc
CFLAGS = -Wall -O2

main: SelfishMiningMC.o
	$(CC) $(CFLAGS) -o main SelfishMiningMC.o

SelfishMiningMC.o: SelfishMiningMC.c
	$(CC) $(CFLAGS) -c SelfishMiningMC.c

clean:
	rm -rf main SelfishMiningMC.o

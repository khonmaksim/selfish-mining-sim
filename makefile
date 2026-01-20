CC = gcc
CFLAGS = -Wall -O2

SelfishMiningMC: SelfishMiningMC.o
	$(CC) $(CFLAGS) -o SelfishMiningMC SelfishMiningMC.o

SelfishMiningMC.o: SelfishMiningMC.c
	$(CC) $(CFLAGS) -c SelfishMiningMC.c

clean:
	rm -rf SelfishMiningMC SelfishMiningMC.o

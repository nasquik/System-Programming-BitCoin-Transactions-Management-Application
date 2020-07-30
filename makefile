OBJS = main.o auxFunctions.o functions.o srHashTable.o wallet.o bitCoinHashTable.o hashFunctions.o transactionHashTable.o walletHashTable.o bitCoinTree.o transactionList.o
SOURCE = main.c auxFunctions.c functions.c srHashTable.c wallet.c bitCoinHashTable.c hashFunctions.c transactionHashTable.c walletHashTable.c bitCoinTree.c transactionList.c
HEADER = auxFunctions.h functions.h srHashTable.h wallet.h bitCoinHashTable.h hashFunctions.h transactionHashTable.h walletHashTable.h bitCoinTree.h transactionList.h

OUT = bitcoin
CC = gcc
FLAGS = -g -c

$(OUT) : $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

main.o : main.c
	$(CC) $(FLAGS) main.c

auxFunctions.o : auxFunctions.c
	$(CC) $(FLAGS) auxFunctions.c

functions.o : functions.c
	$(CC) $(FLAGS) functions.c

srHashTable.o : srHashTable.c
	$(CC) $(FLAGS) srHashTable.c

wallet.o : wallet.c
	$(CC) $(FLAGS) wallet.c

bitCoinHashTable.o : bitCoinHashTable.c
	$(CC) $(FLAGS) bitCoinHashTable.c

hashFunctions.o : hashFunctions.c
	$(CC) $(FLAGS) hashFunctions.c

transactionHashTable.o : transactionHashTable.c
	$(CC) $(FLAGS) transactionHashTable.c

walletHashTable.o : walletHashTable.c
	$(CC) $(FLAGS) walletHashTable.c

bitCoinTree.o : bitCoinTree.c
	$(CC) $(FLAGS) bitCoinTree.c

transactionList.o : transactionList.c
	$(CC) $(FLAGS) transactionList.c

clean :
	rm -f $(OBJS) $(OUT)
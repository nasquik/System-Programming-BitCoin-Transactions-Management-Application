#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "wallet.h"
#include "transactionList.h"
#include "walletHashTable.h"
#include "bitCoinHashTable.h"
#include "transactionHashTable.h"
#include "srHashTable.h"
#include "functions.h"

int main(int argc, char* argv[]){

    //////// ARGUMENTS ////////

    if(argc < 13){
        printf("Incorrect Input!\n");
        return -1;
    }

    char bBF[26];
    char transF[26];
    int value = 0;
    int senderEntries = 0;
    int receiverEntries = 0;
    int bucketSize = 0;

    for(int i = 1; i <= argc-1; i+=2){

        if(strcmp(argv[i], "-a") == 0)
            strcpy(bBF, argv[i+1]);
        else if(strcmp(argv[i], "-t") == 0)
            strcpy(transF, argv[i+1]);
        else if(strcmp(argv[i], "-v") == 0)
            value = atoi(argv[i+1]);
        else if(strcmp(argv[i],"-h1") == 0)
            senderEntries = atoi(argv[i+1]);
        else if(strcmp(argv[i], "-h2") == 0)
            receiverEntries = atoi(argv[i+1]);
        else if(strcmp(argv[i], "-b") == 0)
            bucketSize = atoi(argv[i+1]);
    }

    //////// OPEN FILES ////////

    FILE* bBFfile = NULL;
    FILE* transFile = NULL;
    bBFfile = fopen(bBF, "r");
    transFile = fopen(transF, "r");

    if(bBFfile == NULL || transFile == NULL){
        printf("Error opening file(s)\n");
        exit(EXIT_FAILURE);
    }

    //////// COUNT WALLETS, BITCOINS AND TRANSACTIONS ////////

    int walletCount = 0;
    int bitCoinCount = 0;
    int transactionCount = 0;

    getNumbers(bBFfile, transFile, &walletCount, &bitCoinCount, &transactionCount);

    fclose(bBFfile);
    fclose(transFile);

    //////// INITIALIZE WALLET/BITCOIN/TRANSACTION HASHTABLES ////////

    easyAccess* EA = easyAccessInit();
    int walletBuckets = walletCount / 3 + 1;
    int bitCoinBuckets = bitCoinCount / 3 + 1;
    int transactionBuckets = transactionCount / 3 + 1;

    EA->wallets = walletHashCreate(walletBuckets);
    EA->bitCoins = bitCoinHashCreate(bitCoinBuckets);
    EA->transactions = transactionHashCreate(transactionBuckets);

    bBFfile = fopen(bBF, "r");
    transFile = fopen(transF, "r");

    if(bBFfile == NULL || transFile == NULL){
        printf("Error opening file(s)\n");
        exit(EXIT_FAILURE);
    }
    
    int bitCoinID = 0;
    wallet* Wallet = NULL;

    printf("\nEXTRACTING DATA FROM BITCOIN BALANCE FILE...\n\n");

    char* line = NULL;
    size_t len = 0;
    char* token;

    while(getline(&line, &len, bBFfile) != -1){

        printf("%s", line);
        token = strtok(line, " \r\n");

        if(token == NULL)
            break;
        
        if(walletHashGetEntry(EA->wallets, token) != NULL)              // if walletID already exists, skip this line
            continue;

        Wallet = walletInit(token);

        do{
            token = strtok(NULL, " \r\n");
            if(token != NULL){
                bitCoinID = atoi(token);
                if(bitCoinHashInsert(EA->bitCoins, bitCoinID, Wallet->ownerID, value))      // add bitCoin to the HashTable
                    bclistInsert(Wallet->bitCoinList, bitCoinID, value);                    // add bitCoin to walletID's wallet
                else{
                    printf("\nError! BitCoinID %d is registered to a different walletID.\n\n", bitCoinID);  // if wallet is already registered, stop the application
                    walletDelete(Wallet);
                    goto delete;
                }
                
            }
        }
        while(token!= NULL);

        walletHashInsert(EA->wallets, Wallet);          // add wallet to walletsHashTable
    }

    fclose(bBFfile);
    bBFfile = NULL;

    //////// INITIALIZE SENDER/RECEIVER HASHTABLES ////////

    printf("\nEXTRACTING DATA FROM TRANSACTIONS FILE...\n\n");

    char transactionID[51], senderID[51], receiverID[51];
    int sum;
    dateAndTime current;

    transaction* Transaction;
    wallet* sender;
    wallet* receiver;
    EA->senders = srHashCreate('s', senderEntries, bucketSize);
    EA->receivers = srHashCreate('r', receiverEntries, bucketSize);

    // record all transactions from file

    while(getline(&line, &len, transFile) != -1){

        printf("%s", line);
        sscanf(line, "%s %s %s %d %d-%d-%d %d:%d", transactionID, senderID, receiverID, &sum, &current.day, &current.month, &current.year, &current.hours, &current.minutes);

        sender = walletHashGetEntry(EA->wallets, senderID);
        receiver = walletHashGetEntry(EA->wallets, receiverID);

        if(sender != NULL && receiver != NULL){
            Transaction = transactionCreate(transactionID, sender->ownerID, receiver->ownerID, sum, current);
            requestTransaction(Transaction, EA, 1);
        }
        else{
            printf("\nTransaction could not be logged! One or more of these users is not in the DB.\n\n");
        }
    }

    fclose(transFile);
    transFile = NULL;

    //////// ACT ON USER INPUT ////////

    printf("\nREADY FOR USER INPUT!\n\n");

    while(getline(&line, &len, stdin) != -1){

        token = strtok(line, " \r\n");

        if(token == NULL){
            printf("\nInvalid user input! Please ensure you are typing in the commands properly!\n\n");
        }
        else if(strcmp(token, "requestTransaction") == 0){
            callRequestTransaction(line, EA);
        }
        else if(strcmp(token, "requestTransactions") == 0){
            callRequestTransactions(line, EA);
        }
         else if(strcmp(token, "findEarnings") == 0){
            callFindPaymentsOrEarnings(line, EA, 'e');
         }
         else if(strcmp(token, "findPayments") == 0){
            callFindPaymentsOrEarnings(line, EA, 'p');
         }
         else if(strcmp(token, "walletStatus") == 0){
            callWalletStatus(line, EA);
         }
         else if(strcmp(token, "bitCoinStatus") == 0){
            callBitCoinStatus(line, EA);
         }
         else if(strcmp(token, "traceCoin") == 0){
            callTraceCoin(line, EA);
         }
         else if(strcmp(token, "exit") == 0){
            printf("\nBye! :)\n\n");
            break;
         }
         else{
            printf("\nInvalid user input! Please ensure you are typing in the commands properly!\n\n");
         }
    }

    //////// DELETE EVERYTHING ////////

delete:    

    if(line)
        free(line);

    if(bBFfile != NULL)
        fclose(bBFfile);

    if(transFile != NULL)
        fclose(transFile);

    if(EA->wallets)
        walletHashDelete(EA->wallets);
    if(EA->bitCoins)
        bitCoinHashDelete(EA->bitCoins);
    if(EA->transactions)
     transactionHashDelete(EA->transactions);
    if(EA->senders)
     srHashDelete(EA->senders);
    if(EA->receivers)
     srHashDelete(EA->receivers);
     free(EA);

    printf("\nThe application was terminated\n\n");

    return 0;
}
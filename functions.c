#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "wallet.h"
#include "transactionList.h"
#include "walletHashTable.h"
#include "bitCoinTree.h"
#include "bitCoinHashTable.h"
#include "transactionHashTable.h"
#include "srHashTable.h"
#include "auxFunctions.h"
#include "functions.h"

easyAccess* easyAccessInit(){
    // initialize the auxiliary struct that holds pointers to all the hashtables

    easyAccess* EA = malloc(sizeof(easyAccess));

    EA->bitCoins = NULL;
    EA->wallets = NULL;
    EA->transactions = NULL;
    EA->senders = NULL;
    EA->receivers = NULL;

    return EA;
}

void getNumbers(FILE* balancesFile, FILE* transactionsFile, int* wallets, int* bitCoins, int* transactions){
    // read the files provided by the user from command line and count the wallets, bitCoins and transactions
    // to facilitate the creation of the hashtables without wasting space

    char* line = NULL;
    size_t len = 0;
    char* token;

    while(getline(&line, &len, balancesFile) != -1){

        token = strtok(line, " \r\n");
        (*wallets)++;

        do{
            token = strtok(NULL, " \r\n");
            if(token != NULL)
                (*bitCoins)++;
        }
        while(token!= NULL);
    }

    while(getline(&line, &len, transactionsFile) != -1){
        (*transactions)++;
    }

    if(line)
        free(line);
}

int requestTransaction(transaction* Transaction, easyAccess* EA, char boolean){
    // run some checks to determine if a transaction is valid, perform it and update appropriate values

    if(boolean == 1 && transactionInHashTable(EA->transactions, Transaction->transactionID)){   // since the transactions created by functions
        free(Transaction);                                                                      // have unique IDs this check is performed only
        printf("Error! TransactionID already exists! Transaction was not logged.\n\n");         //when initializing databases
        return 0;
    }

    int ID;

    if(assertIntness(Transaction->transactionID)){          // if transaction is an integer, then compare it to the 
        ID = atoi(Transaction->transactionID);              // biggest transactionID that was also and integer and then
        if(ID > EA->transactions->biggestInt)               // potentially replace it
            EA->transactions->biggestInt = ID;              // this is done to ensure the uniqueness of the transaction IDs later on
    }

    wallet* sender = walletHashGetEntry(EA->wallets, Transaction->senderID);

    if(sender == NULL){
        free(Transaction);
        printf("Error! Sender not found! Transaction was not logged.\n\n");
        return 0;
    }

    wallet* receiver = walletHashGetEntry(EA->wallets, Transaction->receiverID);

    if(receiver == NULL){
        free(Transaction);
        printf("Error! Receiver not found! Transaction was not logged.\n\n");
        return 0;
    }

    int value = Transaction->value;

    if(bclistGetTotalMoney(sender->bitCoinList) >= Transaction->value){                          // if sender has enough money in their wallet
        if(compareDatesAndTimes(&Transaction->currentDateAndTime, &EA->transactions->lastDate)){    // if the new transaction's date is later than the last date logged

            srHashInsert(EA->senders, sender, Transaction);
            srHashInsert(EA->receivers, receiver, Transaction);

            treeRoot* root;
            bcnode* node = sender->bitCoinList->head;
            int oldValue = 0;

            while(node != NULL && value > 0){
                oldValue = value;
                root = bitCoinHashGetEntry(EA->bitCoins, node->bitCoinID);              //find bitCoinTree
                recordTransaction(root, Transaction, &value);                           // record the transaction on the tree
                if(boolean == 1)
                    transactionHashInsert(EA->transactions, Transaction->transactionID);    // add transaction ID to the hash table of existing IDs(only when reading files to initialize databases)
                bclistInsert(receiver->bitCoinList, node->bitCoinID, oldValue - value); // add bitCoin(or update its existing value) to the receiver's wallet
                node->amount -= oldValue - value;                                       // update the value of the bitCoin the sender used for the transaction
                if(node->amount == 0){                                                  // if value is 0 remove it from sender's wallet
                    node = node->next;
                    bclistPop(sender->bitCoinList);
                    continue;
                }
                node = node->next;
            }
            // bclistPrint(sender->bitCoinList);
            // bclistPrint(receiver->bitCoinList);
            printf("Transaction logged successfully!\n\n");
            EA->transactions->lastDate = Transaction->currentDateAndTime;
            return 1;
        }
        else{
            free(Transaction);
            printf("Error! Input date is old! Transaction was not logged.\n\n");
            return 0;   
        }
    }
    else{
        free(Transaction);
        printf("Error! Sender does not have enough money! Transaction was not logged.\n\n");
        return 0;
    }
}

int callRequestTransaction(char* line, easyAccess* EA){
    // user can type in either a complete transaction or the sender/receiver IDs and the amount to be transferred
    // function accepts both options
        
    char* token = NULL;
    wallet* senderWallet;
    wallet* receiverWallet;
    int count = 0;
    int value = 0;

    char buffer[5][51];
    int i = 0;

    token = strtok(NULL, " \r\n");

    while(token != NULL){                       // count arguments and save them in a buffer
        strcpy(buffer[i], token);
        token = strtok(NULL, " \r\n");
        count++;
        i++;
    }

    senderWallet = walletHashGetEntry(EA->wallets, buffer[0]);
    if(senderWallet == NULL){
        printf("Error! Sender Wallet does not exist. Transaction was not logged.\n\n");
        return 0;
    }

    receiverWallet = walletHashGetEntry(EA->wallets, buffer[1]);
    if(receiverWallet == NULL){
        printf("Error! Receiver Wallet does not exist. Transaction was not logged.\n\n");
        return 0;
    }

    if(assertIntness(buffer[2]))
        value = atoi(buffer[2]);
    else{
        printf("Error! Value given is not an integer. Transaction was not logged.\n\n");
        return 0;
    }

    EA->transactions->biggestInt++;                             // create unique transactio ID
    char newBiggest[11];
    sprintf(newBiggest, "%d", EA->transactions->biggestInt);
    transaction* Transaction;
    struct dateAndTime current;

    if(count == 5){

        // user typed in a complete transaction, read all the arguments and perform it

        if(!isDate(buffer[3])){
            printf("Error! The given date's format is incorrect. Transaction was not logged.\n\n");
            return 0;
        }
        sscanf(buffer[3], "%d-%d-%d", &current.day, &current.month, &current.year);

        if(!isTime(buffer[4])){
            printf("Error! The given time's format is incorrect. Transaction was not logged.\n\n");
            return 0;
        }

        sscanf(buffer[4], "%d:%d", &current.hours, &current.minutes);
    }
    else if(count == 3){

        // user typed in only the IDs and the value, set transaction's date and time to the current date and time and perform it

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        createDateAndTime(&current, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
    }
    else{
        printf("Error! Incorrect input. Transaction was not logged.\n\n");
        return 0;
    }

    Transaction = transactionCreate(newBiggest, senderWallet->ownerID, receiverWallet->ownerID, value, current);
    if(requestTransaction(Transaction, EA, 0))
        return 1;
    else
        EA->transactions->biggestInt--;
}

int callRequestTransactions(char* line, easyAccess*EA){
    // user either types in multiple transactions or the name of a file to extract transactions from
        
    char* token = NULL;
    int count = 0;
    char buffer[5][51];
    int i = 0;

    token = strtok(NULL, " \r\n");

    while(token != NULL){                       // count arguments and save them in a buffer
        strcpy(buffer[i], token);
        token = strtok(NULL, " ;\r\n");
        count++;
        i++;
    }

    wallet *sender = NULL, *receiver = NULL;
    char newBiggest[11];
    transaction* Transaction;

    if(count == 5){

        // user has chosen to type in multiple transactions, read first one and then wait for user input and 
        // read the rest until user presses ENTER two times

        int value = 0;

        sender = walletHashGetEntry(EA->wallets, buffer[0]);

        if(sender == NULL){
            printf("Error! Sender Wallet does not exist. Transaction was not logged.\n\n");
            return 0;
        }

        receiver = walletHashGetEntry(EA->wallets, buffer[1]);

        if(receiver == NULL){
            printf("Error! Receiver Wallet does not exist. Transaction was not logged.\n\n");
            return 0;
        }

        if(assertIntness(buffer[2]))
            value = atoi(buffer[2]);
        else{
            printf("Error! Value given is not an integer. Transaction was not logged.\n\n");
            return 0;
        }

        EA->transactions->biggestInt++;
        sprintf(newBiggest, "%d", EA->transactions->biggestInt);
        struct dateAndTime current;

        if(!isDate(buffer[3])){
            printf("Error! The given date's format is incorrect. Transaction was not logged.\n\n");
            return 0;
        }
        sscanf(buffer[3], "%d-%d-%d", &current.day, &current.month, &current.year);

        if(!isTime(buffer[4])){
            printf("Error! The given time's format is incorrect. Transaction was not logged.\n\n");
            return 0;
        }

        sscanf(buffer[4], "%d:%d", &current.hours, &current.minutes);
 
        Transaction = transactionCreate(newBiggest, sender->ownerID, receiver->ownerID, value, current);
        if(!requestTransaction(Transaction, EA, 0))
            EA->transactions->biggestInt--;

        char* comLine = NULL;
        size_t len;

        // do the following until user changes line two times

        while(getline(&comLine, &len, stdin) != -1){

            i = 0;
            token = strtok(comLine, " ;\r\n");

            if(token == NULL){
                if(comLine)
                    free(comLine);
                return -1;
            }

            while(token != NULL && i <= 4){
                strcpy(buffer[i], token);
                token = strtok(NULL, " ;\r\n");
                i++;
            }

            sender = walletHashGetEntry(EA->wallets, buffer[0]);

            if(sender == NULL){
                printf("Error! Sender Wallet does not exist. Transaction was not logged.\n\n");
                continue;
            }

            receiver = walletHashGetEntry(EA->wallets, buffer[1]);

            if(receiver == NULL){
                printf("Error! Receiver Wallet does not exist. Transaction was not logged.\n\n");
                continue;
            }

            if(assertIntness(buffer[2]))
                value = atoi(buffer[2]);
            else{
                printf("Error! Value given is not an integer. Transaction was not logged.\n\n");
                continue;
            }

            EA->transactions->biggestInt++;
            sprintf(newBiggest, "%d", EA->transactions->biggestInt);
            struct dateAndTime current;

            if(!isDate(buffer[3])){
                printf("Error! The given date's format is incorrect. Transaction was not logged.\n\n");
                continue;
            }
            sscanf(buffer[3], "%d-%d-%d", &current.day, &current.month, &current.year);

            if(!isTime(buffer[4])){
                printf("Error! The given time's format is incorrect. Transaction was not logged.\n\n");
                continue;
            }

            sscanf(buffer[4], "%d:%d", &current.hours, &current.minutes);
    
            Transaction = transactionCreate(newBiggest, sender->ownerID, receiver->ownerID, value, current);
            if(!requestTransaction(Transaction, EA, 0))
                EA->transactions->biggestInt--;
        }
        if(comLine)
            free(comLine);
        return 1;
    }
    else if(count == 1){

        // used typed in the name of a file

        char* fileLine = NULL;
        size_t len;
        char senderID[51], receiverID[51];
        int sum;
        dateAndTime current;
        transaction* Transaction;

        FILE* transactionsFile = NULL;
        transactionsFile = fopen(buffer[0], "r");

        if(transactionsFile == NULL){
            printf("Error! Could not open file.\n\n");
            return 0;
        }

        // traverse the whole file, read and perform transactions

        while(getline(&fileLine, &len, transactionsFile) != -1){

            sscanf(fileLine, "%s %s %d %d-%d-%d %d:%d;", senderID, receiverID, &sum, &current.day, &current.month, &current.year, &current.hours, &current.minutes);

            sender = walletHashGetEntry(EA->wallets, senderID);
            receiver = walletHashGetEntry(EA->wallets, receiverID);

            if(sender != NULL && receiver != NULL){

                EA->transactions->biggestInt++;                                 // create unique transaction ID
                sprintf(newBiggest, "%d", EA->transactions->biggestInt);
                Transaction = transactionCreate(newBiggest, sender->ownerID, receiver->ownerID, sum, current);
                transactionPrint(Transaction);
                if(!requestTransaction(Transaction, EA, 0))
                    EA->transactions->biggestInt--;
            }
            else{
                printf("Error! Sender and/or Receiver WalletID not found. Transaction was not logged.\n\n");
                fclose(transactionsFile);
            }
        }
        fclose(transactionsFile);
        if(fileLine)
            free(fileLine);
        return 1;
    }
    else{
        printf("Error! Incorrect input.\n\n");
        return 0;
    }
}

void callWalletStatus(char* line, easyAccess* EA){
    // print the amount of money the wallet given contains

    char* ID;

    ID = strtok(NULL, " \r\n");

    if(ID != NULL){

        wallet* Wallet = walletHashGetEntry(EA->wallets, ID);

        if(Wallet){
            printf("WalletID %s contains %d dollars.\n\n", Wallet->ownerID, bclistGetTotalMoney(Wallet->bitCoinList));
            return;
        }
        else
            printf("Error! This WalletID does not exist.\n\n");
    }
    else{
        printf("Error! Incorrect input.\n\n");
        return;
    }
}

void callTraceCoin(char* line, easyAccess* EA){
    // print transaction history of the given bitCoin
        
    int ID;
    char* token;

    token = strtok(NULL, " \r\n");

    if(token == NULL){
        printf("Error! Incorrect input.\n\n");
        return;    
    }

    if(assertIntness(token))
        ID = atoi(token);
    else{
        printf("Error! BitCoinID given is not an integer.\n\n");
        return;
    }

    treeRoot* root = bitCoinHashGetEntry(EA->bitCoins, ID);

    if(root != NULL){
        printf("BitCoinID %d has been used in the following transactions:\n\n", ID);
        printBitCoinHistory(root);
    }
    else
        printf("Error! This bitCoin does not exist.\n\n");
}

void callBitCoinStatus(char* line, easyAccess* EA){
    // print information about the given wallet
    
    int ID;
    char* token;

    token = strtok(NULL, " \r\n");

    if(token == NULL){
        printf("Error! Incorrect input.\n\n");
        return;    
    }

    if(assertIntness(token))
        ID = atoi(token);
    else{
        printf("Error! BitCoinID given is not an integer.\n\n");
        return;
    }

    treeRoot* root = bitCoinHashGetEntry(EA->bitCoins, ID);

    if(root != NULL){
        printf("\n");
        getBitCoinStatus(root);
        printf("\n");
    }
    else
        printf("Error! This bitCoin does not exist.\n\n");
}

int callFindPaymentsOrEarnings(char* line, easyAccess* EA, char type){
    // depending on the char type given, either print the transactions in which the given walletID is the sender
    // and the money it has paid, or print the transactions in which the given walletID is the receiver and the money it has made

    char* token = NULL;
    wallet* Wallet;
    int count = 0;
    int value = 0;

    char buffer[5][51];
    int i = 0;

    token = strtok(NULL, " \r\n");

    while(token != NULL){
        strcpy(buffer[i], token);
        token = strtok(NULL, " \r\n");
        count++;
        i++;
    }

    Wallet = walletHashGetEntry(EA->wallets, buffer[0]);

    if(Wallet == NULL){
        printf("Error! Wallet does not exist. Transaction was not logged.\n\n");
        return 0;
    }

    transactionList* list;

    if(type == 'e'){
        list = srHashGetTransactionList(EA->receivers, buffer[0]); // wallet is the receiver
    }
    else if(type == 'p'){
        list = srHashGetTransactionList(EA->senders, buffer[0]);  // wallet is the sender
    }

    if(list == NULL){
        printf("Error! WalletID not in Hash Table. Transaction was not logged.\n\n");
        return 0;
    }

    transactionNode* tnode = list->head;
    transaction* trans;
    int flag = 0;
    int sum = 0;

    if(count == 5){

        //user typed in two times and two dates

        dateAndTime first;
        dateAndTime second;

        // determine all arguments are correct

        if(!isTime(buffer[1])){
            printf("Error! The given time's format is incorrect. Transaction was not logged.\n\n");
            return 0;
        }
        sscanf(buffer[1], "%d:%d", &first.hours, &first.minutes);

        if(!isDate(buffer[2])){
            printf("Error! The given date's format is incorrect. Transaction was not logged.\n\n");
            return 0;
        }
        sscanf(buffer[2], "%d-%d-%d", &first.day, &first.month, &first.year);

        if(!isTime(buffer[3])){
            printf("Error! The given time's format is incorrect. Transaction was not logged.\n\n");
            return 0;
        }
        sscanf(buffer[3], "%d:%d", &second.hours, &second.minutes);

        if(!isDate(buffer[4])){
            printf("Error! The given date's format is incorrect. Transaction was not logged.\n\n");
            return 0;
        }
        sscanf(buffer[4], "%d-%d-%d", &second.day, &second.month, &second.year);

        // only print transactions made in the span between the two dates given
        // from those print only those made in the span between the times given

        while(tnode != NULL){
            trans = tnode->currentTransaction;
            if(compareDates(&trans->currentDateAndTime, &first) && compareDates(&second, &trans->currentDateAndTime)){
                if(compareTimes(&trans->currentDateAndTime, &first) && compareTimes(&second, &trans->currentDateAndTime)){
                    transactionPrint(trans);
                    sum += trans->value;
                    flag = 1;
                }
            }
            tnode = tnode->next;
        }
        if(flag){
            if(type == 'p'){
                printf("The payments walletID %s has made in the specified time span amount to %d dollars.\n\n", Wallet->ownerID, sum);
            }
            else{
                printf("The earnings walletID %s has gathered in the specified time span amount to %d dollars.\n\n", Wallet->ownerID, sum);                  
            }
            return 1; 
        }
        else{
            if(type == 'p'){
                printf("No payments by walletID %s were found in the specified time span.\n\n", Wallet->ownerID);
            }
            else{
                printf("No earnings for walletID %s were found in the specified time span.\n\n", Wallet->ownerID);                  
            }
            return 0;
        }
    }
    else if(count == 3){

        // user typed in either two times or two dates

        dateAndTime first;
        dateAndTime second;

        if(isTime(buffer[1]) && isTime(buffer[2])){ // if they typed in two times

            sscanf(buffer[1], "%d:%d", &first.hours, &first.minutes);
            sscanf(buffer[2], "%d:%d", &second.hours, &second.minutes);

            // only print transactions made in the span between those times

            while(tnode != NULL){
                trans = tnode->currentTransaction;
                if(compareTimes(&trans->currentDateAndTime, &first) && compareTimes(&second, &trans->currentDateAndTime)){
                    transactionPrint(trans);
                    sum += trans->value;
                    flag = 1;
                }
                tnode = tnode->next;
            }
            if(flag){
                if(type == 'p'){
                    printf("The payments walletID %s has made in the specified time span amount to %d dollars.\n\n", Wallet->ownerID, sum);
                }
                else{
                    printf("The earnings walletID %s has gathered in the specified time span amount to %d dollars.\n\n", Wallet->ownerID, sum);                  
                }
                return 1;  
            }
            else{
                if(type == 'p'){
                    printf("No payments by walletID %s were found in the specified time span.\n\n", Wallet->ownerID);
                }
                else{
                    printf("No earnings for walletID %s were found in the specified time span.\n\n", Wallet->ownerID);                  
                }
                return 0;
            }

        }
        else if(isDate(buffer[1]) && isDate(buffer[2])){        // if they typed in two dates

            sscanf(buffer[1], "%d-%d-%d", &first.day, &first.month, &first.year);
            sscanf(buffer[2], "%d-%d-%d", &second.day, &second.month, &second.year);

            // only print the transactions made between the two dates
            while(tnode != NULL){
                trans = tnode->currentTransaction;
                if(compareDates(&trans->currentDateAndTime, &first) && compareDates(&second, &trans->currentDateAndTime)){
                    transactionPrint(trans);
                    sum += trans->value;
                    flag = 1;
                }
                tnode = tnode->next;
            }
            if(flag){
                if(type == 'p'){
                    printf("The payments walletID %s has made in the specified time span amount to %d dollars.\n\n", Wallet->ownerID, sum);
                }
                else{
                    printf("The earnings walletID %s has gathered in the specified time span amount to %d dollars.\n\n", Wallet->ownerID, sum);                  
                }
                return 1;  
            }
            else{
                if(type == 'p'){
                    printf("No payments by walletID %s were found in the specified time span.\n\n", Wallet->ownerID);
                }
                else{
                    printf("No earnings for walletID %s were found in the specified time span.\n\n", Wallet->ownerID);                  
                }
                return 0;
            }
        }
        else{
            printf("Error! Incorrect date/time format.\n\n");
            return 0;
            }
    }
    else if(count == 1){

        // user did not type in any times or dates
        // so print all transactions made by or to this wallet
        // since the beginning of the application
        
        while(tnode != NULL){
            trans = tnode->currentTransaction;
            if(trans != NULL){
                transactionPrint(trans);
                sum += trans->value;
                flag = 1;
            }
            tnode = tnode->next;
        }

        if(flag){
            if(type == 'p'){
                printf("The payments walletID %s has made in the specified time span amount to %d dollars.\n\n", Wallet->ownerID, sum);
            }
            else{
                printf("The earnings walletID %s has gathered in the specified time span amount to %d dollars.\n\n", Wallet->ownerID, sum);                  
            }
            return 1;  
        }
        else{
            if(type == 'p'){
                printf("No payments by walletID %s were found in the specified time span.\n\n", Wallet->ownerID);
            }
            else{
                printf("\nNo earnings for walletID %s were found in the specified time span.\n\n", Wallet->ownerID);                  
            }
            return 0;
        }
    }
    else{
        printf("Error! Incorrect date/time format.\n\n");
        return 0;
        }
}
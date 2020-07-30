#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct easyAccess{
    struct walletHashTable* wallets;
    struct bitCoinHashTable* bitCoins;
    struct transactionHashTable* transactions;
    struct srHashTable* senders;
    struct srHashTable* receivers;
}easyAccess;

easyAccess* easyAccessInit();
void getNumbers(FILE*, FILE*, int*, int*, int*);
int requestTransaction(struct transaction*, easyAccess*, char);
int callRequestTransaction(char*, easyAccess*);
int callRequestTransactions(char*, easyAccess*);
void callTraceCoin(char*, easyAccess*);
int callFindPaymentsOrEarnings(char*, easyAccess*, char);
void callWalletStatus(char*, easyAccess*);
void callBitCoinStatus(char*, easyAccess*);

#endif
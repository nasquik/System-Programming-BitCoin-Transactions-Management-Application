#ifndef WALLET_H
#define WALLET_H

typedef struct wallet{
    char ownerID[51];
    struct bclist* bitCoinList;
}wallet;

typedef struct bclist{

    struct bcnode* head;
    struct bcnode* tail;
}bclist;


typedef struct bcnode{
    int bitCoinID;
    int amount;
    struct bcnode* next;
}bcnode;


void bclistInit(bclist*);
int bclistInsert(bclist*, int, int);
int bclistPop(bclist*);
int bclistDeleteAll(bclist*);
bcnode* inbcList(bclist*, int);
void bclistPrint(bclist*);
int bclistGetTotalMoney(bclist*);

wallet* walletInit(char*);
void walletDelete(wallet*);

#endif
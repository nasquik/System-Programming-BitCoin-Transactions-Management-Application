#ifndef WALLETHASHTABLE_H
#define WALLETHASHTABLE_H

typedef struct walletHashTable{
    struct wlist** listArray;
    int numBuckets;
    int count;
}walletHashTable;

typedef struct wlist{

    struct wnode* head;
    struct wnode* tail;
}wlist;

typedef struct wnode{
    struct wallet* Wallet;
    struct wnode* next;
}wnode;

void wlistInit(wlist*);
int wlistInsert(wlist*, struct wallet*);
int wlistDeleteAll(wlist*);
wnode* inwList(wlist*, char*);
void wlistPrint(wlist*);

walletHashTable* walletHashCreate(int);
int walletHashInsert(walletHashTable*, struct wallet*);
void walletHashDelete(walletHashTable*);
struct wallet* walletHashGetEntry(walletHashTable*, char*);
void walletHashPrint(walletHashTable*);

#endif
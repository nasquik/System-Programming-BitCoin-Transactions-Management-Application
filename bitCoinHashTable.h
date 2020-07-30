#ifndef BITCOINHASHTABLE_H
#define BITCOINHASHTABLE_H

typedef struct bitCoinHashTable{
    struct blist** listArray;
    int numBuckets;
    int count;
}bitCoinHashTable;

typedef struct blist{

    struct bnode* head;
    struct bnode* tail;
}blist;

typedef struct bnode{
    struct treeRoot* tree;
    struct bnode* next;
}bnode;

void blistInit(blist*);
bnode* inbList(blist*, int);
int blistInsert(blist*, int, char*, int);
int blistDeleteAll(blist*);
void blistPrint(blist*);

bitCoinHashTable* bitCoinHashCreate(int);
int bitCoinHashInsert(bitCoinHashTable*, int, char*, int);
void bitCoinHashDelete(bitCoinHashTable*);
struct treeRoot* bitCoinHashGetEntry(bitCoinHashTable*, int);
void bitCoinHashPrint(bitCoinHashTable*);

#endif
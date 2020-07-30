#ifndef TRANSACTIONHASHTABLE_H
#define TRANSACTIONHASHTABLE_H

typedef struct transactionHashTable{
    struct tlist** listArray;
    int numBuckets;
    int count;
    int biggestInt;
    struct dateAndTime lastDate;
}transactionHashTable;

typedef struct tlist{

    struct tnode* head;
    struct tnode* tail;
}tlist;

typedef struct tnode{
    char* transactionID;
    struct tnode* next;
}tnode;


void tlistInit(tlist*);
int tlistInsert(tlist*, char*);
int tlistDeleteAll(tlist*);
tnode* intList(tlist*, char*);
void tlistPrint(tlist*);
transactionHashTable* transactionHashCreate(int);
int transactionInHashTable(transactionHashTable*, char*);
int transactionHashInsert(transactionHashTable*, char*);
void transactionHashDelete(transactionHashTable*);

#endif
#ifndef SRHASHTABLE_H
#define SRHASHTABLE_H

typedef struct srHashTable{
    char type;
    struct bucket** buckets;
    int numBuckets;
    int count;
    int maxEntriesPerBucket;
}srHashTable;

typedef struct bucket{
    int currentEntries;
    struct personNode** People;
    struct bucket* next;
}bucket;

typedef struct personNode{
    struct wallet* personalWallet;
    struct transactionList* transactions;
}personNode;

personNode* personNodeCreate(struct wallet*);
void personNodeDelete(personNode*, char);
bucket* bucketCreate(int);
void bucketDelete(bucket*, char);
srHashTable* srHashCreate(char, int, int);
int srHashInsert(srHashTable*, struct wallet*, struct transaction*);
void srHashDelete(srHashTable*);
transactionList* srHashGetTransactionList(srHashTable*, char*);

#endif
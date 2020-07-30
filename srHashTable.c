#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wallet.h"
#include "transactionList.h"
#include "srHashTable.h"
#include "hashFunctions.h"

personNode* personNodeCreate(wallet* Wallet){
    // create personNode and initialize its values

    personNode* person = malloc(sizeof(personNode));
    person->personalWallet = Wallet;
    person->transactions = malloc(sizeof(transactionList));
    transactionListInit(person->transactions);

    return person;
}

void personNodeDelete(personNode* person, char type){
    //delete personNode and all it contains
    
    transactionListDeleteAll(person->transactions, type);
    free(person->transactions);
    free(person);
}

bucket* bucketCreate(int number){
    // create bucket and initialize its values

    bucket* newBucket = malloc(sizeof(bucket));
    newBucket->currentEntries = 0;
    newBucket->next = NULL;
    newBucket->People = malloc(number * sizeof(personNode*));

    for(int i = 0; i < number; i++){
        newBucket->People[i] = NULL;
    }

    return newBucket;
}

void bucketDelete(bucket* Bucket, char type){
    //delete bucket and all it contains

    for(int i = 0; i < Bucket->currentEntries; i++){
        personNodeDelete(Bucket->People[i], type);
    }
    free(Bucket->People);
    free(Bucket);
}

srHashTable* srHashCreate(char type, int numbuckets, int bucketSize){
    //create sender or receiver hash table and initialize values

    srHashTable* HT = malloc(sizeof(srHashTable));
    HT->type = type;
    HT->numBuckets = numbuckets;
    HT->count = 0;
    HT->buckets = malloc(numbuckets * sizeof(bucket*));
    HT->maxEntriesPerBucket = (bucketSize - sizeof(bucket)) / (sizeof(personNode*) + sizeof(personNode));
    
    for(int i = 0; i < numbuckets; i++){
        HT->buckets[i] = bucketCreate(HT->maxEntriesPerBucket);
    }

    return HT;
}

int srHashInsert(srHashTable* HT, wallet* Wallet, transaction* Transaction){
    // insert an entry

    int position;

    if(HT->type == 's')
        position = Hash_Function_str(Transaction->senderID, HT->numBuckets);    // if it's a senderHashTable use senderID for hashing
    else if(HT->type == 'r')
        position = Hash_Function_str(Transaction->receiverID, HT->numBuckets);  // else use receiverID

    
    bucket* auxBucket = HT->buckets[position];

    while(auxBucket->currentEntries == HT->maxEntriesPerBucket){    // while the bucket is full

        for(int i = 0; i < auxBucket->currentEntries; i++){        // check if walletID already in the hashtable and update transactionList
            if(strcmp(auxBucket->People[i]->personalWallet->ownerID, Wallet->ownerID) == 0){
                transactionListInsert(auxBucket->People[i]->transactions, Transaction);
                return 0;
            }
        }

        if(auxBucket->next != NULL){ // if bucket is not the tail of the list
            auxBucket = auxBucket->next;
        }
        else if(auxBucket->currentEntries == HT->maxEntriesPerBucket && auxBucket->next == NULL){   // if bucket is the tail of the list
            bucket* newBucket = bucketCreate(HT->maxEntriesPerBucket);      // create a new bucket and add it to the list
            auxBucket->next = newBucket;
            newBucket->People[0] = personNodeCreate(Wallet);                // first entry in that bucket
            transactionListInsert(newBucket->People[0]->transactions, Transaction);
            newBucket->currentEntries++;

            return 0;
        }
    }

    // if bucket i not full

    int entries = auxBucket->currentEntries;

    
    for(int i = 0; i < entries; i++){       // if walletID is already in the hash table, update transactionList
        if(strcmp(auxBucket->People[i]->personalWallet->ownerID, Wallet->ownerID) == 0){
            transactionListInsert(auxBucket->People[i]->transactions, Transaction);
            return 0;
        }
    }

    // add new entry inside the bucket

    auxBucket->People[entries] = personNodeCreate(Wallet);
    transactionListInsert(auxBucket->People[entries]->transactions, Transaction);
    auxBucket->currentEntries++;

    return 0;
}

transactionList* srHashGetTransactionList(srHashTable* HT, char* ID){
    // return the transactionList of the person with the ID given(used in findPaymentsOrEarnings)

    int position = Hash_Function_str(ID, HT->numBuckets);

    bucket* auxBucket = HT->buckets[position];

    while(auxBucket != NULL){
        for(int i = 0; i < auxBucket->currentEntries; i++){
            if(strcmp(auxBucket->People[i]->personalWallet->ownerID, ID) == 0){
                return(auxBucket->People[i]->transactions);
            }
        }
        auxBucket = auxBucket->next;
    }

    return NULL;
}

void srHashDelete(srHashTable* HT){
    // delete sender/receiver hash table

    bucket* temp;
    bucket* tempNext;

    for(int i = 0; i < HT->numBuckets; i++){
        temp = HT->buckets[i];

        while(temp != NULL){
            tempNext = temp->next;
            bucketDelete(temp, HT->type);
            temp = tempNext;
        }
    }
    free(HT->buckets);

    free(HT);
}
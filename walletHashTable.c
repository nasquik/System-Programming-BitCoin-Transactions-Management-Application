#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wallet.h"
#include "walletHashTable.h"
#include "hashFunctions.h"

// similar functions to those in bitCoinHashTable.c

    // LIST FUNCTIONS //

void wlistInit(wlist* List){

    List->head = NULL;
    List->tail = NULL;
}

wnode* inwList(wlist* List, char* ID){

    wnode* temp = List->head;

    while(temp != NULL){
        if(strcmp(temp->Wallet->ownerID, ID) == 0)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

int wlistInsert(wlist* List, wallet* Wal){

    wnode* temp = inwList(List, Wal->ownerID);

    if(temp == NULL){
        temp = malloc(sizeof(wnode));
        temp->Wallet = Wal;
        temp->next = NULL;

        if(List->head == NULL && List->tail == NULL){
            List->head = temp;
            List->tail = temp;
            return 1;
        }
        else{
            List->tail->next = temp;
            List->tail = temp;
            return 1;
        }
    }
    else{
        return 0;
    }
}

int wlistDeleteAll(wlist* List){

    wnode* delNode = List->head;
    wnode* temp;
    int count = 0;

    while(delNode != NULL){
        temp = delNode->next;
        bclistDeleteAll(delNode->Wallet->bitCoinList);
        free(delNode->Wallet->bitCoinList);
        free(delNode->Wallet);
        free(delNode);
        count++;
        delNode = temp;
    }

    List->head = NULL;
    List->tail = NULL;

    return count;
}

void wlistPrint(wlist* List){

    wnode* curr = List->head;

    if(curr == NULL){
        printf("No Wallets!\n");
        return;
    }

    while(curr != NULL){
        printf("%s\n", curr->Wallet->ownerID);
        curr = curr->next;
    }
    printf("\n");
}

    // HASHTABLE FUNCTIONS //

walletHashTable* walletHashCreate(int numbuckets){

    walletHashTable* HT = malloc(sizeof(walletHashTable));

    HT->numBuckets = numbuckets;
    HT->count = 0;
    HT->listArray = malloc(numbuckets * sizeof(wlist*));
    for(int i = 0; i < numbuckets; i++){
        HT->listArray[i] = malloc(sizeof(wlist));
        wlistInit(HT->listArray[i]);
    }

    return HT;
}

int walletHashInsert(walletHashTable* HT, wallet* Entry){

    int position = Hash_Function_str(Entry->ownerID, HT->numBuckets);

    if(wlistInsert(HT->listArray[position], Entry))
        HT->count++;
}

wallet* walletHashGetEntry(walletHashTable* HT, char* ID){

    int position = Hash_Function_str(ID, HT->numBuckets);

    wnode* returnEntry = inwList(HT->listArray[position], ID);

    if(returnEntry != NULL)
        return returnEntry->Wallet;
    else{
        return NULL;
    }
}

void walletHashDelete(walletHashTable* HT){

    int count = 0;

    for(int i = 0; i < HT->numBuckets; i++){
        count += wlistDeleteAll(HT->listArray[i]);
        free(HT->listArray[i]);
    }
    free(HT->listArray);
    free(HT);
}

void walletHashPrint(walletHashTable* HT){

    printf("Wallets HashTable:\n");

    wnode* temp;
    
        for(int i = 0; i < HT->numBuckets; i++){

        temp = HT->listArray[i]->head;

        while(temp != NULL){
            printf("%s\n", temp->Wallet->ownerID);
            bclistPrint(temp->Wallet->bitCoinList);
            printf("\n");
            temp = temp->next;
        }
    }

    printf("\n");
}
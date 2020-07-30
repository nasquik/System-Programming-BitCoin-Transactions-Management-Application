#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transactionList.h"
#include "bitCoinTree.h"
#include "bitCoinHashTable.h"
#include "hashFunctions.h"

    // LIST FUNCTIONS //

void blistInit(blist* List){
    //initialize blist

    List->head = NULL;
    List->tail = NULL;
}

bnode* inbList(blist* List, int ID){
    // determine if a bitCoin with the given ID is in blist. If so, return a pointer to the bnode that contains it

    bnode* temp = List->head;

    while(temp != NULL){
        if(temp->tree->bitCoinID == ID)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

int blistInsert(blist* List, int ID, char* owner, int value){
    // insert a node at the end of the blist if it is not already in the list

    bnode* temp = inbList(List, ID);

    if(temp == NULL){
        temp = malloc(sizeof(bnode));
        temp->tree = bitCoinTreeCreate(ID, owner, value);
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

int blistDeleteAll(blist* List){
    // delete all the nodes in blist and the trees they contain

    bnode* delNode = List->head;
    bnode* temp;
    int count = 0;

    while(delNode != NULL){
        temp = delNode->next;
        deleteTree(delNode->tree);
        free(delNode);
        count++;
        delNode = temp;
    }

    List->head = NULL;
    List->tail = NULL;

    return count;
}

void blistPrint(blist* List){
    // print all nodes in the list

    bnode* curr = List->head;

    if(curr == NULL){
        printf("No BitCoins!\n");
        return;
    }

    while(curr != NULL){
        printf("%d %s\n", curr->tree->bitCoinID, curr->tree->walletID);
        curr = curr->next;
    }
    printf("\n");
}

    // HASHTABLE FUNCTIONS //

bitCoinHashTable* bitCoinHashCreate(int numbuckets){
    // create bitCoinHashTable with given number of buckets and initialize the structures it contains

    bitCoinHashTable* HT = malloc(sizeof(bitCoinHashTable));

    HT->numBuckets = numbuckets;
    HT->count = 0;
    HT->listArray = malloc(numbuckets * sizeof(blist*));
    for(int i = 0; i < numbuckets; i++){
        HT->listArray[i] = malloc(sizeof(blist));
        blistInit(HT->listArray[i]);
    }

    return HT;
}

int bitCoinHashInsert(bitCoinHashTable* HT, int ID, char* owner, int value){
    // insert entry(if it doesn't already exist in bitCoinHashTable)

    int position = Hash_Function_int(ID, HT->numBuckets);

    if(blistInsert(HT->listArray[position], ID, owner, value)){
        HT->count++;
        return 1;
    }
    return 0;
}

treeRoot* bitCoinHashGetEntry(bitCoinHashTable* HT, int ID){
    // return a pointer to the bitCoinTree of the bitCoin with the given ID

    int position = Hash_Function_int(ID, HT->numBuckets);

    bnode* returnEntry = inbList(HT->listArray[position], ID);

    if(returnEntry != NULL)
        return returnEntry->tree;
    else{
        return NULL;
    }
}

void bitCoinHashDelete(bitCoinHashTable* HT){
    // delete bitCoinHashTable

    int count = 0;

    for(int i = 0; i < HT->numBuckets; i++){
        count += blistDeleteAll(HT->listArray[i]);
        free(HT->listArray[i]);
    }
    free(HT->listArray);
    free(HT);
}

void bitCoinHashPrint(bitCoinHashTable* BHT){
    // print BitCoinHashTable (used for debugging)

    printf("BitCoins HashTable:\n");

    bnode* temp;
    
    for(int i = 0; i < BHT->numBuckets; i++){

        temp = BHT->listArray[i]->head;

        while(temp != NULL){
            printf("%d %s\n", temp->tree->bitCoinID, temp->tree->walletID);
            printf("\n");
            temp = temp->next;
        }
    }
    printf("\n");
}
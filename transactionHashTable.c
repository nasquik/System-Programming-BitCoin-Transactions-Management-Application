#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transactionList.h"
#include "transactionHashTable.h"
#include "hashFunctions.h"

// similar functions to those in bitCoinHashTable.c

void tlistInit(tlist* List){

    List->head = NULL;
    List->tail = NULL;
}

tnode* intList(tlist* List, char* ID){

    tnode* temp = List->head;

    while(temp != NULL){
        if(strcmp(temp->transactionID, ID) == 0)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

int tlistInsert(tlist* List, char* transID){

    tnode* temp = intList(List, transID);

    if(temp == NULL){
        temp = malloc(sizeof(tnode));
        temp->transactionID = transID;
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

int tlistDeleteAll(tlist* List){

    tnode* delNode = List->head;
    tnode* temp;
    int count = 0;

    while(delNode != NULL){
        temp = delNode->next;
        free(delNode);
        count++;
        delNode = temp;
    }

    List->head = NULL;
    List->tail = NULL;

    return count;
}

transactionHashTable* transactionHashCreate(int numbuckets){

    transactionHashTable* HT = malloc(sizeof(transactionHashTable));

    HT->numBuckets = numbuckets;
    HT->count = 0;
    HT->biggestInt = 0;
    createDateAndTime(&HT->lastDate, 0, 0, 0, 0, 0);
    HT->listArray = malloc(numbuckets * sizeof(tlist*));
    for(int i = 0; i < numbuckets; i++){
        HT->listArray[i] = malloc(sizeof(tlist));
        tlistInit(HT->listArray[i]);
    }

    return HT;
}

int transactionInHashTable(transactionHashTable* HT, char* ID){

    int position = Hash_Function_str(ID, HT->numBuckets);

    if(intList(HT->listArray[position], ID) != NULL)
        return 1;
    else
        return 0;
}

int transactionHashInsert(transactionHashTable* HT, char* Entry){

    int position = Hash_Function_str(Entry, HT->numBuckets);

    if(tlistInsert(HT->listArray[position], Entry)){
        HT->count++;
        return 1;
    }
}

void transactionHashDelete(transactionHashTable* HT){

    int count = 0;

    for(int i = 0; i < HT->numBuckets; i++){
        count += tlistDeleteAll(HT->listArray[i]);
        free(HT->listArray[i]);
    }
    free(HT->listArray);
    free(HT);
}
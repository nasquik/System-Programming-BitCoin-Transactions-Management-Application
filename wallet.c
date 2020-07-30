#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wallet.h"

    // LIST FUNCTIONS //

void bclistInit(bclist* List){

    List->head = NULL;
    List->tail = NULL;
}

bcnode* inbcList(bclist* List, int bitCoin){

    bcnode* temp = List->head;

    while(temp != NULL){
        if(temp->bitCoinID == bitCoin)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

int bclistInsert(bclist* List, int bitCoin, int value){

    bcnode* temp = inbcList(List, bitCoin);

    if(temp == NULL){
        temp = malloc(sizeof(bcnode));
        temp->bitCoinID = bitCoin;
        temp->amount = value;
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
        temp->amount += value;
        return 0;
    }
}

int bclistDeleteAll(bclist* List){

    bcnode* delNode = List->head;
    bcnode* temp;
    int count;

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

int bclistPop(bclist* List){
    //pop the first item in the list

    bcnode* temp = List->head;

    if(temp != NULL){
        if(temp->next != NULL){
            List->head = List->head->next;
        }
        else{
            List->head = List->tail = NULL;
        }

        free(temp);
        return 1;
    }
    else{
        printf("This user doesn't have any bitCoins!\n");
        return -1;
    }
}

void bclistPrint(bclist* List){

    bcnode* curr = List->head;

    if(curr == NULL){
        printf("This user doesn't have any bitCoins!\n");
        return;
    }

    while(curr != NULL){
        printf("%d %d\n", curr->bitCoinID, curr->amount);
        curr = curr->next;
    }
    printf("\n");
}

int bclistGetTotalMoney(bclist* List){
    // add the value of every bitCoin and return the sum

    int totalMoney = 0;

    bcnode* curr = List->head;

    if(curr == NULL){
        return -1;
    }

    while(curr != NULL){
        totalMoney += curr->amount;
        curr = curr->next;
    }

    return totalMoney;
}

    // WALLET FUNCTIONS //

wallet* walletInit(char* name){

    wallet* Wallet = malloc(sizeof(wallet));
    strcpy(Wallet->ownerID, name);
    Wallet->bitCoinList = malloc(sizeof(bclist));
    bclistInit(Wallet->bitCoinList);

    return Wallet;
};

void walletDelete(wallet* Wallet){

    bclistDeleteAll(Wallet->bitCoinList);
    free(Wallet->bitCoinList);
    free(Wallet);
};
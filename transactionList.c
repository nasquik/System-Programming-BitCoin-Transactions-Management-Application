#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wallet.h"
#include "transactionList.h"

void createDateAndTime(dateAndTime* current, int day, int month, int year, int hours, int minutes){
    
    current->day = day;
    current->month = month;
    current->year = year;
    current->hours = hours;
    current->minutes = minutes;
}

void printDateAndTime(dateAndTime* current){
    printf("%02d-%02d-%d %02d:%02d\n",  current->day, current->month, current->year, current->hours, current->minutes);
};

transaction* transactionCreate(char* transID, char* sendID, char* receiveID, int val, struct dateAndTime curr){
    
    transaction* Transaction = malloc(sizeof(transaction));

    strcpy(Transaction->transactionID, transID);
    Transaction->senderID = sendID;
    Transaction->receiverID = receiveID;
    Transaction->value = val;
    Transaction->currentDateAndTime = curr;

    return Transaction;
}

void transactionPrint(transaction* trans){
    printf("%s %s %s %d ", trans->transactionID, trans->senderID, trans->receiverID, trans->value);
    printDateAndTime(&trans->currentDateAndTime);
}

void transactionListInit(transactionList* List){

    List->head = NULL;
    List->tail = NULL;
}

transactionNode* inTransactionList(transactionList* List, char* ID){

    transactionNode* temp = List->head;

    while(temp != NULL){
        if(strcmp(temp->currentTransaction->transactionID, ID) == 0)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

int transactionListInsert(transactionList* List, transaction* Transaction){

    transactionNode* temp = inTransactionList(List, Transaction->transactionID);

    if(temp == NULL){
        temp = malloc(sizeof(transactionNode));
        temp->currentTransaction = Transaction;
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

int transactionListDeleteAll(transactionList* List, char type){

    transactionNode* delNode = List->head;
    transactionNode* temp;
    int count = 0;

    while(delNode != NULL){
        temp = delNode->next;
        if(type == 's')
            free(delNode->currentTransaction);
        free(delNode);
        count++;
        delNode = temp;
    }

    List->head = NULL;
    List->tail = NULL;

    return count;
}

void transactionListPrint(transactionList* List){

    transactionNode* curr = List->head;

    if(curr == NULL){
        printf("No Transactions!\n");
        return;
    }

    transaction* trans;

    while(curr != NULL){
        trans = curr->currentTransaction;
        transactionPrint(trans);
        curr = curr->next;
    }
    printf("\n");
}
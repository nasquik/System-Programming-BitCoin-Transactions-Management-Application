#ifndef TRANSACTIONLIST_H
#define TRANSACTIONLIST_H

typedef struct dateAndTime{
    int day;
    int month;
    int year;
    int hours;
    int minutes;
}dateAndTime;

typedef struct transactionList{

    struct transactionNode* head;
    struct transactionNode* tail;
}transactionList;

typedef struct transactionNode{
    struct transaction* currentTransaction;
    struct transactionNode* next;
}transactionNode;

typedef struct transaction{
    char transactionID[16];
    char* senderID;
    char* receiverID;
    int value;
    struct dateAndTime currentDateAndTime;
}transaction;

void createDateAndTime(dateAndTime*, int, int, int, int, int);
void printDateAndTime(dateAndTime*);
transaction* transactionCreate(char*, char*, char*, int, struct dateAndTime);
void transactionPrint(transaction*);
void transactionListInit(transactionList*);
transactionNode* inTransactionList(transactionList*, char*);
int transactionListInsert(transactionList*, struct transaction*);
int transactionListDeleteAll(transactionList*, char);
void transactionListPrint(transactionList*);

#endif
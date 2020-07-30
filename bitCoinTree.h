#ifndef BITCOINTREE_H
#define BITCOINTREE_H

typedef struct treeRoot{
    int bitCoinID;
    int numTransactions;
    int value;
    char* walletID;
    struct treeNode* leftChild;
    struct treeNode* rightChild;
}treeRoot;

typedef struct treeNode{
    int amount;
    char type;
    struct transaction* transaction;
    struct treeNode* leftChild;
    struct treeNode* rightChild;
}treeNode;

treeRoot* bitCoinTreeCreate(int, char*, int);
void printBitCoinHistory(treeRoot*);
void getTransactions(treeNode*, struct transactionList**);
void getBitCoinStatus(treeRoot*);
int recordTransaction(treeRoot*, struct transaction*, int*);
int recTransaction(treeRoot*, treeNode*, struct transaction*, int*);
void deleteTree(treeRoot*);
void deleteNode(treeNode*);

#endif
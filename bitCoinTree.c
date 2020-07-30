#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wallet.h"
#include "transactionList.h"
#include "bitCoinTree.h"

treeRoot* bitCoinTreeCreate(int ID, char* owner, int value){
    // create bitCoinTree and initialize the values it contains

    treeRoot* root = malloc(sizeof(treeRoot));
    root->bitCoinID = ID;
    root->value = value;
    root->walletID = owner;
    root->numTransactions = 0;
    root->leftChild = NULL;
    root->rightChild = NULL;

    return root;
}

treeNode* treeNodeCreate(char type){
    // create a new node and initialize it depending on the type given
 
    treeNode* temp = malloc(sizeof(treeNode));

    temp->type = type;
    temp->amount = 0;
    temp->transaction = NULL;
    temp->leftChild = NULL;
    temp->rightChild = NULL; 
    
    return temp;
}

int recordTransaction(treeRoot* root, transaction* trans, int* value){
    // record a transaction on the bitCoinTree

    if (root == NULL)
        return -1;

    if(*value == 0)
        return 1;

    root->numTransactions++;

    if (root->leftChild == NULL && root->rightChild == NULL){       // if the bitCoin has not been used in a transaction before
        if(strcmp(trans->senderID, root->walletID) == 0){           // make sure the first sender is its original owner
            root->leftChild = treeNodeCreate('l');                  // create a 'left' tree node(represents the receiver of the transaction)
            root->leftChild->transaction = trans;
            root->rightChild = treeNodeCreate('r');                 //create a 'right' tree node(represents the sender of the transaction)
            root->rightChild->transaction = trans;

            if(*value >= root->value){                              //update value inside root node
                root->leftChild->amount = root->value;
                root->rightChild->amount = 0;
                *value -= root->value;
            }
            else{
                root->leftChild->amount = *value;
                root->rightChild->amount = root->value - (*value); 
                *value = 0;
            }

            return 1;
        }
        else{
            printf("Cannot perform transaction! Incorrect tree.\n");
            return -1;
        }
    }
    else{
        recTransaction(root, root->leftChild, trans, value);
        if(*value == 0)
            return 1;

        recTransaction(root, root->rightChild, trans, value);
        return 1;
        }
}

int recTransaction(treeRoot* root, treeNode* node, transaction* trans, int* value){
    // recursive function similar to recordTransaction used to save a transaction on the bitCoinTree

    if (node->leftChild == NULL && node->rightChild == NULL){
        if(node->type == 'l'){                                                      // if the node is a left child(receiver of previous transaction)
            if(strcmp(trans->senderID, node->transaction->receiverID) == 0){        // compare the given transaction's sender ID with the receiver ID of the transaction of which the node is the receiver
                node->leftChild = treeNodeCreate('l');
                node->leftChild->transaction = trans;
                node->rightChild = treeNodeCreate('r');
                node->rightChild->transaction = trans;

                if(*value >= node->amount){
                    node->leftChild->amount = node->amount;
                    node->rightChild->amount = 0;
                    *value -= node->amount;

                }
                else{
                    node->leftChild->amount = *value;
                    node->rightChild->amount = node->amount - (*value); 
                    *value = 0;
                }

                return 1;
            }
        }
        else if(node->type == 'r'){                                                // if the node is a right child(sender of previous transaction)
            if(strcmp(trans->senderID, node->transaction->senderID) == 0){        // compare the given transaction's sender ID with the sender ID of the transaction of which the node is the receiver
                node->leftChild = treeNodeCreate('l');
                node->leftChild->transaction = trans;
                node->rightChild = treeNodeCreate('r');
                node->rightChild->transaction = trans;

                if(*value >= node->amount){
                    node->leftChild->amount = node->amount;
                    node->rightChild->amount = 0;
                    *value -= node->amount;
                }
                else{
                    node->leftChild->amount = *value;
                    node->rightChild->amount = node->amount - (*value);
                    *value = 0; 
                }
                return 1;
            }
        }
        else{
            printf("Cannot perform transaction! Incorrect tree.\n");
            return -1;
        }
    }
    else{                                                                      //recursively call function until a leaf node is reached
        recTransaction(root, node->leftChild, trans, value);
        if(*value == 0)
            return 1;

        recTransaction(root, node->rightChild, trans, value);
        }

    return 1;
}

void getBitCoinStatus(treeRoot* root){
    // print bitCoinID, initial value, the number of transactions it has participated in and its remaining unspent value for the bitCoin given

    if(root == NULL){
        printf("ERROR! BitCoin does not exist.\n");
        return;
    }

    treeNode* node = root->rightChild;
    
    if(node == NULL){
        printf("BitCoinID: %d\nInitial Value: %d\nNo of Transactions: %d\nUnspent: %d\n", root->bitCoinID,  root->value, root->numTransactions, root->value);
        return;
    }

    while(node->rightChild != NULL){
        node = node->rightChild;
    }
    
    printf("BitCoinID: %d\nInitial Value: %d\nNo of Transactions: %d\nUnspent: %d\n", root->bitCoinID,  root->value, root->numTransactions, node->amount);
}

void deleteTree(treeRoot* root){
    // delete bitCoinTree, recursively delete all nodes

    if (root == NULL)
        return;

    if(root->leftChild != NULL)
        deleteNode(root->leftChild);

    if(root->rightChild != NULL)
        deleteNode(root->rightChild);

    free(root);
}

void deleteNode(treeNode* node){
    //delete a node's children and then delete the node

    if (node == NULL)
        return;

    if(node->leftChild != NULL)
        deleteNode(node->leftChild);

    if(node->rightChild != NULL)
        deleteNode(node->rightChild);

    free(node);
}

void printBitCoinHistory(treeRoot* root){
    // gather all transactions a bitCoin has participated in in a transactionList and print the list

    transactionList* temp = malloc(sizeof(transactionList));
    transactionListInit(temp);

    if(root == NULL)
        return;

    if(root->leftChild == NULL && root->rightChild == NULL)
        return;

    if(root->leftChild != NULL)
        getTransactions(root->leftChild, &temp);
    
    if(root->rightChild != NULL)
        getTransactions(root->rightChild, &temp);

    if(temp->head != NULL)
        transactionListPrint(temp);
    else
        printf("This bitCoin does not participate in any transactions!\n");
    
    transactionListDeleteAll(temp, 'r');
    free(temp);
}

void getTransactions(treeNode* node, transactionList** temp){
    // recursively add to the list created in printTransactionHistory

    if(node == NULL)
        return;

    if(node->leftChild != NULL){
        getTransactions(node->leftChild, temp);
    }

    if(node->rightChild != NULL){
        getTransactions(node->rightChild, temp);
    }

    if(node->type == 'l')                                       // since left and right children of a node are created by the same 
        transactionListInsert(*temp, node->transaction);        // transaction, only check the left child and then insert the transaction
                                                                // in the transactionList
    return;
}

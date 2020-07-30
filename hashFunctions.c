#include <stdio.h>
#include <string.h>
#include "hashFunctions.h"

// universal hash functions

int Hash_Function_str(char* ID, int numBuckets){
	// works for strings

	int h = 0;
	long a = 77;
	long p = 1302397;

	int length = strlen(ID);

	for(int i = 0; i < length; i++){
		h = ((h * a) + ID[i]) % p;
	}

	return (h % numBuckets);
}

int Hash_Function_int(int ID,int numBuckets){
	// works for integers

	long p = 1302397;
	long a = 35759;
	long b = 128932;

	return ((a* ID + b) % p) % numBuckets;
}

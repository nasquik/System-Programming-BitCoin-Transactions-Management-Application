#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "transactionList.h"
#include "auxFunctions.h"

int isDate(char* word){
    // determine if the given string is a date

    if(strlen(word) == 10 && isdigit(word[0]) && isdigit(word[1]) && (word[2] == '-') && isdigit(word[3]) && isdigit(word[4]) && (word[5] == '-') && isdigit(word[6]) && isdigit(word[7]) && isdigit(word[8]) && isdigit(word[9])){
        return 1;
    }
    else
        return 0;
}

int isTime(char* word){
    // determine if the given string is a time

    if(strlen(word) == 5 && isdigit(word[0]) && isdigit(word[1]) && (word[2] == ':') && isdigit(word[3]) && isdigit(word[4])){
        return 1;
    }
    else
        return 0;
}

int compareDatesAndTimes(struct dateAndTime* newDate, struct dateAndTime* oldDate){
    // compare two dateAndTime stucts, if newDate corresponds to a later date than oldDate return 1

    if(newDate->year > oldDate->year)
        return 1;
    else if(newDate->year < oldDate->year)
        return 0;
    else{
        if(newDate->month > oldDate->month)
            return 1;
        else if(newDate->month < oldDate->month)
            return 0;
        else{
            if(newDate->day > oldDate->day)
                return 1;
            else if(newDate->day < oldDate->day)
                return 0;
            else{
                if(newDate->hours > oldDate->hours)
                    return 1;
                else if(newDate->hours < oldDate->hours)
                    return 0;
                else{
                    if(newDate->minutes >= oldDate->minutes)
                        return 1;
                    else
                        return 0;
                }
            }
        }
    }
}

int compareDates(struct dateAndTime* date1, struct dateAndTime* date2){
    // compare the dates of two dateAndTime stucts, if date1 corresponds to a later date than date2 return 1

    if(date1->year > date2->year)
        return 1;
    else if(date1->year < date2->year)
        return 0;
    else{
        if(date1->month > date2->month)
            return 1;
        else if(date1->month < date2->month)
            return 0;
        else{
            if(date1->day >= date2->day)
                return 1;
            else
                return 0;
        }
    }
}

int compareTimes(struct dateAndTime* time1, struct dateAndTime* time2){
    // compare the times of two dateAndTime stucts, if time1 corresponds to a later time than time2 return 1

    if(time1->hours > time2->hours)
        return 1;
    else if(time1->hours < time2->hours)
        return 0;
    else{
        if(time1->minutes >= time2->minutes)
            return 1;
        else
            return 0;
    }
}

int assertIntness(char* string){
    // determine if the given string is an int

    for(int i = 0; i < strlen(string); i++){
        if(!isdigit(string[i])){
            return 0;
        }
    }
    
    return 1;
}
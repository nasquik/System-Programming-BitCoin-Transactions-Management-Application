#ifndef AUXFUNCTIONS_H
#define AUXFUNCTIONS_H

int isDate(char*);
int isTime(char*);
int compareDatesAndTimes(struct dateAndTime*, struct dateAndTime*);
int compareDates(struct dateAndTime*, struct dateAndTime*);
int compareTimes(struct dateAndTime*, struct dateAndTime*);
int assertIntness(char*);

#endif
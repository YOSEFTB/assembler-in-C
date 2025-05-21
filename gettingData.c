#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int getData(char *dataLine, int *dataArray,int counter){
	int sign;
	char *numToken=malloc(20*sizeof(char));
	for(;!isdigit(*dataLine);dataLine++)
		;
	numToken=strtok(dataLine,",");
	while(numToken!=NULL){
		sign=1;
		if((*numToken)=='-'){
			numToken++;
			sign*=-1;
		}
		if((*numToken)=='+')
			numToken++;
			
		*(dataArray+counter)=(sign* atoi(numToken));
		counter++;
		numToken=strtok(NULL,",");
	}
	printf("%d\n",counter);
	return counter;

}

int getString(char *stringLine, int *dataArray,int counter){
	char *start;
	int i=0;
	/*for(;isspace(*stringLine);stringLine++)*/
	for(;(*stringLine)!='\"';stringLine++)
		;
		stringLine++;
		start=stringLine;
	while((*stringLine)!='\"'){
		*(dataArray+counter)=*stringLine;
		counter++;
		stringLine++;
		i++;
	}
	*(stringLine)='\0';
	stringLine++;
	printf("%d",stringLine-start);
	return (stringLine-start);
}

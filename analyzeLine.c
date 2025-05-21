#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int isImmediate(char *x);
int isIndex(char *x);
int isRegister(char *x);


int isEmpty(char *a){
	int i;
	for(i=0;(*(a+i))!= '\n' && (*(a+i))!= '\0';i++){
		if(!isspace(*(a+i)))
			return 0;
	}
	return 1;
}

int isComment(char *a){
	if((*a)==';'){
		return 1;
	}
	return 0;
}

int isCommand(char *x){
	int i;
	char * commandList[16]={"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","jsr","red","prn","rts","stop"};
	for(i=0;i<16;i++){
		if(strcmp(x,commandList[i])==0)
			return 1;
	}
	return 0;
}

int isInstruction(char *x){
	if(strcmp(x,".data")==0 || strcmp(x,".string")==0)
		return 1;
	return 0;
}

char * trimBeg(char *x){
	for(;isspace(*(x));x++)
	;
	return x;
}

char * trimEnd(char *x){
	char *c;
	for(c=x+(strlen(x)-1);isspace(*c);c--)
		;
	*(c+1)='\0';
	return x;
}

int wordsForCommand(char *x){
	if(isImmediate(x))
		return 1;
	if(isIndex(x))
		return 2;
	if(isRegister(x))
		return 0;
	else	/* Is label */
		return 2;
	
}

int wordsForCommand2(char *x,char *y){
	int l,m;
	l=wordsForCommand(x);
	m=wordsForCommand(y);
	return (l+m);
}

int isImmediate(char *x){
	if((*x)=='#')
		return 1;
	return 0;
}

int isIndex(char *x){
	char c[35];
	memset(c,'\0',35);
	strcpy(c,x);
	if(strstr(c,"["))
		return 1;
	return 0;		
}

int isRegister(char *x){
	if(strcmp(x,"r0")==0 || strcmp(x,"r1")==0 || strcmp(x,"r2")==0 || strcmp(x,"r3")==0 || strcmp(x,"r4")==0 || strcmp(x,"r5")==0 || strcmp(x,"r6")==0 || strcmp(x,"r7")==0 || 	strcmp(x,"r8")==0 || strcmp(x,"r9")==0 || strcmp(x,"r10")==0 || strcmp(x,"r11")==0 || strcmp(x,"r12")==0 || strcmp(x,"r13")==0 || strcmp(x,"r14")==0 || strcmp(x,"r15")==0){
		return 1;
	}
	return 0;
}

int hasTwoOperands(char *x){
	if(strcmp(x,"mov")==0 || strcmp(x,"cmp")==0 || strcmp(x,"add")==0 || strcmp(x,"sub")==0 || strcmp(x,"lea")==0)
		return 1;
	return 0;
}

int hasOneOperand(char *x){
	if(strcmp(x,"clr")==0 ||strcmp(x,"not")==0 ||strcmp(x,"inc")==0 ||strcmp(x,"dec")==0 ||strcmp(x,"jmp")==0 ||strcmp(x,"bne")==0 ||strcmp(x,"jsr")==0 || 	    strcmp(x,"red")==0 ||strcmp(x,"prn")==0)
		return 1;
	return 0;
}

int hasNoOperands(char *x){
	if(strcmp(x,"rts")==0 || strcmp(x,"stop")==0)
		return 1;
	return 0;
}

int getData(char *dataLine, int *dataArray,int pointInArray, int variable){
	int sign,counter=0;
	char *numToken=malloc(20*sizeof(char));
	if(variable){
		for(;(*dataLine)!=':';dataLine++)
			;
	}
	for(;!isdigit(*dataLine) && (*(dataLine))!='-' && (*(dataLine))!='+' ;dataLine++)
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
			
		*(dataArray+pointInArray)=(sign* atoi(numToken));
		pointInArray++;
		counter++;
		numToken=strtok(NULL,",");
	}
	return counter;
}

int getString(char *stringLine, int *dataArray,int counter){
	char *start;
	for(;(*stringLine)!='\"';stringLine++)
		;
		stringLine++;
		start=stringLine;
	while((*stringLine)!='\"'){
		*(dataArray+counter)=*stringLine;
		counter++;
		stringLine++;
	}
	*(stringLine)='\0';
	stringLine++;
	return (stringLine-start);

}

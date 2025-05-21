#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "temporaryStructHeader.h"

int isEmpty(char *);
int isComment(char *);
char * trimBeg(char *x);
char * trimEnd(char *x);
int isInExtern(char *, struct external *);
int inLabels(char *, struct label *);
int isImmediate(char *);
int isIndex(char *);
int isLabel(char *);
int isCommand(char *);
int isRegister(char *);
int hasTwoOperands(char *);
int hasOneOperand(char *);
int hasNoOperands(char *);
void newEntry(char *, struct entry **);
int alreadyIsEntry(char *, struct entry *);
int getValue(char *, struct label *);
int calculateBase(int);
int calculateOffset(int);
void printLabels(struct label *);
void printInHexa(struct word,int, FILE *p);
int getMachineCode1(char *a,char *b, int, struct label *, struct external *, FILE *p);
int getMachineCode2(char *a,char *b,char *c, int, struct label *, struct external *, FILE *p);
int getMachineCode0(char *a, int, FILE *p);
void getDataArrayCode(FILE *p, int [],int , int  );
void createEntryFolder(char *,struct entry *,struct label *);
void createExternalFolder(char *, struct external *, struct labelOperand *);
void newLabelOperand(char *, int , int , struct labelOperand **);

/*the second passings purpose is to send the operands details to the printing function and the function will print the binary code (based on addresses, operands, and addresses) to a file ".ob" (in hexa form). this funtion will also create the entry file, and the extern file. Also, now that we have a full label list, we can check the validity of every label operand (and make sure its really a label), and check that all entry declaration are also labels*/

int secondPassing(FILE *fptr, char *fileName, struct label *head, struct external *externsHead, int dataArray[], int ICF, int DCF, int flag){
	struct entry *entriesHead;
	struct labelOperand *operands;
	FILE *fptr2;
	int fileLine,instructionLine,validFlag,n;
	char *token1,*token2,*token3,*token4,*tokenIndex;
	char *codeLine,*codeCopy,*obFileName;
	fileLine=0,instructionLine=100,validFlag=1;
	n=0;
	entriesHead=NULL;
	operands=NULL;
	if(flag==0)
		return 0;
	token1= malloc(33*sizeof(char));
	token2= malloc(33*sizeof(char));
	token3= malloc(33*sizeof(char));
	token4= malloc(33*sizeof(char));
	tokenIndex=malloc(33*sizeof(char));
	codeLine= malloc(80*sizeof(char));
	codeCopy= malloc(80*sizeof(char));
	obFileName=malloc( (strlen(fileName)+4)*sizeof(char));
	strcpy(obFileName,fileName);
	strcat(obFileName,".ob");
	*(obFileName+strlen(fileName)+4)='\0';
	fptr2= fopen(obFileName,"w");
	fprintf(fptr2,"\t%d %d\n",ICF,DCF);   /*prints a header to the ".ob" file, with the amount of instructions and the amount of data */
	memset(codeLine,'\0',80);
	memset(codeCopy,'\0',80);
	memset(token1,'\0',33);
	while (fgets(codeLine,81,fptr)!=NULL){
		strcpy(codeCopy,codeLine);
		fileLine++;	/*to print on errors*/
		if(isEmpty(codeLine) || isComment(codeLine))
			continue;
		token1=strtok(codeLine," \f\n\r\t\v");
		if(strcmp(token1+strlen(token1)-1,":")==0){
			token2=strtok(NULL," \f\n\r\t\v");
			if(strcmp(token2,".data")==0)		/*data, string and extern do not need to be checked at all*/
				continue;			/*we already have a complete data array, ready to be accessed. and a complete externs*/
			if(strcmp(token2,".string")==0)
				continue;
			if(strcmp(token2,".extern")==0)
				continue;
			if(strcmp(token2,".entry")==0){	
				token3=strtok(NULL," \f\n\r\t\v");  /*get the entry name, and ensure- 1)its declared as a label.2)its not also an extern*/
				if(!inLabels(token3,head)){
					printf("Error.You have declared an entry label without actually declaring any such label in the code \nIn line %d\n",fileLine);
						validFlag=0;
				}
				if(isInExtern(token3,externsHead)){
					printf("Error. You cannot declare the same label as both entry and extern \nIn line %d\n",fileLine);
					validFlag=0;
				}
				if(alreadyIsEntry(token3,entriesHead))  /*if it was already declared, then we ignore this declaration*/
					continue;
				newEntry(token3,&entriesHead);  /*if its a valid entry, create an entry and add it to the list*/
				continue;
			}
			if(isCommand(token2)){
				if(hasOneOperand(token2)){
					token3=strtok(NULL," \f\n\r\t\v");
					if(isLabel(token3)){	/*get the label name, and ensure its either declared a label or an extern */
						if(!inLabels(token3,head) && !isInExtern(token3,externsHead)){
							printf("Error.You entered a label (as an operand) which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(token2,externsHead))  /*list of operands that used extern, needed for the ."ext" file*/
							newLabelOperand(token2, instructionLine+2, instructionLine+3, &operands);
					}
					
					if(isIndex(token3)){	/*get the label name, and ensure its either declared a label or an extern*/ 
						strcpy(tokenIndex,token3);
						trimBeg(tokenIndex);
						trimEnd(tokenIndex);
						(*(tokenIndex+strlen(tokenIndex)-5))='\0'; /*obtain the label that comes right before the index, to check it*/
						if(!inLabels(tokenIndex,head) && !isInExtern(tokenIndex,externsHead)){
							printf("Error.You entered a label (as an operand) which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(tokenIndex,externsHead))  /*list of operands that used extern, needed for the ."ext" file*/
							newLabelOperand(tokenIndex, instructionLine+2, instructionLine+3, &operands);
					}
					if(validFlag){ /*if our code is valid, we will print the machine code for this line and update the line we are up to*/
						n=getMachineCode1(token2,token3,instructionLine,head,externsHead,fptr2);
						instructionLine+=n;
					}
				}

				if(hasTwoOperands(token2)){
					token3=strtok(NULL,",");
					token3=trimBeg(token3);
					token3=trimEnd(token3);
					if(isLabel(token3)){  /*get the label name, and ensure its either declared a label or an extern */
						if(!inLabels(token3,head) && !isInExtern(token3,externsHead)){
							printf("Error.You entered a label (as an operand) which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(token3,externsHead))  /*list of operands that used extern, needed for the ."ext" file*/
							newLabelOperand(token3, instructionLine+2, instructionLine+3, &operands);
					}
					if(isIndex(token3)){	/*get the label name, and ensure its either declared a label or an extern*/
						strcpy(tokenIndex,token3);
						trimBeg(tokenIndex);
						trimEnd(tokenIndex);
						(*(tokenIndex+strlen(tokenIndex)-5))='\0';   /*obtain the label that comes right before the index, to check it*/
						if(!inLabels(tokenIndex,head) && !isInExtern(tokenIndex,externsHead)){
							printf("Error.You entered a label (as an operand) which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(tokenIndex,externsHead))  /*list of operands that used extern, needed for the ."ext" file*/
							newLabelOperand(tokenIndex, instructionLine+2, instructionLine+3, &operands);
					}
					token4=strtok(NULL," \f\n\r\t\v");
					if(isLabel(token4)){  /*get the label name, and ensure its either declared a label or an extern */
						if(!inLabels(token4,head) && !isInExtern(token4,externsHead)){
							printf("Error.You entered a label (as an operand) which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(token4,externsHead)){  /*add to list of operands that used extern, needed for the ."ext" file*/
							if(isRegister(token3))
								newLabelOperand(token4, instructionLine+2, instructionLine+3, &operands);
							else if(isImmediate(token3))  /*we also add the exact address of the operands machine code*/
								newLabelOperand(token4, instructionLine+3, instructionLine+4, &operands);
							else if(isIndex(token3) || isLabel(token3))
								newLabelOperand(token4, instructionLine+4, instructionLine+5, &operands);
						}
					}
					if(isIndex(token4)){	/*get the label name, and ensure its either declared a label or an extern*/ 
						strcpy(tokenIndex,token4);
						trimBeg(tokenIndex);
						trimEnd(tokenIndex);
						(*(tokenIndex+strlen(tokenIndex)-5))='\0';  /*obtain the label that comes right before the index, to check it*/
						if(!inLabels(tokenIndex,head) && !isInExtern(tokenIndex,externsHead)){
							printf("Error.You entered a label (as an operand) which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(tokenIndex,externsHead))  /*list of operands that used extern, needed for the ."ext" file*/
							newLabelOperand(tokenIndex, instructionLine+2, instructionLine+3, &operands);
					}
					if(validFlag){	/*if our code is valid, we will print the machine code for this line and update the line we are up to*/		
						n=getMachineCode2(token2,token3,token4,instructionLine,head,externsHead,fptr2);
						instructionLine+=n;
					}
				}
				if(hasNoOperands(token2))
					if(validFlag){  /*if our code is valid, we will print the machine code for this line*/
						n=getMachineCode0(token2,instructionLine,fptr2);
						instructionLine+=n;
					}
			}
		}

/*this code is the same as previously, except that here we dont have a label declared. we will now parse the string and send to machine code*/
		else{
		     	if(isCommand(token1)){
				if(hasOneOperand(token1)){
					token2=strtok(NULL," \f\n\r\t\v");
					if(isLabel(token2)){
						if(!inLabels(token2,head) && !isInExtern(token2,externsHead)){
							printf("Error.You entered a label(as an operand)which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(token2,externsHead))
								newLabelOperand(token2, instructionLine+2, instructionLine+3, &operands);
					}
					if(isIndex(token2)){	/*get the label name, and ensure its either declared a label or an extern*/ 
						strcpy(tokenIndex,token2);
						trimBeg(tokenIndex);
						trimEnd(tokenIndex);
						(*(tokenIndex+strlen(tokenIndex)-5))='\0';   /*obtain the label that comes right before the index, to check it*/
						if(!inLabels(tokenIndex,head) && !isInExtern(tokenIndex,externsHead)){
							printf("Error.You entered a label (as an operand) which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(tokenIndex,externsHead))  /*list of operands that used extern, needed for the ."ext" file*/
							newLabelOperand(tokenIndex, instructionLine+2, instructionLine+3, &operands);
					}
					if(validFlag){
						n=getMachineCode1(token1,token2,instructionLine,head,externsHead,fptr2);
						instructionLine+=n;
					}
				}
				if(hasTwoOperands(token1)){
					token2=strtok(NULL,",");
					token2=trimBeg(token2);
					token2=trimEnd(token2);
					if(isLabel(token2)){
						if(!inLabels(token2,head) && !isInExtern(token2,externsHead)){
							printf("Error.You entered a label(as an operand)which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(token2,externsHead))
								newLabelOperand(token2, instructionLine+2, instructionLine+3, &operands);
						
					}

					if(isIndex(token2)){	/*get the label name, and ensure its either declared a label or an extern*/
						strcpy(tokenIndex,token2);
						trimBeg(tokenIndex);
						trimEnd(tokenIndex);
						(*(tokenIndex+strlen(tokenIndex)-5))='\0';  /*obtain the label that comes right before the index, to check it*/
						if(!inLabels(tokenIndex,head) && !isInExtern(tokenIndex,externsHead)){
						   printf("Error.You entered a label(as an operand)which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(tokenIndex,externsHead)) /*list of operands that used extern, needed for the ."ext" file*/
							newLabelOperand(tokenIndex, instructionLine+2, instructionLine+3, &operands);
					}

					token3=strtok(NULL," \f\n\r\t\v");
					if(isLabel(token3)){
						if(!inLabels(token3,head) && !isInExtern(token3,externsHead)){
							printf("Error.You entered a label(as an operand)which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(token3,externsHead)){
							if(isRegister(token2))
								newLabelOperand(token3, instructionLine+2, instructionLine+3, &operands);
							else if(isImmediate(token2))
								newLabelOperand(token3, instructionLine+3, instructionLine+4, &operands);
							else if(isIndex(token2) || isLabel(token2))
								newLabelOperand(token3, instructionLine+4, instructionLine+5, &operands);
						}
					}
					if(isIndex(token3)){	/*get the label name, and ensure its either declared a label or an extern*/
						strcpy(tokenIndex,token3);
						trimBeg(tokenIndex);   
						trimEnd(tokenIndex);
						(*(tokenIndex+strlen(tokenIndex)-5))='\0';  /*obtain the label that comes right before the index, to check it*/
						if(!inLabels(tokenIndex,head) && !isInExtern(tokenIndex,externsHead)){
							printf("Error.You entered a label (as an operand) which wasn't declared,nor is an extern\n");
							printf("In line %d\n",fileLine);
							validFlag=0;
						}
						if(isInExtern(tokenIndex,externsHead)){ /*list of operands that used extern, needed for the ."ext" file*/
							if(isRegister(token2))
								newLabelOperand(token3, instructionLine+2, instructionLine+3, &operands);
							else if(isImmediate(token2))
								newLabelOperand(token3, instructionLine+3, instructionLine+4, &operands);
							else if(isIndex(token2) || isLabel(token2))
								newLabelOperand(token3, instructionLine+4, instructionLine+5, &operands);
						}
					}
					if(validFlag){
						n=getMachineCode2(token1,token2,token3,instructionLine,head,externsHead,fptr2);
						instructionLine+=n;
						}
				}
				if(hasNoOperands(token1))
					if(validFlag){
						n=getMachineCode0(token1,instructionLine,fptr2);
						instructionLine+=n;
						}
			}
			if(strcmp(token1,".data")==0)
				continue;

			if(strcmp(token1,".string")==0)
				continue;
			
			if(strcmp(token1,".extern")==0)
				continue;
			
		
			if(strcmp(token1,".entry")==0){
				token2=strtok(NULL," \f\n\r\t\v");
				if(!inLabels(token2,head)){
					printf("Error.You have declared an entry label without actually declaring any such label in the code \nIn line %d\n",fileLine);
					validFlag=0;
				}
				if(isInExtern(token2,externsHead)){
					printf("Error. You cannot declare the same label as both entry and extern \nIn line %d\n",fileLine);
					validFlag=0;
				}
				if(alreadyIsEntry(token2,entriesHead))
					continue;

				newEntry(token2,&entriesHead);  /*if its a valid entry, create an entry and add it to the list*/
				continue;
			}
	}
	
}

	if(!validFlag){
		remove(obFileName);
		return 0;
	}
	if(validFlag){
		getDataArrayCode(fptr2,dataArray,DCF,instructionLine);
		createEntryFolder(fileName,entriesHead,head);
		createExternalFolder(fileName,externsHead,operands);
	}

return 1;

}

void getDataArrayCode(FILE *p, int data[],int DCF, int line ){
	struct word W;
	int i;
	unsigned int z;
	for(i=0;i<DCF;i++){
		W.A=1;
		W.R=0;
		W.E=0;
		z= data[i];
		W.values = ( 0xffff & z);
		printInHexa(W,line,p);
		line++;
	}

}







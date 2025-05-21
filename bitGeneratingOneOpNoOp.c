#include "temporaryStructHeader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char * trimBeg(char *x);
char * trimEnd(char *x);
int isInExtern(char *, struct external *);
int isImmediate(char *);
int isIndex(char *);
int isLabel(char *);
int isRegister(char *);
int getValue(char *, struct label *);
int calculateBase(int);
int calculateOffset(int);
void printInHexa(struct word,int, FILE *p);

/*this function recieves a 20 bit word, and split the bits into 5 groups (A,B,C,D,E) of 4 bits, and prints them in their hexadecimal value*/


void printInHexa(struct word wordle,int lineNum,FILE *fptr){
int a;
unsigned int b,c,d,e;
if(wordle.A & 1)
	  a=4;
if(wordle.R & 1) 
	  a=2;
if(wordle.E & 1)
	  a=1;
b =( (0xf000) & wordle.values ) >> 12;
c =( (0x0f00) & wordle.values ) >> 8;   
d =( (0x00f0) & wordle.values ) >> 4;  /*masking and manipulating to get the exact 4 bits we need*/
e =( (0x000f) & wordle.values );

fprintf(fptr,"%04d   %c%x-%c%x-%c%x-%c%x-%c%x \n",lineNum,'A',a,'B',b,'C',c,'D',d,'E',e);  /*print the 20 - bit word via the 5 groups we designated*/
}

/*this function recieves one parameter (a command) and generates the binary code for that command*/
int getMachineCode0(char *a, int line, FILE *objectFile){
	struct word word1;
	word1.bit19=0;
	word1.A=1;    /*for the first word (and second, when there are operands), A is always 1, R and E are always 0*/
	word1.R=0;
	word1.E=0;

	if(strcmp(a,"rts")==0){
		word1.values = (0xffff & 1<<14);
	}
	if(strcmp(a,"stop")==0){
		word1.values = (0xffff & 1<<15);
	}
	printInHexa(word1,line,objectFile);
	return 1;
}

/*This functions recieves two parameters (one operand and one command) and generates the binary word based on the addresses, modes,and identifiers of the command and operand*/
int getMachineCode1(char *a, char *b, int line, struct label *head, struct external *externsHead,FILE *objectFile){
	struct word word1;
	struct word word2;
	struct word firstOperandWord1;
	struct word firstOperandLabelBase;
	struct word firstOperandLabelOffset;
	struct word firstOperandIndex1;
	struct word firstOperandIndex2;

	unsigned int x;
	int z,sign;
	char copy1[36],tokenRegister[4],tokenLabel[32];
	
	word1.bit19=0;
	word2.bit19=0;
	word1.A=1;
	word1.R=0;
	word1.E=0;			/*for the first two words, A is always 1, R and E is 0*/
	word2.A=1;
	word2.R=0;
	word2.E=0;

	if(strcmp(a,"clr")==0){
		word1.values = (0xffff & 1<<5);
		word2.values = (0xf000 & (10 << 12));
	}
	if(strcmp(a,"not")==0){
		word1.values = (0xffff & 1<<5);
		word2.values = (0xf000 & (11 << 12));
	}
	if(strcmp(a,"inc")==0){
		word1.values = (0xffff & 1<<5);
		word2.values = (0xf000 & (12 << 12));
	}
	if(strcmp(a,"dec")==0){
		word1.values = (0xffff & 1<<5);
		word2.values = (0xf000 & (13 << 12));   /*these statements will generate the code for opcode and funct (in the first 2 words), using masking*/
	}
	if(strcmp(a,"jmp")==0){
		word1.values = (0xffff & 1<<9);
		word2.values = (0xf000 & (10 << 12));
	}
	if(strcmp(a,"bne")==0){
		word1.values = (0xffff & 1<<9);
		word2.values = (0xf000 & (11 << 12));
	}
	if(strcmp(a,"jsr")==0){
		word1.values = (0xffff & 1<<9);   
		word2.values = (0xf000 & (12 << 12));
	}
	if(strcmp(a,"red")==0){
		word1.values = (0xffff & 1<<12);
		word2.values = (0xf000 & 0);
	}
	if(strcmp(a,"prn")==0){
		word1.values = (0xffff & 1<<13);
		word2.values = (0xf000 & 0);
	}

	if(isRegister(b)){
		x =atoi((b+1));
		word2.values |= 0x0003;    /*generate code based on register number and addressing mode*/
		word2.values |= (x<<2) ;
		printInHexa(word1,line++,objectFile);
		printInHexa(word2,line,objectFile);
		return 2;
	}
	if(isImmediate(b)){
		sign = ( (*(b+1))=='-') ? -1 : 1;
		x= ( (*(b+1))=='-' || (*(b+1))=='+') ? (unsigned)atoi(b+2):(unsigned)atoi(b+1);  
		x*=sign;                           /*generate code based on the value after the # sign (supports optional + or - signs)*/ 
		firstOperandWord1.A=1;
		firstOperandWord1.R=0;
		firstOperandWord1.E=0;
		firstOperandWord1.values = (0xffff & x);	 
		printInHexa(word1,line++,objectFile);
		printInHexa(word2,line++,objectFile);
		printInHexa(firstOperandWord1,line,objectFile);
		return 3;
	}
	if(isLabel(b)){
		word2.values |= 0x0001;
		firstOperandLabelBase.A=0;
		firstOperandLabelOffset.A=0;   /*generate code (2 more words) based on the labels address (base and offset), or 0 if label is external*/
		if(isInExtern(b,externsHead)){
			firstOperandLabelBase.R=0;
			firstOperandLabelBase.E=1;
			firstOperandLabelBase.values &= 0;  /*if its external then A and R will be 0, E will be 1*/
			firstOperandLabelOffset.R=0;
			firstOperandLabelOffset.E=1;
			firstOperandLabelOffset.values &= 0;
		}
		else{   /*is not external*/
			firstOperandLabelBase.R=1;
			firstOperandLabelBase.E=0;
			firstOperandLabelOffset.R=1;  /*for a non-external label, A will be 0, R will be 1, and E will be 0*/
			firstOperandLabelOffset.E=0;
			z=getValue(b,head);
			firstOperandLabelBase.values = (0xffff & (calculateBase(z)));
			firstOperandLabelOffset.values = (0xffff & (calculateOffset(z)));
		}
		printInHexa(word1,line++,objectFile);
		printInHexa(word2,line++,objectFile);   /*prints the words in hexa format*/
		printInHexa(firstOperandLabelBase,line++,objectFile);
		printInHexa(firstOperandLabelOffset,line++,objectFile);
		return 4;  /*to update which line of machine code*/
	}
	if(isIndex(b)){
		int x;  
		strcpy(copy1,b);
		strcpy(tokenLabel,strtok(copy1,"["));
		trimBeg(tokenLabel);			/*gets the register number, and the label name*/
		trimEnd(tokenLabel);
		strcpy(tokenRegister,strtok(NULL,"]"));
		trimBeg(tokenRegister);
		trimEnd(tokenRegister);
		x=atoi((tokenRegister+1));	
		word2.values |= 0x0002;		/*assigns the "register addressing mode" (for destination operand)*/
		word2.values  |= (x<<2);		/*assigns the "register number" (that is in the index)*/
		firstOperandIndex1.A=0;
		firstOperandIndex2.A=0;
		if(isInExtern(tokenLabel,externsHead)){  /*If this label is external, the R is 0 while E is 1.If not, R is 1 while E is 0. Either way A is 0*/
			firstOperandIndex1.R=0;
			firstOperandIndex1.E=1;
			firstOperandIndex1.values &= 0;
			firstOperandIndex2.R=0;		/*If external- words 3 and 4, which would contain the address(base/offset), now contain 0*/
			firstOperandIndex2.E=1;
			firstOperandIndex2.values &= 0;
		}
		else{				/*Not external*/
			firstOperandIndex1.R=1;
			firstOperandIndex1.E=0;
			firstOperandIndex2.R=1;
			firstOperandIndex2.E=0;
			z= getValue(tokenLabel,head);
		firstOperandIndex1.values = (0xffff & (calculateBase(z)));  /*gets the labels address from the list, and computes based on the base/offset*/
		firstOperandIndex2.values = (0xffff & (calculateOffset(z)));
		}
	
		printInHexa(word1,line++,objectFile);
		printInHexa(word2,line++,objectFile);            /*prints the 20- bit code representation in hexa form*/
		printInHexa(firstOperandIndex1,line++,objectFile);
		printInHexa(firstOperandIndex2,line++,objectFile);
		return 4;
	}
	return 0;
}

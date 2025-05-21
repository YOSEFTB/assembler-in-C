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


/*This functions recieves to operands and a command, and generates a 20 bit structure bases on the addresses of labels and the operands addressing modes and other  identifiers . It will be printed later in hexa form*/				
	
int getMachineCode2(char *a, char *b, char *c, int line, struct label *head, struct external *externsHead, FILE *objectFile){
	struct word word1;
	struct word word2;
	struct word firstOperandImmediate;
	struct word firstOperandWord1;
	struct word firstOperandWord2;
	struct word firstOperandIndex1;
	struct word firstOperandIndex2;
	struct word secondOperandImmediate;
	struct word secondOperandWord1;
	struct word secondOperandWord2;
	struct word secondOperandIndex1;
	struct word secondOperandIndex2;
	int flag1,flag2,increase; 			 /*these will keep track of how many words were printed (and will notify the calling function,so that the 									next code addresses will be accurate)*/
	unsigned int x;
	int sign,z;
	char copy1[36],tokenRegister[4],tokenLabel[32];
	
	word1.bit19=0;
	word2.bit19=0;     /*for the first two words, A is always 1, R and E always 0*/
	word1.A=1;
	word1.R=0;
	word1.E=0;
	word2.A=1;
	word2.R=0;
	word2.E=0;

	if(strcmp(a,"mov")==0){
		word1.values =(0xffff & 0x0001);   /*in these functions, we use masking to obtain the opcode and funct for the commands*/
		word2.values &= 0;
	}
	if(strcmp(a,"cmp")==0){
		word1.values = (0xffff & 0x0002);
		word2.values &= 0;
	}
	if(strcmp(a,"add")==0){
		word1.values = (0xffff & 0x0004);
		word2.values = (0xffff & 0xa000);
	}
	if(strcmp(a,"sub")==0){
		word1.values = (0xffff & 0x0004);
		word2.values = (0xffff & 0xb000);
	}
	if(strcmp(a,"lea")==0){
		word1.values = (0xffff & 0x0010);
		word2.values &= 0;
	}

	if(isRegister(b)){
		flag1=3;
		x=atoi((b+1));
		word2.values |= 3<<6;   /*these are source operands, now computing the addressing mode and and register number*/
		word2.values |= (x<<8) ;
	}
	if(isImmediate(b)){
		flag1=0;
		sign = ( (*(b+1))=='-') ? -1 : 1;
		x= ( (*(b+1))=='-' || (*(b+1))=='+') ? (unsigned)atoi(b+2):(unsigned)atoi(b+1);
		x*=sign;
		firstOperandImmediate.A=1;		/*we obtain the immediate value using atoi, and mask it for 16 bits, and save in the word*/
		firstOperandImmediate.R=0;
		firstOperandImmediate.E=0;
		firstOperandImmediate.values = (0xffff & x);
	}
	if(isLabel(b)){
		flag1=1;
		word2.values |= 1<<6;   /*addressing mode (for source) of label (direct)*/
		firstOperandWord1.A=0;
		firstOperandWord2.A=0;
		if(isInExtern(b,externsHead)){
			firstOperandWord1.R=0;
			firstOperandWord1.E=1;
			firstOperandWord1.values &= 0;  /*for extern, A and R are 0, E is 1, and the address (base/offset) is 0*/
			firstOperandWord2.R=0;
			firstOperandWord2.E=1;
			firstOperandWord2.values &= 0;
		}
		else{
			firstOperandWord1.R=1;
			firstOperandWord1.E=0;	/*for non extern, A and E are 0, R is 1, and the address (base/offset) is obtained with the function*/
			firstOperandWord2.R=1;
			firstOperandWord2.E=0;
			z=getValue(b,head);
		firstOperandWord1.values = (0xffff & (calculateBase(z)));
		firstOperandWord2.values = (0xffff & (calculateOffset(z)));
		}
		
	}
	if(isIndex(b)){
		flag1=2;
		strcpy(copy1,b);
		strcpy(tokenLabel,strtok(copy1,"["));
		trimBeg(tokenLabel);			/*obtains the label name and the register number*/
		trimEnd(tokenLabel);
		strcpy(tokenRegister,strtok(NULL,"]"));
		trimBeg(tokenRegister);
		trimEnd(tokenRegister);
		x=atoi((tokenRegister+1));	
		word2.values  |= (x<<8);		/*assigns the "register number" (that is in the index)*/
		word2.values |= 2<<6;		/*assigns the "index addressing mode" (for destination operand)*/
		firstOperandWord1.A=0;
		firstOperandWord2.A=0;
		if(isInExtern(tokenLabel,externsHead)){/*If this label is external,the R is 0 while E is 1.If not external, R is 1 while E is 0.Either way A is 0*/
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
			z=getValue(tokenLabel,head);
		firstOperandIndex1.values = (0xffff & (calculateBase(z)));  /*gets the labels address from the list, and computes based on the base/offset*/
		firstOperandIndex2.values = (0xffff & (calculateOffset(z)));
		}
	}



		/*building words for the destination operand*/

	if(isRegister(c)){
		flag2=3;
		x=atoi((c+1));
		word2.values |= 3;   /*same method as before, only that these are now destination operands */
		word2.values |= (x<<2) ;
	}
	if(isImmediate(c)){
		flag2=0;
		sign = ( (*(c+1))=='-') ? -1 : 1;
		x= ( (*(c+1))=='-' || (*(c+1))=='+') ? (unsigned)atoi(c+2):(unsigned)atoi(c+1);
		x*=sign;
		secondOperandImmediate.A=1;
		secondOperandImmediate.R=0;		/*same method as before, just for destination operand*/
		secondOperandImmediate.E=0;
		secondOperandImmediate.values = (0xffff & x);
	}

	if(isLabel(c)){
		flag2=1;
		word2.values |= 1;
		secondOperandWord1.A=0;
		secondOperandWord2.A=0;
		if(isInExtern(c,externsHead)){
			secondOperandWord1.R=0;   /*same method as for the source operand*/
			secondOperandWord1.E=1;
			secondOperandWord1.values &= 0;
			secondOperandWord2.R=0;
			secondOperandWord2.E=1;
			secondOperandWord2.values &= 0;
		}
		else{
			secondOperandWord1.R=1;
			secondOperandWord1.E=0;   /*same method as for the source operand*/
			secondOperandWord2.R=1;
			secondOperandWord2.E=0;
		z=getValue(c,head);  
		secondOperandWord1.values = (0xffff & (calculateBase(z)));  
		secondOperandWord2.values = (0xffff & (calculateOffset(z)));
		}
	}
	if(isIndex(c)){
		flag2=2;
		strcpy(copy1,c);
		strcpy(tokenLabel,strtok(copy1,"["));
		trimBeg(tokenLabel);
		trimEnd(tokenLabel);
		strcpy(tokenRegister,strtok(NULL,"]"));
		trimBeg(tokenRegister);
		trimEnd(tokenRegister);
		x=atoi((tokenRegister+1));	
		word2.values  |= (x<<8);		/*assigns the "register number" (that is in the index)*/
		word2.values |= 2;		/*assigns the "register addressing mode" (for source operand)*/
		secondOperandIndex1.A=0;
		secondOperandIndex2.A=0;
		if(isInExtern(tokenLabel,externsHead)){/*If this label is external,the R is 0 while E is 1.If not external, R is 1 while E is 0.Either way A is 0*/
			secondOperandIndex1.R=0;
			secondOperandIndex1.E=1;
			secondOperandIndex1.values &= 0;
			secondOperandIndex2.R=0;		/*If external- words 3 and 4, which would contain the address(base/offset), now contain 0*/
			secondOperandIndex2.E=1;
			secondOperandIndex2.values &= 0;
		}
		else{				/*Not external*/
			secondOperandIndex1.R=1;
			secondOperandIndex1.E=0;
			secondOperandIndex2.R=1;
			secondOperandIndex2.E=0;	
 /*we will now compute the value of the label,based on base/offset value (we can find the value in the label list)*/
		z=getValue(tokenLabel,head);
		secondOperandIndex1.values = (0xffff & (calculateBase(z)));  
		secondOperandIndex2.values = (0xffff & (calculateOffset(z)));
		}
	}

	increase=0;
	printInHexa(word1,line++,objectFile);
	printInHexa(word2,line++,objectFile);			/*with these statements we print the binary code, and increment the line number*/
	increase+=2;
	if(flag1==0){
		printInHexa(firstOperandImmediate,line++,objectFile);
		increase+=1;
	}
	if(flag1==1){
		printInHexa(firstOperandWord1,line++,objectFile);
		printInHexa(firstOperandWord2,line++,objectFile);
		increase+=2;
	}
	if(flag1==2){
		printInHexa(firstOperandIndex1,line++,objectFile);
		printInHexa(firstOperandIndex2,line++,objectFile);
		increase+=2;
	}

	if(flag2==0){
		printInHexa(secondOperandImmediate,line,objectFile);
		increase+=1;
	}
	if(flag2==1){
		printInHexa(secondOperandWord1,line++,objectFile);
		printInHexa(secondOperandWord2,line,objectFile);
		increase+=2;
	}
	if(flag2==2){
		printInHexa(secondOperandIndex1,line++,objectFile);
		printInHexa(secondOperandIndex2,line,objectFile);
		increase+=2;			/*we return the number of words printed, to the calling function, so it will increment the line number*/
	}
	return increase;
}


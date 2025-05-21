#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "temporaryStructHeader.h"

int isImmediate(char *x);
int isIndex(char *x);
int isCommand(char *x);
int isRegister(char *x);
int labelOperandValidity(char *x);
int validImmediate(char *x);
int validIndex(char *x);
int validLength(char *x);

	/*this function is called when a 1 operand command is called. it returns whether or not the operand type is valid for the specific command*/
int operandValidity1(char *x, char *y){  
	if(isImmediate(y) && !strcmp(x,"prn")==0){
		printf("Incompatible destination operand. Destination operand of this command cannot be of immediate addressing mode\n");
		return 0;
	}
	if( (strcmp(x,"jmp")==0 || strcmp(x,"bne")==0 || strcmp(x,"jsr")==0) && isRegister(y) ){
		printf("Incompatible destination operand. Destination operand of this command cannot be of register addressing mode\n");
		return 0;
	}
	return 1;
}

	/*this function is called when a 2 operand command is called. it returns whether or not the operand types are valid for the specific command*/
int operandValidity2(char *x, char *y, char *z){
	if(strcmp(x,"mov")==0 || strcmp(x,"add")==0 || strcmp(x,"sub")==0){
		if(isImmediate(z)){
			printf("Incompatible destination operand. Destination operand of this command cannot be of immediate addressing mode\n");
			return 0;		
		}
	}
	if(strcmp(x,"lea")==0){
		if(isImmediate(y) || isRegister(y)){
			printf("Incompatible source operand. Source operand of this command cannot be of immediate or register addressing mode\n");
			return 0;
		}
		if(isImmediate(z)){
			printf("Incompatible destination operand. Destination operand of this command cannot be of immediate addressing mode\n");
			return 0;
		}
	}
	return 1;
}

/*this function is called on operands to see if they have the correct syntax or not*/
int syntaxCheck(char *x){
	if(isImmediate(x)){    /*if the operand is of immediate addressing mode*/
		if(!validImmediate(x)){
			printf("Error. You have incorrectly entered the immediate addressing mode\n");
			return 0;
		}
		return 1;
	}
	else if(isIndex(x)){		/*if the operand is of index addressing mode*/
		if(!validIndex(x)){
			printf("Error. You have incorrectly entered the immediate addressing mode\n");
			return 0;
		}
		return 1;
	}
	else if(isRegister(x))	/*if the operand is of immediate addressing mode (the syntax of a register number does not need to be checked HERE)*/
		return 1;
	else {
		if(!labelOperandValidity(x))   /*if its none of the above, it must be a label operand. Check that it has valid label syntax*/
			return 0;
	return 1;
	}

}

int validImmediate(char *x){	/*returns whether or not the syntax is valid for immediate addressing mode*/
	int i;
	if( (*(x+1))!='-' && (*(x+1))!='+' && !isdigit( *(x+1) ))
		return 0;
	for(i=2;i<strlen(x);i++){
		if( !isdigit(*(x+i)) )
			return 0;
	}
	return 1;
}

int validIndex(char *x){	/*returns whether or not the syntax is valid for index addressing mode*/
	int i;
	for(i=0;(*(x+i))!='\0';i++){
		if( (*(x+i))=='[')
			break;
	}
		if( (*(x+i+1))=='r' && (*(x+i+2))=='1')
			if( (*(x+i+4))==']')
				if( (*(x+i+3))=='0'|| (*(x+i+3))=='1'|| (*(x+i+3)) =='2'|| (*(x+i+3))=='3'|| (*(x+i+3))=='4'|| (*(x+i+3))=='5' ) 
					return 1;
		return 0;
}

int validLength(char *x){		/*returns whether or not this line of code is of correct length*/
	int i,l;
	for(i=0;(*(x+i))!='\n' && i<strlen(x) ;i++)
		l++;
	if(l>80)
		return 0;
	return 1;
}


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int labelValidity(char *x);
int dataValidity();
int stringValidity();
int isCommand(char *x);
int hasTwoOperands(char *x);
int hasOneOperand(char *);
int hasNoOperands(char *);
char * trimBeg(char *x);
char * trimEnd(char *x);
int syntaxCheck(char *x);
int isPotentialLabel(char *x);
int labelOperandValidity(char *x);
int isImmediate(char *x);
int isIndex(char *x);
int isRegister(char *x);
int validImmediate(char *x);
int validIndex(char *x);
int operandValidity2(char *x, char *y,char *z);
int operandValidity1(char *x, char *y);
int dataValidity(char *x);
int stringValidity(char *x);
int isComment(char *x);
int isEmpty(char  *x);

/*this function is the main validity checking of the program, the code is read, line by line, and parsed to detect any sort of error
examples- too many operands for a command, too few, invalid addressing mode for operand, extraneous code at end of line,operands not separated by commas,  invalid label name, invalid operand syntax. data not separated by commas, too many commas, string not between string delimiters,missing delimiter, etc.	*/
int validityCheck(char *line){
	
	int i;
	char *token1,*token2,*token3,*token4,*lineCopy,*lineCopy2;
	token1=malloc(80*sizeof(char));
	token2=malloc(80*sizeof(char));
	token3=malloc(80*sizeof(char));
	token4=malloc(80*sizeof(char));
	lineCopy=malloc(80*sizeof(char));
	lineCopy2=malloc(80*sizeof(char));
	if(isComment(line) || isEmpty(line))
		return 1;

	for(i=0;i<80;i++){
		*(token1+i)='\0';
		*(token2+i)='\0';
		*(token3+i)='\0';
		*(token4+i)='\0';
		*(lineCopy+i)='\0';
		*(lineCopy2+i)='\0';
	}
	strcpy(lineCopy,line);
	strcpy(lineCopy2,line);
	token1= strtok(lineCopy," \f\n\r\t\v");
	if(*(token1+strlen(token1)-1)==':'){
		*(token1+strlen(token1)-1)='\0'; /*if theres a label we check its validity*/
		if(!labelValidity(token1))
			return 0;
		token2= strtok(NULL," \f\n\r\t\v");  /*there must be code after the label*/
		if(token2==NULL){
			printf("Error. You have not entered any commands or instructions after the label declaration\n");
			return 0;
		}
		if (strcmp(token2,".data")==0){   /*check validity of the data*/
			if(!dataValidity(line+strlen(token1)+1))
				return 0;
		}
		else if (strcmp(token2,".string")==0){  /*check validity of the string*/
			if(!stringValidity(line+strlen(token1)+1))
				return 0;
		}
		else if (isCommand(token2)){  /*check validity of the commands-right amount,right type, comma separator, */
			if(hasTwoOperands(token2)){
				token3=strtok(NULL,",");
				if(token3==NULL){
					printf("Error. You have not entered a comma to separate operands");
					return 0;
				}
				trimBeg(token3);
				trimEnd(token3);
				if(!syntaxCheck(token3)){  /*checks the syntax of the operand*/
					return 0;
				}
				token4=strtok(NULL," \f\n\r\t\v");
				if(token4==NULL){
					printf("Error. You have not entered enough operands for this command");
					return 0;
				}
				if(!syntaxCheck(token4)){  /*checks the syntax of the operand*/
					return 0;
				}

				if(!operandValidity2(token2,token3,token4))  /*checks that the type of operand and command are compatible*/
					return 0;
				if(strtok(NULL," \f\n\r\t\v")!=NULL){
					printf("Error. Extraneous code detected in this line\n");
					return 0;
				}
			}
			else if(hasOneOperand(token2)){  
				if((token3=strtok(NULL," \f\n\r\t\v"))==NULL ){
					printf("Error. You have not entered the operand for this command\n");
					return 0;
				}
				if(!syntaxCheck(token3))  /*checks the syntax of the operand*/
					return 0;
				if(!operandValidity1(token2,token3))  /*checks that the type of operand and command are compatible*/
					return 0;
				if(strtok(NULL," \f\n\r\t\v")!=NULL){  /*checks for too much code on line*/
					printf("Error. Extraneous code detected in this line\n");
					return 0;
				}
			}
			else if(hasNoOperands(token2)){   /*checks for too much code on line*/
				if(strtok(NULL," \f\n\r\t\v")!=NULL){
					printf("Error. Extraneous code detected in this line\n");
					return 0;
				}
			}
		}
		else if(strcmp(token2,".entry")==0){   /*checks that there is an actual entry declared after the declaration*/
			if((token3=strtok(NULL," \f\n\r\t\v"))==NULL){
				printf("Error. You must enter a label name after the entry declaration\n");
				return 0;
			}
			if(!labelOperandValidity(token3))   /*checks validity of the entry*/
				return 0;
			if(strtok(NULL," \f\n\r\t\v")!=NULL){  /*checks for too much code on line*/
				printf("Error. Extraneous code detected in this line, after the entry declaration\n");
				return 0;
			}
		}
			
		else if(strcmp(token2,".extern")==0){
			if((token3=strtok(NULL," \f\n\r\t\v"))==NULL){   /*checks that there is an actual extern declared after the declaration*/
				printf("Error. You must enter a label name after the extern declaration\n");
				return 0;
			}
			if(!labelOperandValidity(token3))  /*checks validity of the extern*/
				return 0;
			if(strtok(NULL," \f\n\r\t\v")!=NULL){ /*checks for too much code on line*/
				printf("Error. Extraneous code detected in this line, after the entry declaration\n");
				return 0;
			}
		}
		else{		/*if its not a command, data,string or entry/extern - then its an error*/
			printf("Error. You have entered an invalid instruction\n");
			return 0;
		}
	}

	/*these functions are the same as the previous ones, except now we are in the stage where no label was declared. we will continue to parse the code*/

	else if (strcmp(token1,".data")==0){
		if(!dataValidity(line))
			return 0;
	}
	else if (strcmp(token1,".string")==0){
		if(!stringValidity(line))
			return 0;
	}
	else if (isCommand(token1)){
		if(hasTwoOperands(token1)){
			token2=strtok(NULL,",");
			if(token2==NULL ){
				printf("Error. You have not entered a comma to separate operands\n");
				return 0;
			}
			token2=trimBeg(token2);
			token2=trimEnd(token2);
			if(!syntaxCheck(token2))
				return 0;
			token3=strtok(NULL," \f\n\r\t\v");
			if(token3==NULL){
				printf("Error. You have not entered enough operands for this command\n");
				return 0;
			}
			if(!syntaxCheck(token3))
				return 0;
			if(!operandValidity2(token1,token2,token3))
				return 0;
			if(strtok(NULL," \f\n\r\t\v")!=NULL){
				printf("Error. Extraneous code detected in this line\n");
				return 0;
			}
		}
		else if(hasOneOperand(token1)){
			token2=strtok(NULL," \f\n\r\t\v");
			if(token2==NULL){
				printf("Error. You have not entered the operand for this command\n");
				return 0;
			}
			if(!syntaxCheck(token2))
				return 0;
			if(!operandValidity1(token1,token2))
				return 0;	
			if(strtok(NULL," \f\n\r\t\v")!=NULL){
				printf("Error. Extraneous code detected in this line\n");
				return 0;
			}
		}
		else if(hasNoOperands(token1)){
			if(strtok(NULL," \f\n\r\t\v")!=NULL){
				printf("Error. Extraneous code detected in this line\n");
				return 0;
			}
		}
		else{
			printf("Error. You have entered an invalid command \n");
			return 0;
		}
	}
	else if(strcmp(token1,".entry")==0){
		token2=strtok(NULL," \f\n\r\t\v");
		if(token2==NULL){
			printf("Error. You must enter a label name after the entry declaration\n");
			return 0;
		}
		if(!labelOperandValidity(token2))
			return 0;
		if(strtok(NULL," \f\n\r\t\v")!=NULL){
			printf("Error. Extraneous code detected in this line, after the entry declaration\n");
			return 0;
		}
	}
	else if(strcmp(token1,".extern")==0){
		if((token2=strtok(NULL," \f\n\r\t\v"))==NULL){
			printf("Error. You must enter a label name after the extern declaration\n");
			return 0;
		}
		if(!labelOperandValidity(token2))
			return 0;
		if(strtok(NULL," \f\n\r\t\v")!=NULL){
			printf("Error. Extraneous code detected in this line, after the entry declaration\n");
			return 0;
		}
	}
	else{
			printf("Error. You have entered an invalid instruction \n");
			return 0;
	}

	return 1;
}


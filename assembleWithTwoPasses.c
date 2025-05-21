#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "temporaryStructHeader.h"
#include "assembleHeader.h"


/*this functions is the main part of the assembler. It calls the assembly file to obtain a list of data entered in the file, and of the labels and their addresses, both of which will be used later for the machine code. Then the assembler obtains a list of the extern labels. And finally, the assembler reads the asembly file once more, with the complete data list and label list, performs some last label validity checking, and then prints the machine code of each line to the object file*/

void assembleWithTwoPasses(FILE *fptr, char *fileName){
	int val;
	struct external *externsHead;
	struct label *listHead;
	char *removeFile=malloc((strlen(fileName)+4)*sizeof(char));
	int dataArray[8192];
	int flag,ICF,DCF;   /*dcf is needed to update the labels, and both are needed for the object file*/
	listHead= (struct label*)malloc(sizeof(struct label*));
	flag=1;
	ICF=0,DCF=0;
	externsHead=NULL;
	memset(dataArray,'\0',8192);
	listHead=firstPassing(fptr, dataArray, &flag, &ICF, &DCF);  /*gets the data (.string and .data), and returns a pointer to the list of labels*/
	rewind(fptr);
	externsHead=getExterns(fptr,&externsHead);  /*returns a pointer to the list of external labels*/
	rewind(fptr);
	val=secondPassing(fptr,fileName,listHead,externsHead,dataArray,ICF,DCF,flag);  /*this is where the assembler generates the machine code*/
	if(!val){
		strcpy(removeFile,fileName);
		strcat(removeFile,".am");
		*(removeFile+strlen(fileName)+4)='\0';
		remove(removeFile);
	}
}

/*this function parses the code, and calculates how many lines each command will take up,and based on that can know each labels address.this function also obtains a list (in array form) of all the data enter in instruction lines, whether numbers or characters*/

struct label* firstPassing(FILE *f,  int data[], int *validityFlag, int *ICF, int *DCF){

	int i,fileLine,location,IC,DC,l;
	char *codeLine,*codeCopy,*codeCopy2;
	char *token1,*token2,*token3,*token4;
	struct label *listHead;
	char *name;
	char attribute[10];
	IC=100,DC=0,fileLine=0;
	listHead=NULL;
	codeLine=malloc(83*sizeof(char));
	codeCopy=malloc(82*sizeof(char));
	codeCopy2=malloc(82*sizeof(char));
	name=malloc(32*sizeof(char));
	token1=malloc(33*sizeof(char));
	token2=malloc(8*sizeof(char));
	token3=malloc(5*sizeof(char));
	token4=malloc(8*sizeof(char));

	for(i=0;i<82;i++){
		*(codeLine+i)='\0';
		*(codeCopy+i)='\0';
		*(codeCopy2+i)='\0';
	}

	while(fgets(codeLine,83,f)!=NULL){
		fileLine++;			/*keeps track of what line we are in, in the file*/	
		if(isEmpty(codeLine) || isComment(codeLine))	/*skip empty lines and comment lines*/
			continue;
			if(!validLength(codeLine)){
				printf("Error. You have entered too long a line of code \nIn line %d\n",fileLine);
				*validityFlag=0;
				continue;
			}
		strcpy(codeCopy,codeLine);
		strcpy(codeCopy2,codeLine);
	if(validityCheck(codeLine)){	/*this function checks for all the validity requirements(except minor label details, which will be checked later) */
		token1=strtok(codeCopy," \f\n\r\t\v");		
		if(strcmp(token1+strlen(token1)-1,":")==0){   /*this means a label is being declared*/
			*(token1+strlen(token1)-1)='\0';
			if(alreadyDeclared(token1,listHead)){
				printf("Error. You cannot declare a label twice\nIn line %d\n",fileLine);
				*validityFlag=0;
			}
			strcpy(name,token1);
			token2=strtok(NULL," \f\n\r\t\v");	/*parsing each code line to see its operands or instructions*/
			if(isInstruction(token2)){		
					strcpy(attribute,"data");  /*store the label with the data counter and this attribute*/	
					location=DC;
					if(strcmp(token2,".data")==0){   /*this is a data instruction, the numbers will be stored in data array*/
						l=getData(codeCopy2,data,DC,1);
						DC+=l;
						(*DCF)+=l;  /*increment data amounts, so the addresses for data labels will be accurate*/
					}
					else{
						l=getString(codeCopy2,data,DC);/*this is a string instruction, the characters will be stored in data array*/
						DC+=l;
						(*DCF)+=l;	/*increment data amounts, so the addresses for data labels will be accurate*/
					}
			}

			else if(isCommand(token2)){
				strcpy(attribute,"code");  /*store the label with the code counter and this attribute*/		
				location=IC;
				if(hasTwoOperands(token2)){		/*parsing the line to see how many operands, lines etc.*/
					token3= strtok(NULL,",");
					token4= strtok(NULL," \f\n\r\t\v");
					token3=trimBeg(token3);
					token3=trimEnd(token3);
					
					l=wordsForCommand2(token3,token4);  /*this calculates how many words will be needed for this line*/
					IC+=(l+2);
					(*ICF)+=(l+2);		/*increment code amounts, so the addresses for code labels will be accurate*/
				}
				if(hasOneOperand(token2)){
					token3=strtok(NULL," \f\n\r\t\v");  /*continue parsing for more operands*/
					
					l=wordsForCommand(token3);
						IC+=(l+2);
						(*ICF)+=(l+2);		/*increment code amounts, so the addresses for code labels will be accurate*/
					}	
				if(hasNoOperands(token2)){
					IC+=1;
					(*ICF)+=1;		/*increment code amounts, so the addresses for code labels will be accurate*/
				}
			}
			else if(strcmp(token2,".entry")==0){   /*entry declared after a label is declared, not good. label is ignored*/
				printf("Warning- labels cannot be declared with an entry command \nIn line %d",fileLine);
					continue;
			}
			else{			/*extern declared after a label is declared, not good. label is ignored*/
				printf("Warning- labels cannot be declared with an extern command \nIn line %d",fileLine);
				continue;
			}
		
		newLabel(name,location,attribute,&listHead);  /*this function creates a new label, with deatails about its address and attribute(code or data)*/

		} 

	 /*this next part of code is identical to the previous part, except that here we dont have a label declared. we will continue to parse the line */

		else  if(isInstruction(token1)){
			if(strcmp(token1,".data")==0){
				l=getData(codeCopy2,data,DC,0);
				DC+=l;
				(*DCF)+=l;
			}
			else{
				l=getString(codeCopy2,data,DC);
				DC+=l;
				(*DCF)+=l;
			}

		}
		else if(isCommand(token1)){
			if(hasTwoOperands(token1)){
				token2= strtok(NULL,",");
				trimEnd(token2);
				trimBeg(token2);
				token3= strtok(NULL," \f\n\r\t\v");
				
				l=wordsForCommand2(token2,token3);
				IC+=(l+2);
				(*ICF)+=(l+2);
			}
			else if(hasOneOperand(token1)){
				token2=strtok(NULL," \f\n\r\t\v");
				
				l=wordsForCommand(token2);
				IC+=(l+2);
				(*ICF)+=(l+2);
			}
			else if(hasNoOperands(token1)){
				IC+=1;
				(*ICF)+=1;
			}
		}
		else if(strcmp(token1,".entry")==0){
			continue;
		}
		else if(strcmp(token1,".extern")==0){
			continue;
		}
		
	}
	else{   /*if the validity check came back negative, a reason is printed, and the line number wjere the error occured is printed*/
		printf("In line %d \n",fileLine);

		*validityFlag=0;   /*this is to ensure that no machine code is generated at the end*/
	}
}

updateDataLabels(listHead,*ICF);  /*in order to seperate the data from the commands, we will add ICF to every "data" label, to ensure it comes after "code"*/

return listHead;
}


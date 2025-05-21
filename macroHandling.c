#define MAX_MACRO_NAME_LENGTH 31
#define MAX_MACRO_CONTENT_LENGTH 486  /*as advised by the forum, the macro length can be a constant of 6 lines of 80 chars(and newlines), that can easily be changed*/
#define MAX_CODE_LINE 81
#define ON 1
#define OFF 0

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

struct macro{
		char name[MAX_MACRO_NAME_LENGTH];
		char content[MAX_MACRO_CONTENT_LENGTH];
		struct macro *nextMacro;
	};

char *getContent(char *,struct macro *);
int isMacro(char *,struct macro *);
void newMacro(char a [],char b[],struct macro **head);

/*This function recieves an ".as" file, and processes all the macros, saves their representation, and "spreads" them out in a new file which will be created.
	there is a max macro name length, which is defined, and a max macro content length, which is also defined
	all these definitions and max lengths are specified in the project, or forums */
/*the function saves macros by processing their information and saving it in a linked list, so there is no limit to the number of macros.
	we can then access the macro when it appears in the file, by getting it from the list*/

void openMacro(FILE *fptr, char *name){
	FILE *fptr2;
	struct macro *headOfMacros;
	int i;
	int macroFlag=OFF;
	char macroName[MAX_MACRO_NAME_LENGTH],macroContent[MAX_MACRO_CONTENT_LENGTH];
	char *contentCopy=malloc((MAX_MACRO_CONTENT_LENGTH)*sizeof(char));
	char *str=malloc((MAX_CODE_LINE)*sizeof(char));
	char *nameCopy=malloc(strlen(name)+4);
	headOfMacros=NULL;

	for(i=0;i<MAX_CODE_LINE;i++){   /*initialize all arrays and pointers*/
		str[i]='\0';
	}
	for(i=0;i<(MAX_MACRO_NAME_LENGTH+1);i++)
		macroName[i]='\0';
	memset(macroContent,'\0',(MAX_MACRO_CONTENT_LENGTH));
	memset(contentCopy,'\0',(MAX_MACRO_CONTENT_LENGTH));
	strcpy(nameCopy,name);
	strcat(nameCopy,".am");
	*(nameCopy+strlen(name)+4)='\0';  /*we append ".am" to the file, and store the spreaded macro text in this new file, which we create*/
	fptr2=fopen(nameCopy,"w");

	while(fgets(str,MAX_CODE_LINE,fptr)!=NULL){
		if(strstr(str,"macro")){
			macroFlag=ON;
			for(;(*str)!='o';str++)     /*skip over "macro" letters, to retrieve the macro's name*/
				;
			str+=2;   
			for(;isspace(*str);str++)
				;
			for(i=0;!isspace(*(str+i));i++) /*this function reads the name and places a null terminator at the end of the name*/
				;
				*(str+i)='\0';
			strcpy(macroName,str);
			continue;
		}
		else{
			if (macroFlag==ON){  /*if it's ON then we must continue reading the macro's content, which we save in macroContent*/
				while(!strstr(str,"endm")){
					strcat(macroContent,str);  /*we keep adding all the text of the macro to macroContent*/
					fgets(str,MAX_CODE_LINE,fptr);
				}
				macroFlag=OFF;
				newMacro(macroName,macroContent,&headOfMacros); /*here we create a new macro and add it to the list*/
				memset(macroName,'\0',MAX_MACRO_NAME_LENGTH+1);
				memset(macroContent,'\0',(MAX_MACRO_CONTENT_LENGTH));
				continue;
			}
			else{	/*this means no macro is being defined now, but we need to check if this line contains a macro to be processed*/
				for(i=0;isspace(*(str+i));i++)
					;		/*skip whitespace and read the text to see if its a macro*/
				if(isMacro(str+i,headOfMacros)){
					contentCopy=getContent(str+i,headOfMacros); /*if it is, we access the macro's content, and print it to the file*/
					fputs(contentCopy,fptr2);

				}
				else
					fputs(str,fptr2);
			}
		}
	}
	fclose(fptr2);
}

int isMacro(char *name, struct macro *head){  /*this function recieves a name, and returns if it is the name of a macro or not*/
	char copy[MAX_CODE_LINE];
	int i;
	struct macro *temp;
	strcpy(copy,name);  
	for(i=0;!isspace(*(copy+i));i++)
		;			/*we create a copy of the name, and manipulate it, so we can compare it to the names of macros */
	*(copy+i)='\0';
	for(temp=head;temp!=NULL;temp=temp->nextMacro){
		if(strcmp(copy,temp->name)==0)
			return 1;
	}
	return 0;
}

char * getContent(char *name,struct macro *head){  /*this function is called with a macro name, and returns its content*/
	char copy[MAX_CODE_LINE];
	int i;
	struct macro *temp;
	strcpy(copy,name);
	for(i=0;!isspace(*(copy+i));i++)
		;			/*we create a copy of the name, and manipulate it, so we can compare it to the names of macros */
	*(copy+i)='\0';
	for(temp=head;strcmp(copy,temp->name)!=0;temp=temp->nextMacro)
		;

	return (temp->content);
}

void newMacro(char a [],char b[],struct macro **head){  /*this creates a new macro and stores it in the macros list*/
	struct macro *newNode;
	newNode= ( struct macro*)malloc(sizeof(struct macro));
	
	strcpy(newNode->name,a);
	strcpy(newNode->content,b);  /*we initialize the macro with all its given data*/
	newNode->nextMacro=NULL;
	if(*head==NULL)
		*head=newNode;
	else{
		newNode->nextMacro=*head; /*the macro is now set as the head of the list, and can now be accessed at will*/
		*head=newNode;
	}
}


#include "temporaryStructHeader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int isIndex(char *x);
int isImmediate(char *x);
int isCommand(char *x);
int isRegister(char *x);
int alreadyDeclared(char *x,struct label*);
int isReservedWord(char *x);
int isInExtern(char *x, struct external *);
void newPotentialLabel(char *x,struct potentialLabel **);
int alreadyIsExternal(char *x, struct external *);

void newLabel(char *name, int location, char *attribute, struct label **listHead){	/*this function creates a new label and puts it at the list head*/
	struct label *newNode;
	newNode=(struct label*)malloc(sizeof(struct label));
	strcpy(newNode->name,name);
	strcpy(newNode->type,attribute);
	newNode->lineNum= location;
	newNode->next=NULL;
	if(*listHead==NULL)
		*listHead=newNode;
	else{
		newNode->next=*listHead;
		*listHead=newNode;
	}
}

void newEntry(char *n, struct entry** entriesHead){   /*this function creates a new entry and puts it at the entry list head*/
	struct entry *newEntry;
	newEntry=(struct entry*)malloc(sizeof(struct entry));
	strcpy(newEntry->name,n);
	newEntry->next=NULL;
	if(*entriesHead==NULL)
		*entriesHead=newEntry;
	else{
		newEntry->next=*entriesHead;
		*entriesHead=newEntry;
	}
}

struct external* getExterns(FILE *fptr,struct external **head){		/*this function return a pointer to a list of all externs deckared in the code*/
	char line[81];
	char *token1,*token2;
	struct external *newExternal;
	token1=malloc(32*sizeof(char));
	token2=malloc(32*sizeof(char));
	memset(line,'\0',81);
	memset(token1,'\0',32);
	while(fgets(line,81,fptr)!=NULL){
		token1=strtok(line," \f\n\r\t\v");
		if(token1==NULL)
			return *head;
		if(strcmp(token1+strlen(token1)-1,":")==0){  /*if theres a label, read the next token, and if its an extern then save it*/
			token2=strtok(NULL," \f\n\r\t\v");
			if(token2==NULL)
				return *head;
			if(strcmp(token2,".extern")==0){	/*if the token is an extern then save it, if not then continue (or exit at end of file)*/
			if(!alreadyIsExternal(token2,*head)){  /*if this extern has already been declared, dont create another one*/
				newExternal=(struct external*)malloc(sizeof(struct external));
				strcpy(newExternal->name,strtok(NULL," \f\n\r\t\v"));
				newExternal->next=NULL;
				if(*head==NULL)
					*head=newExternal;
				else{
					newExternal->next=*head;
					*head=newExternal;
				}
			}
			}
		}
		else{ /*no label*/
			if(strcmp(token1,".extern")==0){   /*if the token is an extern then save it, if not then continue (or exit at end of file)*/
			if(!alreadyIsExternal(token1,*head)){  /*if this extern has already been declared, dont create another one*/
				newExternal=(struct external*)malloc(sizeof(struct external));
				strcpy(newExternal->name,strtok(NULL," \f\n\r\t\v"));
				newExternal->next=NULL;
				if(*head==NULL)
					*head=newExternal;
				else{
					newExternal->next=*head;
					*head=newExternal;
				}
			}
			}
		}
	}
	return *head;
}

void newExternal(char *n, struct external** externsHead){	/*this function creates a new extern and puts it at the extern list head*/
	struct external *newExternal;
	newExternal=(struct external*)malloc(sizeof(struct external));
	strcpy(newExternal->name,n);
	newExternal->next=NULL;
	if(*externsHead==NULL)
		*externsHead=newExternal;
	else{
		newExternal->next=*externsHead;
		*externsHead=newExternal;
	}
}
	 /*this function returns whether or not the entry that was declared was already declared previously. If it was, it is ignored this time*/
int alreadyIsEntry(char *x, struct entry *head){ 
	struct entry *temp;
	if(x==NULL)
		return 0;
	for(temp=head;temp!=NULL;temp=temp->next){
		if(strcmp(x,temp->name)==0)
			return 1;
	}
	return 0;
}
	 /*this function returns whether or not the extern that was declared was already declared previously. If it was, it is ignored this time*/
int alreadyIsExternal(char *x, struct external *head){
	struct external *temp;
	if(x==NULL)
		return 0;
	for(temp=head;temp!=NULL;temp=temp->next){
		if(strcmp(x,temp->name)==0)
			return 1;
	}
	return 0;
}

	 /*this function returns whether or not the label that was declared was already declared previously. If it was, that is an error*/
int alreadyDeclared(char *x, struct label *headOfLabels){
	struct label *temp;
	if(x==NULL)
		return 0;
	for(temp=headOfLabels;temp!=NULL;temp=temp->next){
		if(strcmp(temp->name,x)==0)
			return 1;
	}
	return 0;
}
		/*this function checks if the passed label is a reserved word*/
int isReservedWord(char *x){
	if(x==NULL)
		return 0;
	if(isRegister(x) || isCommand(x))
		return 1;
	return 0;
}
/*this function recieves a label thats being declared, and checks if its a valid label*/
int labelValidity(char *x){
	int i;
	if(x==NULL)
		return 0;
	if(strlen(x)>31){  /*too long a label*/
		printf("Error in declared label name- you have entered a name which is too long\n");
		return 0;
	}
	if(!isalpha(*x) ){  /*first character of label not alphabetical*/
		printf("Error. You have entered an invalid label name. The labels first character must be alphabetical\n");
		return 0;
	}
	if(isReservedWord(x)){  /*a label cannot be a reserved word*/
		printf("Error in declared label name- you cannot use a reserved word as a label name\n");
		return 0;
	}
	for(i=0;i<strlen(x);i++){	/*all letters of the label must be alphanumerical*/
		if( !isalnum(*(x+i)) ){
			printf("Error. You have entered an invalid label name. Names must have only alphanumeric characters\n");
			return 0;
		}
	}
	return 1;
}
			/*this function recieves a label thats being used as an operand, and checks if its a valid label*/
int labelOperandValidity(char *l){
	int i;
	if(l==NULL)
		return 0;
	if(strlen(l)>31){			/*too long*/
		printf("Error. You have entered a label which is longer than the maximum length\n");
		return 0;
	}
	if(!isalpha(*l) ){		/*first character of label not alphabetical*/
		printf("Error. You have entered an invalid label name. The labels first character must be alphabetical\n");
		return 0;
	}
	if(isReservedWord(l)){			/*a label cannot be a reserved word*/
		printf("Error in label name- you cannot use a reserved word as a label name\n");
		return 0;
	}
	for(i=0;i<strlen(l);i++){
		if( !isalnum(*(l+i)) ){			/*all letters of the label must be alphanumerical*/
			printf("Error. You have entered an invalid label name. Names must have only alphanumeric characters\n");
			return 0;
		}
	}
	return 1;	
}
		/*this function gets a name, (operand), and returns whether or not it is the name of a label*/
int inLabels(char *p, struct label* listHead){
	int found;
	struct label *temp;
	if(p==NULL)
		return 0;
		found=0;
		for(temp=listHead;temp!=NULL && found==0;temp=temp->next){
			if(strcmp(p,temp->name)==0)
				found=1;
		}
			if(found)
				return 1;
		return 0;	
}
		/*this function gets a name, (operand), and returns whether or not it is the name of an external label*/
int isInExtern(char *x, struct external *h){
	struct external *temp;
	int found=0;
	if(x==NULL)
		return 0;
	for(temp=h;temp!=NULL && found==0;temp=temp->next){
		if(strcmp(x,temp->name)==0)
			found=1;
	}
	if(found==0)
		return 0;
	return 1;
}


		/*this function recieves the name of a label, and returns the labels address (line number it was declared in (updated for data))*/
int getValue(char *n, struct label *head){
	struct label *temp;
		for(temp=head;temp!=NULL && strcmp(temp->name,n)!=0 ;temp=temp->next)
			;
			return (temp->lineNum);
}
	/*returns the base of an address*/
int calculateBase(int x){
	if((x%16)==0)
		return (x-16);
	return (x-(x%16));
}
		/*returns the offset of an address*/
int calculateOffset(int x){
	return (x%16);
}
		/*returns whether or not this operand is a label*/
int isLabel(char *x){
	if(x==NULL)
		return 0;
	if(!isIndex(x) && !isRegister(x) && !isImmediate(x))
		return 1;
	return 0;
}
	/*in order to separate data and code, we add ICF+100 to each data label so they all come after the code and code labels */
void updateDataLabels(struct label *head, int offset){
	struct label *temp;
	for(temp=head;temp!=NULL;temp=temp->next){
		if(strcmp(temp->type,"data")==0)
			temp->lineNum += (offset+100);
	}
}
/*this function generates the ".ent" file that contains information about each entry (where its declared in the code,based on base and offset)*/
void createEntryFolder(char *fileName,struct entry *entriesHead,struct label *head){
	FILE *fp;
	struct entry *temp;
	int a,b,c;
	char *newEntryFile=malloc((strlen(fileName)+5)*sizeof(char));
	strcpy(newEntryFile,fileName);
	strcat(newEntryFile,".ent");
	*(newEntryFile+strlen(fileName)+5)= '\0';
	if(entriesHead==NULL)
		return;
	fp=fopen(newEntryFile,"w");
	for(temp=entriesHead;temp!=NULL;temp=temp->next){
		a=getValue(temp->name,head);
		b=calculateBase(a);
		c=calculateOffset(a);
		fprintf(fp,"%s,%04d,%04d\n",temp->name,b,c);
	}
}
/*this function generates the ".ext" file that contains information about each extern,where its used in the code,and where the address and offset are in the code*/
void createExternalFolder(char *fileName, struct external *externsHead, struct labelOperand *operands){
	FILE *fp;
	struct external *temp;
	struct labelOperand *op;
	char *newExternalFile=malloc((strlen(fileName)+5)*sizeof(char));
	strcpy(newExternalFile,fileName);
	strcat(newExternalFile,".ext");
	*(newExternalFile+strlen(fileName)+5)= '\0';
	if(externsHead==NULL)   /*if there are no externs then no such file is generated*/
		return;
	fp=fopen(newExternalFile,"w");
	for(temp=externsHead;temp!=NULL;temp=temp->next){
		for(op=operands;op!=NULL;op=op->next)
			if(strcmp(temp->name,op->name)==0)
				fprintf(fp,"%s BASE %04d\n%s OFFSET %04d\n\n",temp->name,op->base,temp->name,op->offset); /*printing format for the ext file*/
	}
}

/*this function generates a list of places where labels are used as operands, and this list is used to complete the ext file with addresses for each extern*/
void newLabelOperand(char *name, int bs, int offs, struct labelOperand **operands){
	struct labelOperand *newNode;
	newNode=(struct labelOperand*)malloc(sizeof(struct labelOperand));
	strcpy(newNode->name,name);
	newNode->base= bs;   /*stores the line where each labels address (base and offset) will be printed*/
	newNode->offset=offs;
	newNode->next=NULL;
	if(*operands==NULL)
		*operands=newNode;
	else{
		newNode->next=*operands;
		*operands=newNode;
	}
}


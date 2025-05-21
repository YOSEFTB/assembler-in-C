
int calculateBase(int x){
	if((x%16)==0)
		return (x-16);
	return (x-(x%16));
}

int calculateOffset(int x){
	return (x%16);
}

int isValidlabel(char token[]){
	if(alreadyDefined(token, *headOfLabelsList)){
		printf("Error in label- label has already been defined");
		return 0;
	}
	if(isReservedWord(token)){
		printf("Illegal name- label name cannot be a reserved word");
		return 0;
	}
	if(tooLong(token)){
		printf("Error- your label name is too long");
		return 0;
	}

	return 1;
}

int alreadyDefined(char a[], struct list* head){
	struct list* temp;
	for(temp=head;temp->nextLabel!=NULL;temp=temp->nextLabel){
		if(strcmp(temp->name,a)==0)
			return 1;
	}
	if(strcmp(temp->name,a)==0)
		return 1;
	return 0;
}

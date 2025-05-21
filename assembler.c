#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int validFile(char *);
void assemble(char *);
int fileExists(char *);
void openMacro(FILE*, char[]);
void assembleWithTwoPasses(FILE *fptr, char *);

int main(int argc, char* argv[]){

int i;
char *fileCopy;

	for(i=1;i<argc;i++){
		fileCopy= malloc(strlen(argv[i])*sizeof(char));
		strcpy(fileCopy,argv[i]);
		if(validFile(fileCopy)){
			printf("Found file %s \n",fileCopy);
			assemble(fileCopy);
		}
		else{
			printf("Cannot find file %s \n",argv[i]);
			continue;
		}
	}
	printf("There are no more files left to be read\n");
	return 0;
}


void assemble(char *x){
	FILE *fptr;
	FILE *fptr2;
	char *fileName= malloc(strlen(x)*sizeof(char)+4);
	char *fileForAssembly= malloc(strlen(x)+1);
	strcpy(fileName,x);
	strcat(fileName,".as");
	*(fileName+strlen(x)+4)='\0';
	fptr= fopen(fileName,"r");
	openMacro(fptr,x);
	strcpy(fileForAssembly,x);
	strcat(fileForAssembly,".am");
	*(fileForAssembly+strlen(x)+4)='\0';
	fptr2=fopen(fileForAssembly,"r");
	assembleWithTwoPasses(fptr2,x);
	fclose(fptr2);

}

int validFile(char x[]){
	char *c= malloc(strlen(x)+4);
	strcpy(c,x);
	strcat(c,".as");
	*(c+(strlen(x)+4))='\0';
		if(fileExists(c))
			return 1;
		else
			return 0;
}


int fileExists(char x[]){
	FILE *fptr= fopen(x,"r");
		if(fptr==NULL)
			return 0;
		else{
			fclose(fptr);
			return 1;
		}

}

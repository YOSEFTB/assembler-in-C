#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


int dataValidity(char *line){
	int i,k,commaFlag,signFlag,noNumFlag,noInputYet;
	noNumFlag=0,signFlag=0,commaFlag=0,noInputYet=1;
	for(;isspace(*line);line++)
		;
	line+=6;
	for(i=0;(*(line+i))!='\n' && (*(line+i))!='\0';i++){
		if(isspace(*(line+i)) && !noInputYet && !commaFlag){
			noNumFlag=1;
			continue;
		}
		if(isspace(*(line+i)) && noInputYet){
			continue;
		}
		if(isspace(*(line+i)) && !noInputYet && commaFlag){
			continue;
		}
		if( (*(line+i)) !='-' && (*(line+i)) !='+' && ( (*(line+i)-'0')<0 || (*(line+i)-'0')>9 ) && (*(line+i)) !=',' && !isspace(*(line+i)) ){
			printf("Error. You have entered data which is not a number or a valid operator");
			return 0;
		}
		if((*(line+i))==',' && commaFlag){
			printf("Error. You have entered too many commas in your data input\n");
			return 0;
		}
		if((*(line+i))==',' && !commaFlag && !noInputYet){
			commaFlag=1;
			noNumFlag=0;
			continue;
		}
		if((*(line+i))==',' && !commaFlag && noInputYet){
			printf("Error. Comma detected before any data was input, which is not valid\n");
			return 0;
		}
		if( ( (*(line+i)-'0')>=0 || (*(line+i)-'0')<=9 ) && !noNumFlag){
			commaFlag=0;
			noInputYet=0;
			continue;
		}

		if( ( (*(line+i)-'0')>=0 || (*(line+i)-'0')<=9 ) && noNumFlag){
			printf("Error. Numbers must be separated by a comma\n");
			return 0;
		}
		if(( (*(line+i))=='-' || (*(line+i))=='-') && signFlag){
			printf("Error. A value sign must only follow a number, not another value sign");
			return 0;
		}
		if(( (*(line+i))=='-' || (*(line+i))=='-') && noNumFlag){
			printf("Error. Numbers must be separated by a comma (Also numbers with a preceding value sign)");
			return 0;
		}
	}
	for(k=0;isspace(*(line+i-k));k++)
		;
		if((*(line+i-k))==','){
			printf("Error. Extraneous comma detected at end of data input");
			return 0;
		}
	return 1;
}

int stringValidity(char *line){
	int i,inString=0;
	for(;isspace(*line) && (*line)!='\0' ;line++)
		;
	line+=7;
	for(;isspace(*line) && (*line)!='\0' ;line++)
		;
	if(!isspace(*line) && (*line)!='\"'){
		printf("Error. You can only enter characters in between the string delimiters (quotation marks)\nHere, in letter -%c-",*line);
		return 0;	
	}
	if((*line)=='\0'){
		printf("Error. You must enter characters after declaring the line to be a string instruction\n");
		return 0;
	}
	for(i=0; ((*(line+i))!='\n') && (*(line+i))!='\0' ;i++){
		if(!isprint(*(line+i))){
			printf("Error. You must only enter printable characters");
			return 0;
		}
		if( (*(line+i))=='\"' && inString && (*(line+i-1))!='\\' ){
			inString=0;
			continue;
		}
		if((*(line+i))=='\"' && !inString){
			inString=1;
			continue;
		}
		if(isspace(*(line+i)))
			continue;
		if(isprint(*(line+i)) && inString)
			continue;
		if(isprint(*(line+i)) && !inString){
			printf("Error. You can only enter characters in between the string delimiters (quotation marks)\n");
			return 0;
		}
		
	}
	if(inString){
		printf("Error. Strings must begin and end with quotation marks\n");
		return 0;
	}
	return 1;
}


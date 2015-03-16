#include <stdio.h>
#include <stdlib.h>

int main(){
	FILE *fp;
	fp = fopen("data1.txt","w");
	if(fp == NULL) printf("fp is null");
	else printf("fp is not null");
	fprintf(fp,"fprintf test from %s\n","scott");
	fputs("fputs test\n",fp);
	fclose;
	
}
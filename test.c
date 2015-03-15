#include <stdio.h>
#include <stdlib.h>

int main(){
	char a[] = "PORT 192,168,1,5,32,155";
	int b[6];
	sscanf(a,"PORT %d,%d,%d,%d,%d,%d",&b[0],&b[1],&b[2],&b[3],&b[4],&b[5]);
	printf("%d,%d,%d,%d",b[0],b[1],b[2],b[3]);
	
}
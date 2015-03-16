#include <stdio.h>
#include <stdlib.h>

int main(){
	char fake[] = "PORT 192,168,1,5,10,177";
	//int f[4];
	int f[6];
	unsigned char fc[2];
	char tmp[40];
	//sscanf(fake,"PORT %d,%d,%d,%d,%d,%d",&f[0],&f[1],&f[2],&f[3],(int*)&fc[0],(int*)&fc[1]);
	sscanf(fake,"PORT %d,%d,%d,%d,%d,%d",&f[0],&f[1],&f[2],&f[3],&f[4],&f[5]);
	printf("watch:\n");
	printf("aaa %d.%d.%d.%d",f[0],f[1],f[2],f[3]);
	
}
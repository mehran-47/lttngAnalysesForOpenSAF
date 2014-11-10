#include <stdio.h>
#include <unistd.h>
#include <lttng/tracef.h>

int main(){
	int iternum=100;
	for(int i=0; i<iternum;i++){
		//printf("I'm a number, %d\n", i);
		tracef("Dummy trace number %d",i);
		usleep(1500000);
	}
	return 0;
}
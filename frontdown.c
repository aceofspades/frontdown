#include <stdio.h>
#include <stdlib.h>

#include "frontdown.h"

int main(void){
	printf("Frontdown %s\n", FD_VERSION);
	#ifdef __GNUC__
		printf("Compiled: %s %s with gcc %d.%d.%d\n\n", __DATE__, __TIME__, __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
	#endif
	
	return 0;
}

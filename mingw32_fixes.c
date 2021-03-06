#ifdef _WIN32_
#include "frontdown_internal.h"


char *getpass(char *out){
	int i=0;
	char *ret;
	
	printf("%s",out);
	
	ret=malloc(1024);

	do
	{
		ret[i] = _getch();
		i++;
		if(i==1024)break;
	} while( (ret[i-1] != '\n') && (ret[i-1] != '\r') );
	ret[i-1]='\0';
	
	printf("\n");

	return ret;
}

char *get_current_dir_name(){
	char *dir;
	int i;
	
	if( (dir = _getcwd( NULL, 0 )) == NULL )
		perror( "_getcwd error" );
	
	while(dir[i]!='\0'){
		if(dir[i]=='\\') dir[i]=0x2f;
		i++;
	}
	
	return dir;
	
}


#endif

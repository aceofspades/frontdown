#ifdef _WIN32_
#include "mingw32_fixes.h"


char *getpass(){
	int i;
	char *ret;
	
	ret=malloc(1024);

	do
	{
		ret[i] = _getch();
		i++;
		if(i==1024)break;
	} while( ret[i-1] != '\n' );
	ret[i-1]='0';

	return ret;
}

char *get_current_dir_name(){
	char *dir;
	
   if( (dir = _getcwd( NULL, 0 )) == NULL )
      perror( "_getcwd error" );

	return dir;
	
}


#endif

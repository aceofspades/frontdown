#ifndef __FRONTDOWN_H__
#define __FRONTDOWN_H__

#define FD_VERSION "0.1"
#define FD_PATHLEN 65536

struct dirnode{
	char path[FD_PATHLEN];	
	struct dirnode *next;
};

struct dirnode *root;

#endif
	

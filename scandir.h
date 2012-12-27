#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <libgen.h>

#include "frontdown.h"

#ifndef __SCANDIR_H__
#define __SCANDIR_H__

struct dirnode{
	char path[FD_PATHLEN];	
	struct dirnode *sub;
	struct dirnode *top;
	struct dirnode *next;
};

int fd_scandir(const char* path);
struct dirnode *anakin_filewalker(struct dirnode *luke, struct dirnode *leia);

#endif

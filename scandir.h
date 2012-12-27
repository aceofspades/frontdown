#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <libgen.h>

#include "frontdown.h"
#include "fd_curl.h"

#ifndef __SCANDIR_H__
#define __SCANDIR_H__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

struct dirnode{
	char path[FD_PATHLEN];	
	struct dirnode *sub;
	struct dirnode *top;
	struct dirnode *next;
};

int fd_scandir(const char* path, long long timestamp, struct bucket *buck, size_t n_bucks);
struct dirnode *anakin_filewalker(struct dirnode *luke, struct dirnode *leia, char *pwd, long long time);

#endif

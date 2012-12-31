#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <libgen.h>
#include <regex.h>

#include "frontdown.h"
#include "communication.h"
#include "fd_curl.h"

#ifndef __SCANDIR_H__
#define __SCANDIR_H__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

struct dirnode{
	char path[FD_NAMELEN];	
	struct dirnode *sub;
	struct dirnode *top;
	struct dirnode *next;
};

extern int fd_scandir(const char* path, long long timestamp, struct exclude_list *excludes);
extern struct dirnode *anakin_filewalker(struct dirnode *luke, struct dirnode *leia, const char *source, char *cpath, long long time, struct exclude_list *excludes);
extern int upload(const char *source, char *relpath, char *name, struct stat filestat);
extern int filter(char *path, char *name, long long timestamp, long long time, struct exclude_list *excludes);

#endif

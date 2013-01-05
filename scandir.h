#ifndef __SCANDIR_H__
#define __SCANDIR_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <libgen.h>
#include <regex.h>

#include "frontdown_internal.h"

struct dirnode{
	char path[FD_NAMELEN];	
	struct dirnode *sub;
	struct dirnode *top;
	struct dirnode *next;
};

extern int fd_scandir(struct frontdown_config *conf,long long timestamp);
extern int anakin_filewalker(struct frontdown_config *config, struct dirnode *luke, struct dirnode *leia, const char *source, char *cpath, long long time, struct frontdown_exclude_list *excludes);
extern int upload(struct frontdown_config *config, const char *source, char *relpath, char *name, struct stat filestat);
extern int filter(char *path, char *name, long long timestamp, long long time, struct frontdown_exclude_list *excludes);

#endif

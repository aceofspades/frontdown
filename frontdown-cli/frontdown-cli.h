#include <stdio.h>
#include <stdlib.h>

#ifndef __FRONTDOWN_CLI_H__
#define __FRONTDOWN_CLI_H__

#include "../frontdown.h"


struct frontdown_config config;
struct frontdown_exclude_list *latest_exclude;
char conf[FD_PATHLEN];
char destinationLogin;

extern int parse_options(int argc, char **argv);
extern void help(void);

#include "parser.h"

#endif
	

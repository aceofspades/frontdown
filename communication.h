#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "frontdown_internal.h"

extern int close_destination();
extern int open_destination(struct frontdown_config *config);
extern int get_indexfile(struct frontdown_config *config, char *source);
extern int put_file(struct frontdown_config *config, char *source, char *filename, char *target, curl_off_t size);
extern int create_dest_dir(struct frontdown_config *config, char *relpath);

#endif

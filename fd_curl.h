#include <pthread.h>

#ifndef __FD_CURL_H__
#define __FD_CURL_H__

struct path{
	char pathpart[1024];
	struct path *path_next_part;
};

struct bucket{
	pthread_mutex_t lock;
	struct path *pathparts;
	char used;
};

#endif

#ifndef __FRONTDOWN_H__
#define __FRONTDOWN_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32_
	#include "mingw32_fixes.h"
#endif

#define FD_VERSION "0.1"
#define FD_PATHLEN 16384
#define FD_NAMELEN 512

struct frontdown_exclude_list{
	char exclude_path[FD_PATHLEN];
	struct frontdown_exclude_list *next;
};

enum frontdown_connection_type {
	_DICT_, _FILE_, _FTP_, _HTTP_, _IMAP_, _LDAP_,
	_POP3_, _RTMP_, _RTSP_, _SCP_, _SFTP_, _SMTP_, _TELNET_,
	_TFTP_, _UNKNOWN_
};

enum frontdown_error_code{
	_INDEX_DB_ERROR_, _MKDIR_ERROR_, _DST_CONNECTION_ERROR_, _PUT_FILE_ERROR_
};

struct frontdown_config{
	// The maximal source length is FD_PATHLEN
	char *source;

	// The maximal destination length is FD_PATHLEN
	char *destination;

	// The maximal destinationUname length is FD_NAMELEN
	char *destinationUname;

	// The maximal destinationPwd length is FD_NAMELEN
	char *destinationPwd;
	
	enum frontdown_connection_type con_type;
	
	// The last element musst allways have a
	// NULL pointer as next element;
	struct frontdown_exclude_list *excludes;
	
	// Function to printout informations
	// e.g. cwd or exclude hints
	int (*info)(char *);
	
	// Function for error handling
	// arg 1 contains error code as above
	// if arg 2 is !=0 it's a fatal error
	// arg 3 contains the path to the failing file or directory
	// it's always the path to the source
	// to ignore nonfatal error's return 0
	// return !=0 to abort
	// a fatal error will always abort
	int (*error)(enum frontdown_error_code, char, char*);
	
	// If last_backup is >0 the timestamp will be loaded from 
	// destinations index.db
	char last_backup;
	
	//Unix time
	long long now;
};

extern int frontdown(struct frontdown_config *config);

#endif
	

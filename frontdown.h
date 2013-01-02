#include <stdio.h>
#include <stdlib.h>

#ifndef __FRONTDOWN_H__
#define __FRONTDOWN_H__

#ifdef _WIN32_
	#include "mingw32_fixes.h"
#endif

#ifdef _GUI_
	#include "gtkui.h"
#endif

#define FD_VERSION "0.1"
#define FD_PATHLEN 65536
#define FD_NAMELEN 512
#define FD_BUCKETSIZE 256

struct exclude_list{
	char exclude_path[16384];
	struct exclude_list *next;
};
struct exclude_list *latest_exclude;

enum connection_type {
	_DICT_, _FILE_, _FTP_, _HTTP_, _IMAP_, _LDAP_,
	_POP3_, _RTMP_, _RTSP_, _SCP_, _SFTP_, _SMTP_, _TELNET_,
	_TFTP_, _UNKNOWN_
};


struct{
	char source[16384];
	
	char destination[FD_PATHLEN];
	char destinationLogin;
	char *destinationUname;
	char *destinationPwd;
	
	unsigned int threads;
	
	enum connection_type con;
	
	struct exclude_list *excludes;
	
	char conf[16384];
	long long last_backup;
	long long now;
	
	FILE *index_db;
	
} config;


extern int parse_options(int argc, char **argv);
extern void help(void);

#include "communication.h"
#include "parser.h"
#include "scandir.h"

#endif
	

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

#include "frontdown.h"
#include "scandir.h"
#include "fd_curl.h"
#include "communication.h"

void version(void){
	printf("\nFrontdown %s\n", FD_VERSION);
	#ifdef __GNUC__
		printf("Compiled: %s %s with gcc %d.%d.%d\n\n", __DATE__, __TIME__, __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
	#endif
}

void usage(void){
	printf("Usage: frontdown [OPTIONS] \n\n");
	printf("\t-c --conf          Configuration file\n");
	printf("\t-d --destination   Backup destination\n");
	printf("\t-e --exclude       File/Folder to exclude as POSIX Extended Regular Expressions\n");
	printf("\t-h --help          Print this help\n");
	printf("\t-l --login         Destination requires login\n");
	printf("\t-s --source        Backup source\n");
	
	printf("\n");
	printf("There are no bugs - just random features.\n");
	printf("Mail them to: <nosupport@nowhere.nix>\n\n");
	printf("(C) Copyright 2012 by Patrick Eigensatz & Florian Wernli\n\n");
}


void help(){
	version();
	usage();
}

int main(int argc, char **argv){
	// Initialize exclude lists
	config.excludes = calloc(1, sizeof(struct exclude_list));
	config.con=-1;
	config.now=(long long)time(NULL);
	
	latest_exclude = config.excludes;
	
	// Parse command line options
	parse_options(argc, argv);
	
	// Display parsed options
	printf("================================ CONFIG ================================\n");
	printf("Configuration File:     %s\n", strlen(config.conf)==0?"None":config.conf);
	printf("Source:                 %s\n", config.source);
	printf("Destination:            %s\n", config.destination);
	printf("Last backup:            %s\n", config.last_backup==0?"Never before":ctime((time_t*)&config.last_backup));
	printf("========================================================================\n\n");
	
	
	// Get index.db
	char indexpath[16384];
	char fixbuf[64];
	char *buf;
	strcpy(indexpath, config.destination);
	strcat(indexpath, "/index.db");

	if(config.destinationLogin){
		printf("To provide maximal security we won't display any character entered!\n\n");

		config.destinationUname=getpass("Destination User: ");
		buf=malloc(strlen(config.destinationUname));
		strcpy(buf, config.destinationUname);
		config.destinationUname=buf;

		config.destinationPwd=getpass("Destination Password: ");			
		buf=malloc(strlen(config.destinationPwd));
		strcpy(buf, config.destinationPwd);
		config.destinationPwd=buf;

		printf("\n\n");
	}
	
	if(config.last_backup != 0)
		get_indexfile(indexpath);
	
	open_destination(config.destination);
	create_dest_dir("/");

	sprintf(fixbuf,"%0*lld/", 15,config.now);
	strcat(config.destination, "/BACKUP");
	strcat(config.destination, fixbuf);

	fd_scandir(config.source, config.last_backup, config.excludes);
	close_destination();
	
	return 0;
}

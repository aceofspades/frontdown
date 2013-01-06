#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

#include "frontdown-cli.h"

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
	printf("(C) Copyright 2012-2013 by Patrick Eigensatz & Florian Wernli\n\n");
}


void help(){
	version();
	usage();
}

int info(char *text){
	printf("INFO: %s\n",text);
}

int error(enum frontdown_error_code error_code, char fatal, char *text){
	printf("ERROR: %s\n",text);
	if(!fatal){
		printf("Ignore nonfatal error\n");
		return 0;
	}
	return -1;
}

int main(int argc, char **argv){
	// Initialize exclude lists
	if((config.excludes = calloc(1, sizeof(struct frontdown_exclude_list)))==NULL){
		fprintf(stderr,"Make sure you have enough memory and run it again");
		exit(1);
	}

	config.now=(long long)time(NULL);
	latest_exclude = config.excludes;
	
	config.source=malloc(FD_PATHLEN);
	config.destination=malloc(FD_PATHLEN);
	
	config.destinationUname=NULL;
	config.destinationPwd=NULL;
	
	config.info=&info;
	config.error=&error;
	
	config.source[0]=0;
	config.destination[0]=0;
	config.last_backup=0;

	// Parse command line options (+ file config if specified)
	if(parse_options(argc, argv)<0){
		fprintf(stderr,"\nRun '%s --help' for help\n", argv[0]);
		exit(64);
	}	
	
	char urlbuffer[FD_PATHLEN];
	
	// Check for URL without scheme (only for destination, at the moment)
	if(strstr(config.destination, "file://") == config.destination) config.con_type=_FILE_;
	else if(strstr(config.destination, "ftp://") == config.destination) config.con_type=_FTP_;
	else if(strstr(config.destination, "sftp://") == config.destination) config.con_type=_SFTP_;
	else if(strstr(config.destination, "ftps://") == config.destination) config.con_type=_FTP_;
	else {
		config.con_type=_FILE_;
		strcpy(urlbuffer, "file://");
		strncat(urlbuffer, config.destination, FD_PATHLEN-8);
		strcpy(config.destination, urlbuffer);
	}
	  
	
	// Display parsed options
	printf("================================ CONFIG ================================\n");
	printf("Configuration File:     %s\n", strlen(conf)==0?"None":conf);
	printf("Source:                 %s\n", config.source);
	printf("Destination:            %s\n", config.destination);
	printf("Backuped before:        %s\n", config.last_backup!=0?"yes":"no");
	printf("========================================================================\n\n");
	
	
	char *buf;
	
	if(destinationLogin){
		config.destinationUname=getpass("User: ");
		buf=malloc(strlen(config.destinationUname));
		strcpy(buf, config.destinationUname);
		config.destinationUname=buf;

		config.destinationPwd=getpass("Password: ");			
		buf=malloc(strlen(config.destinationPwd));
		strcpy(buf, config.destinationPwd);
		config.destinationPwd=buf;

		printf("\n\n");
	}
	
	frontdown(&config);
	
	return 0;
}

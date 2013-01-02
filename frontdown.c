#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

#include "frontdown.h"
#include "scandir.h"
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
	
	#ifdef _GUI_
	printf("\t-u                 Use GTK UI\n");
	#endif

	printf("\n");
	printf("There are no bugs - just random features.\n");
	printf("Mail them to: <nosupport@nowhere.nix>\n\n");
	printf("(C) Copyright 2012-2013 by Patrick Eigensatz & Florian Wernli\n\n");
}


void help(){
	version();
	usage();
}

int main(int argc, char **argv){
	// Initialize exclude lists
	if((config.excludes = calloc(1, sizeof(struct exclude_list)))==NULL){
		fprintf(stderr,"Make sure you have enough memory and run it again");
		exit(1);
	}

	config.con=-1;
	config.now=(long long)time(NULL);
	latest_exclude = config.excludes;
	
	config.source[0]=0;
	config.destination[0]=0;

	// Parse command line options (+ file config if specified)
	if(parse_options(argc, argv)<0){
		fprintf(stderr,"\nRun '%s --help' for help", argv[0]);
		exit(64);
	}	
	
	char urlbuffer[16384];
	
	// Check for URL without scheme (only for destination, at the moment)
	if(strstr(config.destination, "file://") != config.destination
	   && strstr(config.destination, "ftp://") != config.destination
	   && strstr(config.destination, "sftp://") != config.destination
	   && strstr(config.destination, "ftps://") != config.destination){
			strcpy(urlbuffer, "file://");
			strncat(urlbuffer, config.destination, 16384-8);
			strcpy(config.destination, urlbuffer);
		}
	  
	
	// Display parsed options
	printf("================================ CONFIG ================================\n");
	printf("Configuration File:     %s\n", strlen(config.conf)==0?"None":config.conf);
	printf("Source:                 %s\n", config.source);
	printf("Destination:            %s\n", config.destination);
	printf("Backuped before:        %s\n", config.last_backup!=0?"yes":"no");
	printf("========================================================================\n\n");
	
	
	// Get index.db
	char indexpath[16384];
	char fixbuf[64];
	char *buf;
	struct stat statbuf;
	
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
	
	open_destination(config.destination);

	remove("./index.db");
	if(config.last_backup > 0){
		if(get_indexfile(indexpath)<0){
			fprintf(stderr, "Do you whish to continue anyway? [y,n]");
			char cont=0;
			while(((cont=getc(stdin))!='y')&&(cont!='n'))
				fprintf(stderr, "Do you whish to continue anyway? [y,n]");
			
			if(cont=='n')exit(74);
		}
		
		//Read last backup time
		if((config.index_db=fopen("./index.db", "rb"))){
			fscanf(config.index_db, "%lld", &config.last_backup);
			fclose(config.index_db);
		}
	}


	//Rewrite index.db
	config.index_db=fopen("./index.db", "wb+");
	rewind(config.index_db);
	fprintf(config.index_db, "%0*lld\n", 15, config.now);
	
	create_dest_dir("/");

	sprintf(fixbuf,"%0*lld/", 15, config.now);
	strcat(config.destination, "/BACKUP");
	strcat(config.destination, fixbuf);

	fd_scandir(config.source, config.last_backup, config.excludes);

	fclose(config.index_db);

	if(config.last_backup != 0){
		get_indexfile(indexpath);
	}

	stat("./index.db", &statbuf);
	put_file("./index.db", "index.db", indexpath, statbuf.st_size); 

	close_destination();
	
	return 0;
}

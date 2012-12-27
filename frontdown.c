#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

#include "frontdown.h"

void version(void){
	printf("\nFrontdown %s\n", FD_VERSION);
	#ifdef __GNUC__
		printf("Compiled: %s %s with gcc %d.%d.%d\n\n", __DATE__, __TIME__, __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
	#endif	
}

void usage(void){
	printf("Usage: frontdown [OPTIONS] \n\n");
	printf("\t-h --help          Prints this help\n");
	printf("\t-s --source        Backup Source\n");
	printf("\t-d --destination   Backup Destination\n");
	printf("\t-H --hidden        Include files starting with .\n");
	printf("\t-c --conf          Configuration file\n");
	
	printf("\n");
	printf("There are no bugs - just random features.\n");
	printf("Mail them to: <nosupport@nowhere.nix>\n\n");
	printf("(C) Copyright 2012 by Patrick Eigensatz & Florian Wernli\n\n");
}

// Global file counter
int n=0;

struct dirnode *listdir(struct dirnode *origin, struct dirnode *node){		
	struct dirent *pwd_ent;
	struct stat buf;
	DIR* pwd;
	
	char path[FD_PATHLEN];

	if((pwd=opendir(origin->path))==NULL){
		perror("opendir");
		return NULL;
	}
	
	while((pwd_ent=readdir(pwd))){
		
		if((pwd_ent->d_name[0]=='.') && ( \
			(pwd_ent->d_name[1]=='\0') || \
			((pwd_ent->d_name[1]=='.')&&(pwd_ent->d_name[2]=='\0'))));
		else{

			strcpy(path,origin->path);
			strcat(path,"/");
			strcat(path, pwd_ent->d_name);

			if(stat(path, &buf)<0){
				printf("FAILED AT: %s \n\t", path);
				perror("stat");
			}

			printf("> %s/%s:\n", origin->path, pwd_ent->d_name);
			puts("\t------------------------------------");

			printf("\t|Dev:\t%i\n", (int)buf.st_dev);
			printf("\t|Inode:\t%i\n", (int)buf.st_ino);
			printf("\t|Mode:\t%o\n", (int)buf.st_mode);
			printf("\t|Nlink:\t%i\n", (int)buf.st_nlink);
			printf("\t|UID:\t%i\n", (int)buf.st_uid);
			printf("\t|GID:\t%i\n", (int)buf.st_gid);
			printf("\t|Size:\t%ld\n", (long)buf.st_size);
			printf("\t|atime:\t%s", ctime(&buf.st_atime));
			printf("\t|mtime:\t%s", ctime(&buf.st_mtime));
			printf("\t|ctime:\t%s", ctime(&buf.st_ctime));

			puts("\t------------------------------------");
		
			if(S_ISDIR(buf.st_mode)){
				printf("--DEBUG--\nFound dir: %s\n", pwd_ent->d_name);
				node->next=calloc(1,sizeof(struct dirnode));
				node=node->next;
				strcpy(node->path, path);
			}
			n += 1;
		}
	}

	closedir(pwd);
	
	return node;
}

void help(){
	version();
	usage();
}

int main(int argc, char **argv){
	// Parse command line options
	parse_options(argc, argv);
	
	// Display parsed options
	printf("===================== CONFIG =====================\n");
	printf("Source:                 %s\n", config.source);
	printf("Destination:            %s\n", config.destination);
	printf("Threads:                %d\n", config.threads);
	printf("Include hidden Files:   %s\t\n", config.hidden==0?"no":"yes");
	

	struct dirnode *node;
	void *freewilli;

	root=calloc(1, sizeof(struct dirnode));	
	strcpy(root->path, config.source);
	node=root;

	do{
		printf("--DEBUG--\n %s --- %p\n", root->path, root->next);
		node=listdir(root, node);

		freewilli=root;
		root=root->next;
		free(freewilli);
	}while((node!=NULL)&&(root!=NULL));
	
	printf("Total %d files\n", n);
	
	return 0;
}

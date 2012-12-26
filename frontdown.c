#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

#include "frontdown.h"

void version(){
	printf("\nFrontdown %s\n", FD_VERSION);
	#ifdef __GNUC__
		printf("Compiled: %s %s with gcc %d.%d.%d\n\n", __DATE__, __TIME__, __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
	#endif	
}

void usage(char *prog){
	printf("Usage: %s [OPTIONS] [JOB]\n\n", prog);
	printf("\t-h\t--help\tprints this help\n");
	printf("\n");
	printf("There are no bugs - just random features.\n");
	printf("Mail them to: <nosupport@nowhere.nix>\n\n");
	printf("(C) Copyright 2012 by Patrick Eigensatz & Florian Wernli\n\n");
}

// Global file counter
int n=0;

int listdir(struct dirnode *node){		
	struct dirent *pwd_ent;
	struct stat buf;
	DIR* pwd;

	char path[FD_PATHLEN];
	int pathlen;

	strcpy(path,node->path);

	if((pwd=opendir(node->path))==NULL){
		perror("opendir");
		return -1;
	}
	
	while((pwd_ent=readdir(pwd))){
		strcat(node->path, pwd_ent->d_name);

		if((path[2]=='.') && ( \
			(path[3]=='\0') || ((path[3]=='.')&&(path[4]=='\0'))));
		else{

			stat(path, &buf);

			printf("> %s:\n", path);
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
				node->next=calloc(1,sizeof(struct dirnode));
				node=node->next;
				strcpy(node->path,path);
			}
			
			n += 1;
		}

		memset(path+2, 0, FD_PATHLEN-2);
	}

	closedir(pwd);
	
	return 0;
}

void help(int argc, char** argv){
	version();
	usage(argv[0]);
}

int main(int argc, char **argv){
	struct dirnode *node;
	void *freewilli;
	if(argc < 2)
		help(argc, argv);
	
	root=calloc(1,sizeof(struct dirnode));	
	strcpy(root->path,"./");

	node=root;
	do{
		chdir(node->path);
		listdir(node);
		printf("--DEBUG--\n %s --- %p\n", node->path, node->next);
		freewilli=node;
		node=node->next;
		free(freewilli);
	}while(node!=NULL);
	
	printf("Total %d files\n", n);
	
	return 0;
}

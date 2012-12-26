#include <stdio.h>
#include <stdlib.h>
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


int listdir(int level){
	char path[1024]={0};
	int i;

	struct dirent *pwd_ent;
	struct stat buf;
	DIR* pwd;
	
	path[0]='.';
	path[1]='/';

	if((pwd=opendir(path))==NULL){
		perror("opendir");
		return -1;
	}
	
	while((pwd_ent=readdir(pwd))){
		strcpy(&path[2], pwd_ent->d_name);

		if((path[2]=='.') && ( \
			(path[3]=='\0') || ((path[3]=='.')&&(path[4]=='\0'))));
		else{
			for(i=0;i<level;i++)putc('-',stdout);
			printf("> %s:\n", pwd_ent->d_name);
			puts("------------------------------------");
			stat(path, &buf);

			printf("\t|Dev:\t%i\n", (int)buf.st_dev);
			printf("\t|Inode:\t%i\n", (int)buf.st_ino);
			printf("\t|Mode:\t%o\n", (int)buf.st_mode);
			printf("\t|Nlink:\t%i\n", (int)buf.st_nlink);
			printf("\t|UID:\t%i\n", (int)buf.st_uid);
			printf("\t|GID:\t%i\n", (int)buf.st_gid);
			printf("\t|Size:\t%ld\n", (long)buf.st_size);
			printf("\t|atime:\t%s", ctime(&buf.st_atime));
			printf("\t|mtime:\t%s", ctime(&buf.st_mtime));
			printf("\t|ctime:\t%s\n", ctime(&buf.st_ctime));

			puts("------------------------------------");
		
			if(S_ISDIR(buf.st_mode)){
				chdir(path);
				listdir(level+1);
				chdir("..");
			}
		}
		
		for(i=2;i<1024;i++) path[i]=0;
	}

	closedir(pwd);
	
	return 0;
}

void help(int argc, char** argv){
	version();
	usage(argv[0]);
}

int main(int argc, char **argv){
	if(argc < 2)
		help(argc, argv);
	
	listdir(0);
	
	return 0;
}

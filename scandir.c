#include "scandir.h"

int FilesFound=0;
int FilesFailed=0;
int FilesUpdated=0;


void upload(const char *source, char *relpath, char *name, struct stat filestat){
	char srcpathstring[FD_PATHLEN]={0};
	char dstpathstring[FD_PATHLEN]={0};
	int len;
	
	strcpy(srcpathstring, source);
	len=strlen(source);
	srcpathstring[len]='/';
	strcpy(&srcpathstring[len+1], relpath);
	len+=1+strlen(relpath);
	srcpathstring[len]='/';
	strcpy(&srcpathstring[len+1], name);

	strcpy(dstpathstring, config.destination);
	len=strlen(config.destination);
	dstpathstring[len]='/';
	strcpy(&dstpathstring[len+1], relpath);
	len+=1+strlen(relpath);
	dstpathstring[len]='/';
	strcpy(&dstpathstring[len+1], name);

	put_file(srcpathstring, name, dstpathstring, filestat.st_size);
	FilesUpdated += 1;
}


int fd_scandir(const char* path, long long timestamp, struct exclude_list *excludes){
	char pathstring[FD_PATHLEN]={0};
	char *sourcepath;
	char *dirpointer[128];
	int dirptrc=0;
	struct dirnode *node, *root, *freewilli;
	
	char *orig_dir=get_current_dir_name();
	printf("%s", orig_dir);

	if(chdir(path)<0){
		perror("chdir");
		return -1;
	}

	sourcepath=get_current_dir_name();

	root=calloc(1,sizeof(struct dirnode));	
	strcpy(root->path, ".");
	pathstring[0]='.';
	dirpointer[dirptrc]=&pathstring[1];

	create_dest_dir(pathstring);
	
	node=root;

	do{
		printf("%s\n", pathstring);

		if(anakin_filewalker(node, node->sub, sourcepath, pathstring, timestamp, excludes)==NULL){ //No more subdirs

			next_dir:

				freewilli=node;
				node=node->next; //Try next dir in folder
				
				if(freewilli->top==NULL) break; //Make sure we're not in root
				freewilli->top->sub=node; //Attach new folder as primary subdir of its parent

				if(node==NULL){	//if no directory left in this stage
					node=freewilli->top; //move one stage up
					free(freewilli);	//free old stage
					if(node->top==NULL)break; //validate stage
					if(chdir("..")<0){		//change directory
						perror("chdir");
						return -1;
					}
					dirptrc--;
					*dirpointer[dirptrc]='\0';	//edit dir string
					
			goto next_dir;	//last subdir of current dir was handled, so we need to get the next
				
				}
			free(freewilli); //free old dir

			if(chdir("..")<0){
				perror("chdir");
				return -1;
			}
			dirptrc--;
			*dirpointer[dirptrc]='\0';

		}else{ //More subdirs
			node=node->sub; //goto subdir
		}

		if(chdir(node->path)<0){ //change to new dir
			perror("chdir");
			puts(node->path);
			return -1;
		}
		
		//update pathstring
		dirptrc++;
		*dirpointer[dirptrc-1]='/';
		memcpy(dirpointer[dirptrc-1]+1, node->path, strlen(node->path));
		dirpointer[dirptrc]=dirpointer[dirptrc-1]+1+strlen(node->path);
		*dirpointer[dirptrc]='\0';
		
		create_dest_dir(pathstring);

	}while(node->top!=NULL);
	
	printf("Total %d folders/files\n\
	Updated %d files\n\
	Failed on %d files\n", FilesFound, FilesUpdated, FilesFailed);

	chdir(orig_dir);
	return 0; 
}


int filter(char *path, char *name, long long timestamp, long long time, struct exclude_list *excludes){
	if(time>=timestamp)return -1;

	int status;
	char pathstring[FD_PATHLEN]={0};
	struct exclude_list *excl_walker;
	regex_t re;
	
	excl_walker=excludes;

	strcpy(pathstring, path);
	status=strlen(path);
	pathstring[status]='/';
	strcpy(&pathstring[status+1], name);

	while(excl_walker!=NULL&&excl_walker->exclude_path[0]!=0){
		if (regcomp(&re, excl_walker->exclude_path, REG_EXTENDED|REG_NOSUB) != 0) {
			excl_walker=excl_walker->next;
			continue;
		}
		status = regexec(&re, &pathstring[2], (size_t) 0, NULL, 0);
		regfree(&re);
		if (status != 0) {
			excl_walker=excl_walker->next;
			continue;
		}
		printf("IGNORE: %s MATCH: %s\n", &pathstring[2], excl_walker->exclude_path);
		return -1;
	}
	return 0;
}

struct dirnode *anakin_filewalker(struct dirnode *luke, struct dirnode *leia, const char *source, char *cpath, long long time, struct exclude_list *excludes){		
	struct dirent *pwd_ent;
	struct stat buf;
	DIR* pwd;
	
	char path[FD_NAMELEN]={0};

	if((pwd=opendir("."))==NULL){
		perror("opendir");
		FilesFailed++;
		return leia;
	}
	
	while((pwd_ent=readdir(pwd))){
		
		if((pwd_ent->d_name[0]=='.') && ( \
			(pwd_ent->d_name[1]=='\0') || \
			((pwd_ent->d_name[1]=='.')&&(pwd_ent->d_name[2]=='\0'))));
		else{
			strcpy(path, pwd_ent->d_name);

		#ifdef WIN32
			if(stat(path, &buf)<0){
		#else
			if(lstat(path, &buf)<0){
		#endif
				FilesFailed++;
				perror("stat");
			} else {

				if(filter(cpath, pwd_ent->d_name, buf.st_mtime, time, excludes)==0){
					if(S_ISREG(buf.st_mode)){					
						upload(source, cpath, pwd_ent->d_name, buf);
					}

					if(S_ISDIR(buf.st_mode)){					
						if(leia!=NULL){
							leia->next=calloc(1,sizeof(struct dirnode));
							leia=leia->next;
						}else{
							luke->sub=leia=calloc(1,sizeof(struct dirnode));						
						}
						leia->top=luke;
						strcpy(leia->path, path);
					}
				}
				FilesFound += 1;
			}
		}
	}

	closedir(pwd);
	
	return leia;
}


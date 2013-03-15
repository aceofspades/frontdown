#include "scandir.h"

int FilesFound=0;
int FilesFailed=0;
int FilesUpdated=0;


int upload(struct frontdown_config *config, const char *source, char *relpath, char *name, struct stat filestat){
	char srcpathstring[FD_PATHLEN*2]={0};
	char dstpathstring[FD_PATHLEN*2]={0};
	char ret;
	int len=-1;
	
//	strcpy(srcpathstring, source);
//	len=strlen(source);
//	srcpathstring[len]='/';
//	strcpy(&srcpathstring[len+1], relpath);
//	len+=1+strlen(relpath);
//	srcpathstring[len]='/';
	strcpy(&srcpathstring[len+1], name);

	strcpy(dstpathstring, config->destination);
	len=strlen(config->destination);
	dstpathstring[len]='/';
	strcpy(&dstpathstring[len+1], relpath);
	len+=1+strlen(relpath);
	dstpathstring[len]='/';
	strcpy(&dstpathstring[len+1], name);
	
	config->info(dstpathstring);


	if((ret=put_file(config, srcpathstring, name, dstpathstring, filestat.st_size))==0){
		FilesUpdated++;
		return 0;
	} else if(ret==-1){
		FilesFailed++;
		return -1;
	} else {
		return -2;
	}
}


int fd_scandir(struct frontdown_config *config, long long timestamp){
	char pathstring[FD_PATHLEN*2]={0};
	char *dirpointer[128];
	int dirptrc=0;
	char ret;
	struct dirnode *node, *root, *freewilli;
	
	char sourcepath[FD_PATHLEN*2];
	strcpy(sourcepath, config->source);

	char orig_dir[FD_PATHLEN*2];
	strcpy(orig_dir, get_current_dir_name());

	if(chdir(config->source)<0){
		perror("chdir");
		return -1;
	}

	root=calloc(1,sizeof(struct dirnode));	
	strcpy(root->path, ".");
	pathstring[0]='.';
	dirpointer[dirptrc]=&pathstring[1];

	create_dest_dir(config, pathstring);
	
	node=root;

	do{
		config->info(pathstring);

		if((ret=anakin_filewalker(config, node, node->sub, sourcepath, pathstring, timestamp, config->excludes))==0){ //No more subdirs

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

		} else if(ret==1) { //More subdirs
			node=node->sub; //goto subdir
		} else {
			return -1;
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
		
	create_dest_dir(config, pathstring);

	}while(node->top!=NULL);
	
	printf("Total %d files\n \
	\rUpdated %d files\n \
	\rFailed on %d files\n", FilesFound, FilesUpdated, FilesFailed);

	chdir(orig_dir);
	return 0; 
}


int filter(char *path, char *name, long long timestamp, long long time, struct frontdown_exclude_list *excludes){
	int status;
	char pathstring[FD_PATHLEN]={0};
	struct frontdown_exclude_list *excl_walker;
	regex_t re;
	
	excl_walker=excludes;

	strcpy(pathstring, path);
	status=strlen(path);
	pathstring[status]='/';
	strcpy(&pathstring[status+1], name);

	while(excl_walker!=NULL){
		if(excl_walker->exclude_path[0]==0){
			strcpy(excl_walker->exclude_path, "^index.db$");
		}
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

int anakin_filewalker(struct frontdown_config *config, struct dirnode *luke, struct dirnode *leia, const char *source, char *cpath, long long time, struct frontdown_exclude_list *excludes){		
	struct dirent *pwd_ent;
	struct stat buf;
	DIR* pwd;
	
	char path[FD_NAMELEN]={0};

	if((pwd=opendir("."))==NULL){
		perror("opendir");
		FilesFailed++;
		if(leia==NULL) return 0;
		return 1;
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

				if(S_ISREG(buf.st_mode)){					
					FilesFound += 1;
				}

				if(filter(cpath, pwd_ent->d_name, buf.st_mtime, time, excludes)==0){
					if(S_ISREG(buf.st_mode)){
						if(time<buf.st_mtime){
							if(upload(config, source, cpath, pwd_ent->d_name, buf)<-1)return -1;
						}
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
			}
		}
	}

	closedir(pwd);
	
	if(leia==NULL) return 0;
	return 1;
}


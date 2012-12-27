#include "scandir.h"

int n=0;


void upload(const char *source, char *relpath, char *name, long long timestamp){}


int fd_scandir(const char* path, long long timestamp, struct exclude_list *excludes, struct exclude_list *dir_excludes){
	char pathbuf[65536]={0};
	char *dirpointer[128];
	int dirptrc=0;
	
	char *orig_dir=get_current_dir_name();
	if(chdir(path)<0){
		perror("chdir");
		return -1;
	}
	
	struct dirnode *node, *root, *freewilli;

	root=calloc(1,sizeof(struct dirnode));	
	strcpy(root->path, ".");
	pathbuf[0]='.';
	dirpointer[dirptrc]=&pathbuf[1];
	
	node=root;

	do{
		if(anakin_filewalker(node, node->sub, path, &pathbuf[0], timestamp, excludes, dir_excludes)==NULL){

			next_dir:

				freewilli=node;
				node=node->next;
				
				if(freewilli->top==NULL) break;

				freewilli->top->sub=node;

				if(node==NULL){
					node=freewilli->top;

					free(freewilli);
					
					if(node->top==NULL)break;
					
					if(chdir("..")<0){
						perror("chdir");
						return -1;
					}
					dirptrc--;
					*dirpointer[dirptrc]='\0';
					
			goto next_dir;
				
				}
						
			free(freewilli);

			if(chdir("..")<0){
				perror("chdir");
				return -1;
			}
			dirptrc--;
			*dirpointer[dirptrc]='\0';

		}else{
			node=node->sub;
		}

		if(chdir(node->path)<0){
			perror("chdir");
			puts(node->path);
			return -1;
		}

		dirptrc++;
		*dirpointer[dirptrc-1]='/';
		memcpy(dirpointer[dirptrc-1]+1, node->path, strlen(node->path));
		dirpointer[dirptrc]=dirpointer[dirptrc-1]+1+strlen(node->path);
		*dirpointer[dirptrc]='\0';

	}while(node->top!=NULL);
	
	printf("Total %d files\n", n);

	chdir(orig_dir);
	return 0; 
}


int filter(char *name, long long timestamp, long long time, struct exclude_list *excludes){
	if(time>timestamp)return -1;
	
	return 0;
}

struct dirnode *anakin_filewalker(struct dirnode *luke, struct dirnode *leia, const char *source, char *cpath, long long time, struct exclude_list *excludes, struct exclude_list *dir_excludes){		
	struct dirent *pwd_ent;
	struct stat buf;
	DIR* pwd;
	
	char path[FD_PATHLEN]={0};

	if((pwd=opendir("."))==NULL){
		perror("opendir");
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
				perror("stat");
			} else {


				if(filter(pwd_ent->d_name, buf.st_mtime, time, excludes)==0){
					printf("%s/%s\n", cpath, pwd_ent->d_name);
					upload(source, cpath, pwd_ent->d_name, buf.st_mtime);
				}
{
	//			puts("\t------------------------------------");
	//
	//			printf("\t|Dev:\t%i\n", (int)buf.st_dev);
	//			printf("\t|Inode:\t%i\n", (int)buf.st_ino);
	//			printf("\t|Mode:\t%o\n", (int)buf.st_mode);
	//			printf("\t|Nlink:\t%i\n", (int)buf.st_nlink);
	//			printf("\t|UID:\t%i\n", (int)buf.st_uid);
	//			printf("\t|GID:\t%i\n", (int)buf.st_gid);
	//			printf("\t|Size:\t%ld\n", (long)buf.st_size);
	//			printf("\t|atime:\t%s", ctime(&buf.st_atime));
	//			printf("\t|mtime:\t%s", ctime(&buf.st_mtime));
	//			printf("\t|ctime:\t%s", ctime(&buf.st_ctime));
	//
	//			puts("\t------------------------------------");
}
				
//				if(&buf.st_atime)

				if(S_ISDIR(buf.st_mode)){
					
					struct exclude_list *exclude_walker;
					int i,offs;
					exclude_walker=dir_excludes;
					
					while(exclude_walker!=NULL){
						i=0;						
								printf("EXCLUDE: %s\n", exclude_walker->exclude_path);
						if(strlen(exclude_walker->exclude_path)+1==strlen(pwd_ent->d_name)+strlen(cpath)){
								printf("EXCLUDE: %s\n", exclude_walker->exclude_path);
							while((exclude_walker->exclude_path[i]-cpath[i+2])==0)i++;
								printf("EXCLUDE: %s CPATH %d %d %s\n", exclude_walker->exclude_path,i+1,cpath[i+1],cpath);
							if(cpath[i+1]=='\0'){
								printf("EXCLUDE: %s\n", exclude_walker->exclude_path);
								offs=++i;
								while(((exclude_walker->exclude_path[i]-pwd_ent->d_name[i-offs])==0)&&\
								(exclude_walker->exclude_path[i]!='\0'))i++;
								if(exclude_walker->exclude_path[i]!='\0') goto ignore_dir;
							}
						}
						exclude_walker=exclude_walker->next;
					}
					
					
					if(leia!=NULL){
						leia->next=calloc(1,sizeof(struct dirnode));
						leia=leia->next;
					}else{
						luke->sub=leia=calloc(1,sizeof(struct dirnode));						
					}
					leia->top=luke;
					strcpy(leia->path, path);
				}
				ignore_dir:
				n += 1;
			}
		}
	}

	closedir(pwd);
	
	return leia;
}


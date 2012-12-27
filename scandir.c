#include "scandir.h"

int n=0;

int fd_scandir(const char* path, long long timestamp, struct bucket *buck, size_t n_bucks){
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
		if(anakin_filewalker(node, node->sub, &pathbuf[0], timestamp)==NULL){

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

struct dirnode *anakin_filewalker(struct dirnode *luke, struct dirnode *leia, char *cpath, long long time){		
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

				printf("%s/%s\n", cpath, pwd_ent->d_name);

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
					if(leia!=NULL){
						leia->next=calloc(1,sizeof(struct dirnode));
						leia=leia->next;
					}else{
						luke->sub=leia=calloc(1,sizeof(struct dirnode));						
					}
					leia->top=luke;
					strcpy(leia->path, path);
				}
				n += 1;
			}
		}
	}

	closedir(pwd);
	
	return leia;
}


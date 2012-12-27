#include "scandir.h"

int n=0;

int fd_scandir(const char* path){
	char *orig_dir=get_current_dir_name();
	if(chdir(path)<0){
		perror("chdir");
		return -1;
	}
	
	struct dirnode *node, *root, *freewilli;

	root=calloc(1,sizeof(struct dirnode));	
	strcpy(root->path, ".");
	node=root;

	do{
		if(anakin_filewalker(node, node->sub)==NULL){
			printf("[--DEBUG-NO-SUBDIR--\n");
			
			freewilli=root;
			
			while(freewilli!=NULL){
				puts(freewilli->path);
				freewilli=freewilli->sub;
			}

			next_dir:
				
				freewilli=node;
				node=node->next;
				
				printf("TRY NEXT PATH: %s\n",node->path);
				
				freewilli->top->sub=node;

				if(node==NULL){
					node=freewilli->top;

					printf("TRY PARENT PATH: %s\n", node->path);

					free(freewilli);
					
					if(node->top==NULL)break;
					
					if(chdir("..")<0){
						perror("chdir");
						return -1;
					}

			goto next_dir;
				
				}
			
			
			free(freewilli);

			if(chdir("..")<0){
				perror("chdir");
				return -1;
			}

			printf("--DEBUG-NO-SUBDIR--]\n");

		}else{
			node=node->sub;
		}

		if(chdir(node->path)<0){
			perror("chdir");
			puts(node->path);
			return -1;
		}
	}while(node->top!=NULL);
	
	printf("Total %d files\n", n);

	
	
	chdir(orig_dir);
	return 0; 
}

struct dirnode *anakin_filewalker(struct dirnode *luke, struct dirnode *leia){		
	struct dirent *pwd_ent;
	struct stat buf;
	DIR* pwd;
	
	char path[FD_PATHLEN]={0};

	if((pwd=opendir("."))==NULL){
		printf("FAILED OPENDIR AT: %s \n", luke->path);
		perror("opendir");
		return leia;
	}
	
	while((pwd_ent=readdir(pwd))){
		
		if((pwd_ent->d_name[0]=='.') && ( \
			(pwd_ent->d_name[1]=='\0') || \
			((pwd_ent->d_name[1]=='.')&&(pwd_ent->d_name[2]=='\0'))));
		else{
			strcpy(path, pwd_ent->d_name);

			if(stat(path, &buf)<0){
				printf("FAILED STAT AT: %s \n", path);
				perror("stat");
			} else {

//				printf("%s/%s:\n", luke->path, pwd_ent->d_name);

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


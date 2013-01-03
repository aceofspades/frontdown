#include "frontdown_internal.h"

int frontdown(struct frontdown_config *config){
	char indexpath[FD_PATHLEN+20];
	char *dest;
	char fixbuf[64];
	char *buf;
	struct stat statbuf;
	
	config->info("Backup started");

	dest=(char*)malloc(FD_PATHLEN+30);
	
	strcpy(indexpath, config->destination);
	strcat(indexpath, "/index.db");
	
	if(open_destination(config)==-1){
		return -1;
	}

	remove("./index.db");
	if(config->last_backup > 0){
		if(get_indexfile(config, indexpath)!=0) return -1;
		config->last_backup=0;
		//Read last backup time
		if((index_db=fopen("./index.db", "rb"))){
			fscanf(index_db, "%lld", &last_backup_time);
			config->last_backup=1;
			fclose(index_db);
		}
	}

	//Rewrite index.db
	index_db=fopen("./index.db", "wb+");
	rewind(index_db);
	fprintf(index_db, "%0*lld\n", 15, config->now);
	
	if(create_dest_dir(config, "/")!=0) return -1;

	sprintf(fixbuf,"/BACKUP%0*lld/", 15, config->now);
	strcpy(dest, config->destination);
	strcat(dest, fixbuf);

	free(config->destination);
	config->destination=dest;

	config->info(config->destination);

	fd_scandir(config, last_backup_time);

	fclose(index_db);

	if(config->last_backup != 0){
		if(get_indexfile(config, indexpath)!=0) return -1;
	}

	stat("./index.db", &statbuf);
	put_file(config, "./index.db", "index.db", indexpath, statbuf.st_size); 

	close_destination();

	remove("./index.db");

	return 0;
}

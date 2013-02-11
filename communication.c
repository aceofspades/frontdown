#include "communication.h"

struct fd_file{
	char *filename;
	FILE *stream;
};

struct {
	void *curl;
	CURLcode result;
	struct curl_slist *commandlist;
} dst_connection;


int fileappend(void *buf, size_t size, size_t nmemb, void *stream){
	struct fd_file *out = (struct fd_file *)stream;
	if(out && !out->stream){
		out->stream=fopen(out->filename, "ab");
		
		if(!out->stream)
			return -1;
		
		return fwrite(buf, size, nmemb, out->stream);
	}
}


int fileread(void *ptr, int size, int n, void *stream){
	return (curl_off_t)fread(ptr, size, n, stream);
}


int get_indexfile(struct frontdown_config *config, char *source){
	struct fd_file downloadfile = {"index.db", NULL};
	
	if(dst_connection.curl){
		curl_easy_setopt(dst_connection.curl, CURLOPT_URL, source);
		curl_easy_setopt(dst_connection.curl, CURLOPT_WRITEFUNCTION, fileappend);
		curl_easy_setopt(dst_connection.curl, CURLOPT_WRITEDATA, &downloadfile);
		//~ curl_easy_setopt(dst_connection.curl, CURLOPT_VERBOSE, 1L);
		
		dst_connection.result = curl_easy_perform(dst_connection.curl);
		
		if(dst_connection.result != CURLE_OK){
			if(downloadfile.stream)
				fclose(downloadfile.stream);

			if(config->error(_INDEX_DB_ERROR_, 0, source)!=0){
				return -1;
			} else {
				return 0;
			}
		}
	}

	if(downloadfile.stream)
		fclose(downloadfile.stream);
		
	return 0;
}

int create_dest_dir(struct frontdown_config *config, char *relpath){
	char fullpath[FD_PATHLEN*2]={0};

	switch(config->con_type){
		case _FILE_:
			strcpy(fullpath, &config->destination[7]);
			strcat(fullpath, &relpath[1]);
			if((mkdir(fullpath , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))!=0){
				if((errno==EEXIST)&&(config->error(_MKDIR_ERROR_, 0, fullpath)==0)){
					return 0;
				} else {
					config->error(_MKDIR_ERROR_, 1, fullpath);
					return -1;
				}
			}
		break;

		case _UNKNOWN_:
		default:
		break;
	}
	return 0;
}

int close_destination(){
	curl_easy_cleanup(dst_connection.curl);
	curl_global_cleanup();
	return 0;
}

int open_destination(struct frontdown_config *config){
	CURL *curl;
	curl_global_init(CURL_GLOBAL_ALL);
	dst_connection.curl = curl=curl_easy_init();

	if(dst_connection.curl){
		dst_connection.commandlist = curl_slist_append(dst_connection.commandlist, "NOOP");
		curl_easy_setopt(dst_connection.curl, CURLOPT_READFUNCTION, fileread);
		curl_easy_setopt(dst_connection.curl, CURLOPT_FTP_CREATE_MISSING_DIRS , 1);
		//~ curl_easy_setopt(dst_connection.curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(dst_connection.curl, CURLOPT_URL, config->destination);
	
		if(config->destinationUname!=NULL){
			curl_easy_setopt(dst_connection.curl, CURLOPT_USERNAME, config->destinationUname);			
		}
		if(config->destinationPwd!=NULL){
			curl_easy_setopt(dst_connection.curl, CURLOPT_PASSWORD, config->destinationPwd);			
		}
		
		dst_connection.result = curl_easy_perform(dst_connection.curl);
		if(dst_connection.result != CURLE_OK && \
			dst_connection.result != CURLE_REMOTE_FILE_NOT_FOUND && \
			dst_connection.result != CURLE_FILE_COULDNT_READ_FILE )
		{
			config->error(_DST_CONNECTION_ERROR_, 1, config->destination);
			return -1;
		}
	}
	curl_slist_free_all(dst_connection.commandlist);
	dst_connection.commandlist=NULL;
	return 0;
}

int put_file(struct frontdown_config *config, char *source, char *filename, char *target, curl_off_t size){
	FILE *handle = fopen(source, "rb");
	
	char cmd_buffer[strlen(source)+6];

	if(config->con_type==_FTP_){
		strcpy(cmd_buffer, "RNFR ");
		strcat(cmd_buffer, filename);
	}

	if(dst_connection.curl && handle != NULL){
		dst_connection.commandlist = curl_slist_append(dst_connection.commandlist, cmd_buffer);
		curl_easy_setopt(dst_connection.curl, CURLOPT_URL, target);
		curl_easy_setopt(dst_connection.curl, CURLOPT_UPLOAD, 1);
		curl_easy_setopt(dst_connection.curl, CURLOPT_POSTQUOTE, dst_connection.commandlist);
		curl_easy_setopt(dst_connection.curl, CURLOPT_READDATA, handle);
		curl_easy_setopt(dst_connection.curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)size);
				
		dst_connection.result = curl_easy_perform(dst_connection.curl);
		if(dst_connection.result != CURLE_OK){

			curl_slist_free_all(dst_connection.commandlist);
			dst_connection.commandlist=NULL;
			fclose(handle);

			//modify mtime
			if((handle=fopen(source, "ab")))
				fclose(handle);

			if(config->error(_PUT_FILE_ERROR_, 0, source)==0) return -1;
			return -2;
		}
	}
	
	fclose(handle);
	curl_slist_free_all(dst_connection.commandlist);
	dst_connection.commandlist=NULL;
	return 0;
}

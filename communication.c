#include "communication.h"

// This code was basically adjusted from
// http://curl.haxx.se/libcurl/c/ftpget.html


struct ftpfile{
	char *filename;
	FILE *stream;
};

struct {
	void *curl;
	CURLcode result;
	struct curl_slist *commandlist;
} dst_connection;


int filewrite(void *buf, size_t size, size_t nmemb, void *stream){
	struct ftpfile *out = (struct ftpfile *)stream;
	if(out && !out->stream){
		out->stream=fopen(out->filename, "wb");
		
		if(!out->stream)
			return -1;
	}
	
	return fwrite(buf, size, nmemb, out->stream);
}

int fileread(void *ptr, int size, int n, void *stream){
	return (curl_off_t)fread(ptr, size, n, stream);
}


int get_indexfile(char *source){
	CURL *curl;
	CURLcode result;
	
	struct ftpfile downloadfile = {"index.db", NULL};
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	
	if(curl){
		curl_easy_setopt(curl, CURLOPT_URL, source);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, filewrite);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &downloadfile);
		//~ curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		
		result = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		
		if(result != CURLE_OK){
			fprintf(stderr, "Could not download index file.\n");
			return 1;
		}
	}
	
	if(downloadfile.stream)
		fclose(downloadfile.stream);
	
	curl_global_cleanup();
	
	return 0;
}

int create_dest_dir(char *relpath){
	char fullpath[FD_PATHLEN]={0};

	if(config.con==-1){
		char buf[6]={0};
		int i;
		for(i=0;i<6;i++){
			if(config.destination[i]==':') break;
			buf[i]=config.destination[i];
		}
		if(strcasecmp(buf,"file")==0) config.con=_FILE_;
	}
	
	switch(config.con){
		case _FILE_:
			strcpy(fullpath, &config.destination[7]);
			strcat(fullpath, relpath);
			mkdir(fullpath ,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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

int open_destination(char *target){
	CURL *curl;
	curl_global_init(CURL_GLOBAL_ALL);
	dst_connection.curl = curl=curl_easy_init();

	if(dst_connection.curl){
		dst_connection.commandlist = curl_slist_append(dst_connection.commandlist, "NOOP");
		curl_easy_setopt(dst_connection.curl, CURLOPT_READFUNCTION, fileread);
		curl_easy_setopt(dst_connection.curl, CURLOPT_FTP_CREATE_MISSING_DIRS , 1);
		//~ curl_easy_setopt(dst_connection.curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(dst_connection.curl, CURLOPT_URL, target);
	
		if(config.destinationLogin){
			curl_easy_setopt(dst_connection.curl, CURLOPT_USERNAME, config.destinationUname);			
			curl_easy_setopt(dst_connection.curl, CURLOPT_PASSWORD, config.destinationPwd);			
		}
		
		dst_connection.result = curl_easy_perform(dst_connection.curl);
		if(dst_connection.result != CURLE_OK){
			if(errno==EISDIR)return 0;
			perror("CURL DST_CONNECTION");
			exit(-1);
			return 1;
		}
	}
	curl_slist_free_all(dst_connection.commandlist);
	dst_connection.commandlist=NULL;
	return 0;
}

int put_file(char *source, char *filename, char *target, curl_off_t size){
	FILE *handle = fopen(source, "rb");
	
	char cmd_buffer[strlen(source)+6];
	strcpy(cmd_buffer, "RNFR ");
	strcat(cmd_buffer, filename);
	
	if(dst_connection.curl && handle != NULL){
		dst_connection.commandlist = curl_slist_append(dst_connection.commandlist, cmd_buffer);
		curl_easy_setopt(dst_connection.curl, CURLOPT_URL, target);
		curl_easy_setopt(dst_connection.curl, CURLOPT_UPLOAD, 1);
		curl_easy_setopt(dst_connection.curl, CURLOPT_POSTQUOTE, dst_connection.commandlist);
		curl_easy_setopt(dst_connection.curl, CURLOPT_READDATA, handle);
		curl_easy_setopt(dst_connection.curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)size);
				
		dst_connection.result = curl_easy_perform(dst_connection.curl);
		if(dst_connection.result != CURLE_OK){
			perror("PUT_FILE");
			curl_slist_free_all(dst_connection.commandlist);
			dst_connection.commandlist=NULL;
			fclose(handle);
			return 1;
		}
	}
	
	fclose(handle);
	curl_slist_free_all(dst_connection.commandlist);
	dst_connection.commandlist=NULL;
	return 0;
}

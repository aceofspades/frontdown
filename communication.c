#include "communication.h"

// This code was basically adjusted from
// http://curl.haxx.se/libcurl/c/ftpget.html


struct ftpfile{
	char *filename;
	FILE *stream;
};

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

int put_file(char *source, char *filename, char *target, curl_off_t size){
	CURL *curl;
	CURLcode result;
	
	struct curl_slist *commandlist=NULL;
	FILE *handle = fopen(source, "rb");
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	
	char cmd_buffer[strlen(source)+6];
	strcpy(cmd_buffer, "RNFR ");
	strcat(cmd_buffer, filename);
	
	if(curl && handle != NULL){
		commandlist = curl_slist_append(commandlist, cmd_buffer);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, fileread);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
		curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS , 1);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_URL, target);
		curl_easy_setopt(curl, CURLOPT_POSTQUOTE, commandlist);
		curl_easy_setopt(curl, CURLOPT_READDATA, handle);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)size);
		
		if(config.destinationLogin){
			curl_easy_setopt(curl, CURLOPT_USERNAME, config.destinationUname);			
			curl_easy_setopt(curl, CURLOPT_PASSWORD, config.destinationPwd);			
		}
		
		result = curl_easy_perform(curl);
		if(result != CURLE_OK){
			fclose(handle);
			return 1;
		}
		
		curl_slist_free_all(commandlist);
		curl_easy_cleanup(curl);
	}
	
	fclose(handle);
	curl_global_cleanup();
	
	return 0;

}

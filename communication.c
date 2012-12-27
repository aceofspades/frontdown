#include <stdio.h>
#include <curl/curl.h>

#include "frontdown.h"

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
		
		if(CURLE_OK != result){
			fprintf(stderr, "Could not download index file.\n");
		}
	}
	
	if(downloadfile.stream)
		fclose(downloadfile.stream);
	
	curl_global_cleanup();
	
	return 0;
}

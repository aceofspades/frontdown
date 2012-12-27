#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "frontdown.h"
#include "parser.h"

int parse_config(void){	
	// Init the search
	search_init(config.conf);
	
	char *test_ptr;
	test_ptr = search_for_key("source", 0);
	if(test_ptr == NULL){
		search_destroy();
		return 0;
	}
	memcpy(config.source, test_ptr, 16384);
	
	
	test_ptr = search_for_key("destination", 0);
	if(test_ptr == NULL){
		search_destroy();
		return 0;
	}
	memcpy(config.destination, test_ptr, 16384);
	
	
	test_ptr = search_for_key("threads", 0);
	if(test_ptr == NULL){
		search_destroy();
		return 0;
	}
	config.threads = atoi(test_ptr);
	
	test_ptr = search_for_key("hidden", 0);
	if(test_ptr == NULL){
		search_destroy();
		return 0;
	}
	config.hidden = strcmp(test_ptr, "yes")==0?1:0;
	
	// optional
	test_ptr = search_for_key("lastbackup", 0);
	if(test_ptr != NULL){
		config.last_backup = atoi(test_ptr);
		printf(">>%d<<\n", config.last_backup);
	}
	
	
	// Free used memory
	search_destroy();
	
	return 1;
}

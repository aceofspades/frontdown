#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#include "frontdown.h"
#include "parser.h"


void parse_options(int argc, char **argv){
	struct option command_options[] = {
		{"source", required_argument, 0, 's'},
		{"destination", required_argument, 0, 'd'},
		{"threads", required_argument, 0, 't'},
		{"hidden", no_argument, 0, 'H'},
		{"help", no_argument, 0, 'h'},
		{"conf", required_argument, 0, 'c'},
		{"exclude-file", required_argument, 0, 'e'},
		{"exclude-directory", required_argument, 0, 'E'},
		{0, 0, 0, 0}
	};
	int opt, option_index=0;
	
	// Defaults
	config.hidden      = 0;
	config.threads     = 1;
	config.last_backup = 0;

	char s=0, d=0, c=0;

	while(1){
		opt = getopt_long(argc, argv, "s:d:t:Hhc:e:E:", command_options, &option_index);
		if(opt == -1)
			break;
		
		if(opt == 's'){
			strncpy(config.source, optarg, 16383);
			s=1;
		} else if(opt == 'd'){
			strncpy(config.destination, optarg, 16383);
			d=1;
		} else if(opt == 't'){
			config.threads = atoi(optarg);
		} else if(opt == 'H'){
			config.hidden = 1;
		} else if(opt == 'h'){
			help();
			exit(0);
		} else if(opt == 'c'){
			strncpy(config.conf, optarg, 16383);
			if(!parse_config()){
				fprintf(stderr, "Error parsing configuration file\n");
				abort();
			}
			c=1;
			
		} else if(opt == 'e'){
			strncpy(latest_file_exclude->exclude_path, optarg, 16383);
			latest_file_exclude->next = calloc(1, sizeof(struct exclude_list));
			latest_file_exclude = latest_file_exclude->next;
		} else if(opt == 'E'){
			strncpy(latest_dir_exclude->exclude_path, optarg, 16383);
			latest_dir_exclude->next = calloc(1, sizeof(struct exclude_list));
			latest_dir_exclude = latest_dir_exclude->next;
		} else{
			abort();
		}
	}
	
	if(!(c || (s && d))){
		errno = EINVAL;
		perror("Need configuration file or source and destination");
		abort();
	}
	
	if(optind < argc){
		printf("Unkown argument: %s\n", argv[optind]);
	}
}

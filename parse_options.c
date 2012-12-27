#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#include "frontdown.h"


void parse_options(int argc, char **argv){
	struct option command_options[] = {
		{"source", required_argument, 0, 's'},
		{"destination", required_argument, 0, 'd'},
		{"threads", required_argument, 0, 't'},
		{"hidden", no_argument, 0, 'H'},
		{"help", no_argument, 0, 'h'},
		{"conf", required_argument, 0, 'c'},
		{0, 0, 0, 0}
	};
	int opt, option_index=0;
	config.hidden  = 0;
	config.threads = 1;

	char s=0, d=0, c=0;

	while(1){
		opt = getopt_long(argc, argv, "s:d:t:Hhc:", command_options, &option_index);
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
			// TODO parse config
			c=1;
		} else{
			exit(1);
		}
	}
	
	if(!(c || (s && d))){
		errno = EINVAL;
		perror("Need configuration file or source and destination");
		exit(1);
	}
	
	if(optind < argc){
		printf("Unkown argument: %s\n", argv[optind]);
	}
}

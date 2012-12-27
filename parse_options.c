#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "frontdown.h"


void parse_options(int argc, char **argv){
	struct option command_options[] = {
		{"source", required_argument, 0, 's'},
		{"destination", required_argument, 0, 'd'},
		{"threads", required_argument, 0, 't'},
		{"hidden", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};
	int opt, option_index=0;
	config.hidden  = 0;
	config.threads = 1;

	while(1){
		opt = getopt_long(argc, argv, "s:d:t:h", command_options, &option_index);
		if(opt == -1)
			break;
		
		if(opt == 's')
			strncpy(config.source, optarg, 16384);
		else if(opt == 'd')
			strncpy(config.destination, optarg, 16384);
		else if(opt == 't')
			config.threads = atoi(optarg);
		else if(opt == 'h')
			config.hidden = 1;
		else
			exit(0);
	}
	
	if(optind < argc){
		printf("Unkown argument: %s\n", argv[optind]);
	}
}

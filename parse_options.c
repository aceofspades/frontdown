#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#include "frontdown.h"
#include "parser.h"


int parse_options(int argc, char **argv){
	struct option command_options[] = {
		{"source", required_argument, 0, 's'},
		{"destination", required_argument, 0, 'd'},
		{"help", no_argument, 0, 'h'},
		{"conf", required_argument, 0, 'c'},
		{"exclude", required_argument, 0, 'e'},
		{"login", no_argument, 0, 'l'},
		{0, 0, 0, 0}
	};
	int opt, option_index=0;
	
	// Defaults
	config.last_backup = 0;
	config.destinationLogin = 0;

	char s=0, d=0, c=0;

	while(1){
		opt = getopt_long(argc, argv, "s:d:t:hc:e:lbu", command_options, &option_index);
		if(opt == -1)
			break;
		
		if(opt == 's'){
			if(s){
				fprintf(stderr, "Cannot use more than 1 source directory at the moment\n");
				return(-1);
			}
			
			strncpy(config.source, optarg, 16383);
			s=1;
		} else if(opt == 'd'){
			if(d){
				fprintf(stderr, "Cannot use more than 1 destination directory at the moment\n");
				return(-1);
			}
			
			strncpy(config.destination, optarg, 16383);
			d=1;
		} else if(opt == 'l'){
			config.destinationLogin = 1;
		} else if(opt == 'h'){
			help();
			return(-1);
		} else if(opt == 'c'){
			if(c){
				fprintf(stderr, "Only 1 configuration file is allowed\n");
				return(-1);
			}
			
			strncpy(config.conf, optarg, 16383);

			if(!parse_config()){
				fprintf(stderr, "Error parsing configuration file\n");
				return(-1);
			}
			c=1;
			
		} else if(opt == 'e'){
			strncpy(latest_exclude->exclude_path, optarg, 16383);
			latest_exclude->next = calloc(1, sizeof(struct exclude_list));
			latest_exclude = latest_exclude->next;
		} else if(opt == 'b'){
			config.last_backup=1;
		}
		#ifdef _GUI_
		else if(opt == 'u'){
			gui();
			break;
		}
		#endif
		else{
			return(-1);
		}
	}
	
	if(!(c || (s && d))){

		#ifdef _GUI_
			gui();
			if((config.source[0] && config.destination[0])) return;
		#endif

		fprintf(stderr, "Need configuration file or source and destination");
		return(-1);
	}
	
	if(optind < argc){
		fprintf(stderr, "Unkown argument: %s\n", argv[optind]);
		return(-1);
	}
	return(0);
}

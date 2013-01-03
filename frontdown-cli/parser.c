/*
 * Copyright 2012 Patrick Eigensatz
 *
 * This file is licensed under the GNU LGPL 3.0
 * You find the license here:
 * http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 * This code is distributed AS-IS and WITHOUT ANY WARRANTY.
 * See the license file for details.
 *
 * About this file:
 *
 * This file was originally written in 2012 as a simple parser for
 * configuration files in the Cubebox project.
 *
 *
 * You're free to use this file in one of your projects under the
 * terms of the license.
 *
 * Have fun!
 *
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct key_value{
	char *key;
	char *value;
};

/* 2nd return value of get_pair
 *
 * 1	File completly read
 * 2	Empty line or line == NULL
 * 3	Comment
 */
char __PARSE_ERR=0;

/* We use this function to receive a pointer where exactly one line
 * is stored. Remember: Using this function, lines are not limited!
 */
char *next_line(FILE *stream){
	unsigned int buffersize=32;
	int string_length;
	char *line = (char*)malloc(buffersize);
	char *pos  = line;

	// Init the line
	strcpy(line, "\0");

	// Save value for further use!
	string_length = strlen(line);

	// Read as long as last character is newline or we read everything
	// compatibility note: works on Linux ('\n') and Windows. ("\r\n")
	while(line[string_length-1] != '\n'){
		if(feof(stream)){
			__PARSE_ERR = 1;
			break;
		}

		// Read until the buffer is full
		fgets(pos, buffersize-string_length, stream);
		string_length = strlen(line);

		// If we are at the end of the buffer, expand the buffer
		if(string_length == buffersize-1){
			buffersize *= 2;
			line = realloc(line, buffersize);
			pos  = line + string_length;
		}
	}

	// Cut the newline character at the end
	*(line+string_length-1) = '\0';

	return line;
}


/* Now we need to cut '\t' and comments to split the value from
 * the key. Neither the key nor the value is length-limited!
 */
struct key_value *get_pair(char *line){
	struct key_value *pair = (struct key_value *)malloc(sizeof(struct key_value));
	unsigned int buffersize=16;
	char *lineptr;

	if(__PARSE_ERR == 1){
		free(pair);
		return NULL;
	}

	if(strcmp(line, "") == 0 || line == NULL){
		__PARSE_ERR = 2;
		free(pair);
		return NULL;
	}

	// Drop everything behind a hashtag!
	lineptr  = strchr(line, '#');

	if(lineptr == line){
		__PARSE_ERR = 3;
		free(pair);
		return NULL;

	} else if(lineptr != NULL){
		*lineptr = '\0';
	}

    // Get first occurence of a tab before a hashtag
    lineptr = strchr(line, '\t');
    if(lineptr == NULL){
		perror("Error on reading line\n");
		__PARSE_ERR = 3;
		free(pair);
		return NULL;
	}
    *lineptr = '\0';

    buffersize = strlen(line);

	// Allocate and copy the line into the key
	pair->key = (char *)malloc(buffersize+1);
	if(pair->key == NULL){
		perror("Error allocating memory!\n");
		return NULL;	
	}

	// Read the key
	strcpy(pair->key, line);
	*(line+buffersize) = '\t';


	// Cut comments in the value
	lineptr = strchr(line, '#');
	if(lineptr != NULL){
		*lineptr = '\0';
		free(pair);
		return NULL;
	}

	// Now get everything behind the last tab!
	lineptr = strrchr(line, '\t');
	if(lineptr == NULL){
		perror("Error on reading line\n");
		free(pair);
		return NULL;		
	}


	// Allocate memory for the rest of the line
	buffersize = strlen(lineptr+1);
	pair->value = (char *)malloc(buffersize+1);
	if(pair->value == NULL){
		perror("Error allocating memory!\n");
		return NULL;
	}


	// Read the line
	strcpy(pair->value, lineptr+1);

	// Finished!
	__PARSE_ERR = 0;
	return pair;
}

// This is just more confortable than calling both functions.
struct key_value *parse_next_line(FILE *stream){
	struct key_value *tmp = get_pair(next_line(stream));

	while(__PARSE_ERR != 1){
		if(__PARSE_ERR == 0){
			return tmp;
		}

		tmp = get_pair(next_line(stream));
	}

	return NULL;
}



// Search-specific GLOBALS
struct key_value **__PARSER_SEARCH, **__PARSER_SEARCH_TMP;
unsigned int __PARSER_KEYS_READ=0;



/* Read everything into memory, so we can
 * search for a key later!
*/
void search_init(char *filename){
	FILE *stream = fopen(filename, "r");
	if(stream == NULL){
		perror("Error: Could not read file stream!\n");
		return;
	}
	
	unsigned int buffersize = 8;

	if(__PARSER_SEARCH == NULL){
		__PARSER_SEARCH = (struct key_value **)malloc(buffersize*sizeof(struct key_value *));
		if(__PARSER_SEARCH == NULL){
			perror("Error while initialising memory for the keys\n");
			fclose(stream);
			return;
		}
	} else {
		fclose(stream);
		return;
	}

	__PARSER_SEARCH[__PARSER_KEYS_READ] = parse_next_line(stream);
	while(__PARSER_SEARCH[__PARSER_KEYS_READ] != NULL){
		__PARSER_KEYS_READ++;
		__PARSER_SEARCH[__PARSER_KEYS_READ] = parse_next_line(stream);

		if(__PARSER_KEYS_READ == buffersize){
			buffersize *= 2;
			__PARSER_SEARCH_TMP = realloc(__PARSER_SEARCH, buffersize*sizeof(struct key_value *));

			if(__PARSER_SEARCH_TMP == NULL){
				perror("Error allocation additional memory for the keys\n");
				
				free(__PARSER_SEARCH);
				__PARSER_SEARCH = NULL;
				fclose(stream);
				
				return;
			} else
				__PARSER_SEARCH = __PARSER_SEARCH_TMP;

		}
	}
	
	fclose(stream);
}



/* Free the memory we used when we read the keys into memory */
void search_destroy(void){
	int i;

	if(__PARSER_SEARCH != NULL){
		for(i=0; i<__PARSER_KEYS_READ; i++){
			free(__PARSER_SEARCH[i]);
		}

		free(__PARSER_SEARCH);
	}

	__PARSER_KEYS_READ = 0;
}



/* Search the value for a specific key. (Allows to read everything and
 * return just the value we need.)
*/
char *search_for_key(char *key, char case_sensitive){
	if(__PARSER_SEARCH == NULL){
		perror("Error: Search was not initialized!\n");
		return NULL;
	}

	unsigned int i=0;


	if(case_sensitive == 0){
		for(i=0; i<__PARSER_KEYS_READ; i++){
			if(strcasecmp(__PARSER_SEARCH[i]->key, key) == 0)
				return __PARSER_SEARCH[i]->value;
		}
	} else {
		for(i=0; i<__PARSER_KEYS_READ; i++){
			if(strcmp(__PARSER_SEARCH[i]->key, key) == 0)
				return __PARSER_SEARCH[i]->value;
		}
	}

	return NULL;
}


// Free allocated memory for struct
void key_value_destroy(struct key_value *keyval){
	if(keyval != NULL){
		if(keyval->key != NULL)
			free(keyval->key);

		if(keyval->value != NULL)
			free(keyval->value);

		free(keyval);
	}
}

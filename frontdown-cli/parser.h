#ifndef __PARSER_H__
#define __PARSER_H__

extern int parse_config(void);
extern void search_init(char *filename);
extern char *search_for_key(char *key, char case_sensitive);
extern void search_destroy(void);

#endif
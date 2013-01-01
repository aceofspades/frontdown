#ifndef __MINGW32_FIXES_h__
#define __MINGW32_FIXES_h__

#warning "Using WIN32 compatibility fixes"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <direct.h>
#include <conio.h>

#define mkdir(a, b) _mkdir(a)

extern char *getpass();
extern char *get_current_dir_name();

#endif

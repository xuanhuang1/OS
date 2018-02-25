#ifndef TOKNIZER_H
#define TOKNIZER_H
#include "common.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct tokenizer{
char *str;
// the string to parse                                                                                     
char *pos;
// position in string                                                                                      
} TOKENIZER;

static const char delim[] ={'&', ';'};

//int init_tokenizer(TOKENIZER *t, char* line);
//char *get_next_token(TOKENIZER *tokenizer);
//int destory_tokenizer(TOKENIZER *t);
int parse(char*);

#endif

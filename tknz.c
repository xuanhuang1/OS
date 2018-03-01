#include "common.h"

typedef struct tokenizer{
char *str;
// the string to parse                                                                                                                                                      
char *pos;
// position in string                                                                                                                                                       
} TOKENIZER;

char delim[] ={'&', ';'};
/*
int init_tokenizer(TOKENIZER *t, char* line);
int repos_tokenizer(TOKENIZER *t, char* line);
char *get_next_token(TOKENIZER *tokenizer);
int destory_tokenizer(TOKENIZER *t);
*/

int init_tokenizer(TOKENIZER *t, char* line){
  if(line){
    t->str = (char*)malloc(strlen(line)+1);
    strcpy(t->str, line);
    t->str[strlen(line)] = '\0';
    t->pos = &line[0];
    return TRUE;
  }
  printf("Fail init_tokenizer\n");
  return FALSE;
}
int repos_tokenizer(TOKENIZER *t, char* line){
  t->pos = &line[0];
  return TRUE;
}

int isDelim(char c){
  int indx = 0;
  while(delim[indx]){
    if(c == delim[indx])
      return indx+1;
    indx++;
  }
  return FALSE;
}

char* getSubstr(int slength, char* src){
  char* string = (char*)malloc(sizeof(char)*(slength+1));
  memcpy(string, src, slength);
  string[slength] = '\0';
  return string;
}

char *get_next_token(TOKENIZER *tokenizer){
  int slength = 0;
  if(*(tokenizer->pos) == '\0')
    return NULL;

  while(*((tokenizer->pos)+1) != '\0'){
    if(!isspace(*(tokenizer->pos))){
      if(isDelim(*(tokenizer->pos))){
        char* string = getSubstr(1, tokenizer->pos);
        tokenizer->pos++;
        return string;
      }
      if( isDelim(*(tokenizer->pos+1)) || isspace(*(tokenizer->pos+1)) ){
        char* string = getSubstr(slength+1, tokenizer->pos-slength);
        //printf("string %s, %d\n",tokenizer->pos-slength, slength);                                                                                                        
        tokenizer->pos++;
        return string;
      }
      slength++;
    }
    tokenizer->pos++;
  }
  if(!isspace(*(tokenizer->pos))){
    char* string = getSubstr(slength+1, tokenizer->pos-slength);
    //printf("string %s\n",string);                                                                                                                                           
    tokenizer->pos++;
    return string;
  }
  return NULL;
}


int destory_tokenizer(TOKENIZER *t){
  free(t->str);
  t = NULL;
  return TRUE;
}

int parse(char* string){

  TOKENIZER t;
  init_tokenizer(&t, string);

  int n = 0;

  char* a;
  while((a=get_next_token(&t))!= NULL){
    n++;
    free(a);
  }

  repos_tokenizer(&t, string);
  args = (char**)malloc(sizeof(char*)*(n+1));
  int i = 0;
  while((args[i++]=get_next_token(&t))!= NULL){
    //printf("token b:%s \n",args[i]);                                                                                                                               
  }

  //if(!n){printf("empty imput!\n");}

  //printf("num of token: %d \n", n);                                                                                                                                
  destory_tokenizer(&t);
  return n;
}

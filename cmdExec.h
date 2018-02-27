#ifndef CMDEXEC_H
#define CMDEXEC_H

int checkBuiltIn(char** token, char* line);
int exeCmds(char**,char* line, int );
void freeGlobals();

#endif
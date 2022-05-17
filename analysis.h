#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <unistd.h>
#include <tchar.h>
#include <psapi.h>
#include <wchar.h>
#include "builtin.h"
#define TOKEN_DELIM " \t"

char *read_line();
char **split_line(char *line);
int excute(char **args);

void AnalysisCommand(char **args);

// using namespace std;
// vector<pair<string,DWORD>> v;
// int pos = 0;

char *read_line(){
    int buffer_size = 32;
    char *str;
    int position = 0;
    int ch;
    str = (char *)malloc(buffer_size); if(!str) return str;
    while(1){
        ch=getchar();
        if(ch==EOF || ch=='\n'){
            str[position] = '\0';
            return str;
        }else str[position] = ch;
        position ++;

        if(position >= buffer_size){
            buffer_size += 32;
            str = (char*)realloc(str, buffer_size);
            if(!str) return str;
        }
    }
    return str;
}
char **split_line(char *line){
    char **args = (char **)malloc(sizeof(char *));
    char *arg;
    char *str = (char *)malloc(strlen(line));
    str = strcpy(str, line);
    int index = 0;
    for(arg=strtok(str, " ");arg!=NULL;arg=strtok(NULL, " ")){
        args[index] = arg;
        index++;
        args = (char **)realloc(args, sizeof(char *)*(index+1));
        if(!args){
            exit(1);
        }
    }
    args[index] = NULL;
    index++;
    args = (char **)realloc(args, sizeof(char *)*(index+1));
    args[index] = NULL;
    return args;
}
int excute(char **args){
    if(args[0] == NULL) return 1;
    for(int i=0;i<num_builtins();i++){
        if(strcmp(args[0], builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }
    printf("command not found.\n");
    return 1;
}
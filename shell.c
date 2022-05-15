#include <windows.h>
#include <stdio.h>
#include "analysis.h"
void tiny_shell();

int main(int agvc, char *arg[]){
    tiny_shell();
    return 0;
}
void tiny_shell(){
    char *line;
    char **args;
    int status = 0;
    do
    {
        printf("tiny_shell> ");
        line = read_line();
        args = split_line(line);
        status = excute(args);
        free(line);
        free(args);
    } while (status);
}


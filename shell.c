#include <windows.h>
#include <stdio.h>
#include "analysis.h"
#include "process.h"
#include "builtin.h"

void tiny_shell();

int main(int agvc, char *arg[]){
    tiny_shell();
    return 0;
}
void tiny_shell(){
    char *line;
    char **args;
    int status = 0;
    char* current_dir = (char*)malloc(MAX_PATH);
    do
    {
        current_dir = (char*)get_current_dir();
        printf("(shell)%s>", current_dir);
        line = read_line();
        args = split_line(line);
        status = excute(args);
        free(line);
        free(args);
    } while (status);
}


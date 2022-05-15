#include<stdio.h>
#include<windows.h>

int help(char **args);
int exit(char **args);
int path(char **args);
int list_path();
int add_path(char **args);
int num_builtins();

int (*builtin_func[]) (char **args) = {
    &help,
    &exit,
    &path
};
char *builtin_str[] = {"help", "exit", "path"};
int num_builtins(){
    return sizeof(builtin_str)/sizeof(char *);
}
int path(char **args){
    if(args[1] == NULL){
        printf("too few args.\n");
        return 1;
    }
    if(strcmp(args[1], "-all") == 0){
        list_path();
    }else if(strcmp(args[1], "-add") == 0){
        add_path(args);
    }
    return 1;
}

int exit(char **args){
    return 0;
}
int help(char **args){
    printf("Tiny Shell\n");
    for(int i=0;i<num_builtins();i++){
        printf("\t%s\n", builtin_str[i]);
    }
    return 1;
}
int list_path(){
    LPTSTR var;
    LPCH env = GetEnvironmentStrings();
    if(env == NULL)
    {
        printf("no env\n");
    }else{
        var = (LPTSTR)env;
        int idx=0;
        while(*var){
            printf("%d: %s\n", ++idx, var);
            var += lstrlen(var) + 1;
        }
    }
    return 1;
}
int add_path(char **args){
    LPSTR lp_name, lp_value;
    if(args[2] != NULL && args[3] != NULL){
        lp_name = args[2];
        lp_value = args[3];
        if(SetEnvironmentVariable(lp_name, lp_value)){
            printf("added (%s: %s)\n", lp_name, lp_value);
            free(lp_name);
            free(lp_value);
        }else{
            printf("error\n");
        }
    }else{
        printf("too few args\n");
    }
    return 1;
}

#include<stdio.h>
#include<windows.h>

int help(char **args);
int exit(char **args);
int path(char **args);
int date(char **args);
int time(char **args);
int dir(char **args);
int list_path();
int add_path(char **args);
int num_builtins();

int (*builtin_func[]) (char **args) = { &help, &exit, &path, &date, &time, &dir};
char *builtin_str[] = {"help", "exit", "path", "date", "time", "dir"};
int num_builtins(){
    return sizeof(builtin_str)/sizeof(char *);
}
int date(char **args){
    if(args[1]!=NULL){
        printf("no option for date command.\n");
    }else{
        SYSTEMTIME st, lt;
        GetSystemTime(&st);
        GetLocalTime(&lt);
        printf(" UTC date: %02d/%02d/%04d\n", st.wDay, st.wMonth, st.wYear);
        printf(" Local date: %02d/%02d/%04d\n", lt.wDay, lt.wMonth, lt.wYear);
    }
    return 1;
}
int time(char **args){
    if(args[1]!=NULL){
        printf("no option for time command.\n");
    }else{
        SYSTEMTIME st, lt;
        GetSystemTime(&st);
        GetLocalTime(&lt);
        printf(" UTC time: %02d:%02d:%02d.%04d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        printf(" Local time: %02d:%02d:%02d.%04d\n", lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
    }
    return 1;
}
int dir(char **args){
    if(args[1]!=NULL){
        printf("no option for dir command.\n");
        return 1;
    }
    LPSTR cur_dir = (char *)calloc(MAX_PATH, sizeof(char));
    GetCurrentDirectory(MAX_PATH, cur_dir);
    cur_dir = strcat(cur_dir, "\\*");
    WIN32_FIND_DATA data;
    HANDLE h = FindFirstFile(cur_dir, &data);
	if (h != INVALID_HANDLE_VALUE){
        printf(" Directory of %s\n", cur_dir);
        while(FindNextFileA(h, &data)){
            if(data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
                printf("%9s %9s %s\n", "<DIR>", "", data.cFileName);
            else printf("%9s %9d %s\n", "", data.nFileSizeLow, data.cFileName);
        }
	}
    return 1;
}int path(char **args){
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
    if(args[1]!=NULL){
        printf("no option for exit command.\n");
        return 1;
    }
    return 0;
}
int help(char **args){
    if(args[1]!=NULL){
        printf("no option for help command.\n");
        return 1;
    }
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
            printf(" %d: %s\n", ++idx, var);
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
            printf(" added (%s: %s)\n", lp_name, lp_value);
            free(lp_name);
            free(lp_value);
        }else{
            printf(" error\n");
        }
    }else{
        printf("too few args\n");
    }
    return 1;
}

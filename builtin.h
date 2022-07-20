#pragma once
#include<stdio.h>
#include<windows.h>
#include "process.h"
#include <wchar.h>

LPSTR current_dir;

LPSTR get_current_dir();
int pc_op(char *pid, int (*op)(DWORD pid));
int exc(char **args);
int help(char **args);
int fexit(char **args);
int path(char **args);
int date(char **args);
int ftime(char **args);
int dir(char **args);
int pc(char **args);
int list_path();
int add_path(char **args);
int cd(char **args);
int num_builtins();

int (*builtin_func[]) (char **args) = {cd, pc, exc, path, date, ftime, dir, help, fexit};
char *builtin_str[] = {"cd", "pc", "exc",  "path", "date", "time", "dir", "help", "exit"};
char *daysOfWeek[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

int num_builtins(){
    return sizeof(builtin_str)/sizeof(char *);
}

LPSTR get_current_dir(){
    LPSTR current_dir = (char *)calloc(MAX_PATH, sizeof(char));
    GetCurrentDirectory(MAX_PATH, current_dir);
    return current_dir;
}

int date(char **args){
    if(args[1]!=NULL){
        printf(" no option for date command\n");
        return 1;
    }
    SYSTEMTIME st, lt;
    GetSystemTime(&st);
    GetLocalTime(&lt);
    printf(" UTC date: %s-%02d/%02d/%04d\n", daysOfWeek[st.wDayOfWeek], st.wDay, st.wMonth, st.wYear);
    printf(" Local date: %s-%02d/%02d/%04d\n", daysOfWeek[lt.wDayOfWeek], lt.wDay, lt.wMonth, lt.wYear);

    return 1;
}
int ftime(char **args){
    if(args[1]!=NULL){
        printf(" no option for time command\n");
        return 1;
    }
    SYSTEMTIME st, lt;
    GetSystemTime(&st);
    GetLocalTime(&lt);
    printf(" UTC time: %02d:%02d:%02d.%02d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    printf(" Local time: %02d:%02d:%02d.%02d\n", lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds);
    return 1;
}
int cd(char** args) {
    if (args[1] == NULL) return 1;
    char* t = (char*)calloc(MAX_PATH, sizeof(char));
    t = strcat(t, args[1]);
    if (SetCurrentDirectory(t) == 0) {
        printf(" cannot find the path specified\n");
    }
    free(t);
    return 1;
}
int dir(char **args){
    if(args[1]!=NULL){
        printf(" no option for dir command\n");
        return 1;
    }
    LPSTR path = (LPSTR)calloc(MAX_PATH, sizeof(char));
    path = strcat(get_current_dir(), "//*");
    WIN32_FIND_DATA data;
    HANDLE h = FindFirstFile(path, &data);
	if (h != INVALID_HANDLE_VALUE){
        printf("\n Directory of %s\n\n", get_current_dir());
        SYSTEMTIME st = { 0 };
        LPFILETIME systemTime;
        LPFILETIME localTime;
        do{
            systemTime = &data.ftLastWriteTime;
            FileTimeToLocalFileTime(systemTime, localTime);
            FileTimeToSystemTime(&data.ftLastWriteTime, &st);
            if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                printf("%02d/%02d/%d  %02d:%02d %s %9s %12s %s\n", st.wDay, st.wMonth, st.wYear, st.wHour%12, st.wMinute, (st.wHour>=12)?"PM":"AM", "<DIR>", "", data.cFileName);
            else {
                DWORD64 fileSize = ((DWORD64)data.nFileSizeHigh<<32) | data.nFileSizeLow;
                printf("%02d/%02d/%d  %02d:%02d %s %9s %12llu %s\n", st.wDay, st.wMonth, st.wYear, st.wHour % 12, st.wMinute, (st.wHour >= 12) ? "PM" : "AM", "", fileSize, data.cFileName);
            }
        } while (FindNextFileA(h, &data));
	}
    printf("\n");
    CloseHandle(h);
    return 1;
}
int path(char **args){
    if(args[1] == NULL){
        printf(" too few args\n");
        return 1;
    }
    if(strcmp(args[1], "-all") == 0){
        list_path();
    }else if(strcmp(args[1], "-add") == 0){
        add_path(args);
    }
    return 1;
}
int fexit(char **args){
    if(args[1]!=NULL){
        printf(" no option for exit command\n");
        return 1;
    }
    DWORD current_id = GetCurrentProcessId();
    HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
    do{
    	if(processEntry.th32ParentProcessID == current_id)
            killBgProcess(processEntry.th32ProcessID);
    }while (Process32Next(hProcessSnapshot, &processEntry));
    CloseHandle(hProcessSnapshot);
    return 0;
}
int help(char **args){
    if(args[1]!=NULL){
        printf(" no option for help command\n");
        return 1;
    }
    printf("Tiny Shell\n");
    for(int i=0;i<num_builtins();i++){
        printf("\t%s\n", builtin_str[i]);
    }
    return 1;
}
int exc(char **args){
    if(args[2]!=NULL){
        printf(" no option for execute command\n");
        return 1;
    }
    char *file_name = (char *)malloc(256);
    file_name = strcpy(file_name, args[1]);
    char *file_ext = strrchr(args[1], '.');
    if(strcmp(file_ext, ".bat") != 0 || file_ext == NULL){
        printf(" can only excute batch file\n");
        return 1;
    }
    char* full = (char*)calloc(MAX_PATH, sizeof(char));
    file_name = _fullpath(full, file_name, 256); 
    FILE *fp;
    fp = fopen(args[1], "r");
    if(fp == NULL){
        printf(" can't open or doesn't exist %s\n", args[1]);
        return 1;
    }
    ShellExecuteA(NULL, "open", file_name, NULL, NULL, SW_SHOW);
    fclose(fp);
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
    free(var);
    return 1;
}
int add_path(char **args){
    LPSTR lp_name, lp_value;
    if(args[2] == NULL || args[3] == NULL || args[4] != NULL){
        printf(" path -add <name> <value>\n");
    }else {
        lp_name = args[2];
        lp_value = args[3];
        if(SetEnvironmentVariable(lp_name, lp_value)){
            printf(" added (%s: %s)\n", lp_name, lp_value);
        }else{
            printf(" error\n");
        }
    }
    free(lp_name);
    free(lp_value);
    return 1;
}

int pc_op(char *pid, int (*op)(DWORD pid)){
    for(int i=0;i<strlen(pid);i++){
        if((pid[i]<'0' || pid[i]>'9'))
            return -2;
    }
    int _pid = atoi(pid);
    if(_pid < 0)
        return -1;
    return op((DWORD)_pid);
}

int pc(char **args){
    int flag;
    char *op = (char *)malloc(15);
    if(args[1] == NULL){
        printf(" too few args\n");
        return 1;
    }
    if(args[2] == NULL){
        if(strcmp(args[1], "-all") == 0){
            if(listAllProcess() == -1){
                printf(" can't get any process\n");
            }
        }else printf(" syntax error\n");
    } else if(args[3] == NULL && args[2] != NULL){
        if(strcmp(args[1], "-child") == 0){
            flag = pc_op(args[2], listChildProcess);
            strcpy(op, "get child");
        }else if(strcmp(args[1], "-kill") == 0){
            flag = pc_op(args[2], killBgProcess);
            strcpy(op, "kill");
        }else if(strcmp(args[1], "-stop") == 0){
            flag = pc_op(args[2], stopBgProcess);
            strcpy(op, "stop");
        }else if(strcmp(args[1], "-resume") == 0){
            flag = pc_op(args[2], resumeBgProcess);
            strcpy(op, "resume");
        }else if (strcmp(args[1], "-fg") == 0 || strcmp(args[1], "-bg") == 0){
            DWORD pid = createProcess(args);
            if(pid == -1){
               printf(" can't create new process\n");
            }
        }   
        if(flag == -1)
            printf(" can't %s process %d\n", op, atoi(args[2]));
        else if(flag == -2) 
            printf(" pid is of type USIGNED INT\n");
        
    }else printf(" syntax error\n");
    return 1;
}

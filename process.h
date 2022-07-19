#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>
#include <sys/types.h>

DWORD inputHandleThread();
DWORD createProcess(char **args);
int listAllProcess();
int resumeBgProcess(DWORD pid);
int listChildProcess(DWORD pid);
int killBgProcess(DWORD pid);
int stopBgProcess(DWORD pid);

int listAllProcess(){
	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(hProcessSnapshot,&processEntry))	return -1;
	
	printf("%-15s %-15s %s\n", "[ParentID]", "[Process ID]", "[Process Name]");
    do{
    	printf("%-15d %-15d %s\n", processEntry.th32ParentProcessID, processEntry.th32ProcessID, processEntry.szExeFile);
    }while (Process32Next(hProcessSnapshot, &processEntry));
    CloseHandle(hProcessSnapshot);
	return 1;
}
int listChildProcess(DWORD pid){ // pc -child parentId
	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(hProcessSnapshot,&processEntry))	return -1;
    printf(" %-15s %s\n", "[Process ID]", "[Process Name]");  
    do{
        if(processEntry.th32ParentProcessID == pid)
    	 printf(" %-15d %s\n", processEntry.th32ProcessID, processEntry.szExeFile);
    }while (Process32Next(hProcessSnapshot, &processEntry));
    CloseHandle(hProcessSnapshot);
	return 1;
}

DWORD createProcess(char **args)
{
    DWORD waitTime;
    if(strcmp(args[1],"-bg") == 0){
        waitTime = 0;
    }else if(strcmp(args[1],"-fg") == 0){
        waitTime = INFINITE;
    }
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    if(!CreateProcess(NULL, args[2], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)){
        return -1;
    }
    DWORD pid = pi.dwProcessId;
    if(strcmp(args[1],"-bg") == 0){
        WaitForSingleObject(pi.hProcess, 0);
    }
    else if(strcmp(args[1],"-fg") == 0){
        DWORD id;
        HANDLE handles[2];
        handles[0] = pi.hProcess;
        handles[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)inputHandleThread, NULL, 0, &id);
        WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        killBgProcess(pid);
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return pid;
}

int killBgProcess(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!TerminateProcess(hProcess, 0) || hProcess == NULL) return -1;
    CloseHandle(hProcess);
    return 1;
}

int stopBgProcess(DWORD pid){
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 threadEntry; 
    threadEntry.dwSize = sizeof(THREADENTRY32);
    if (!Thread32First(hThreadSnapshot, &threadEntry)){ 
		CloseHandle(hThreadSnapshot);          
		return -1;
    }
    do{
        if (threadEntry.th32OwnerProcessID == pid){
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
            SuspendThread(hThread);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));

    CloseHandle(hThreadSnapshot);
    return 1;
}
int resumeBgProcess(DWORD pid){
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 threadEntry; 
    threadEntry.dwSize = sizeof(THREADENTRY32);
    if (!Thread32First(hThreadSnapshot, &threadEntry)){ 
		CloseHandle(hThreadSnapshot);          
		return -1;
    }
    do{
        if (threadEntry.th32OwnerProcessID == pid){
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);
            if(!ResumeThread(hThread)) return -1;
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));

    CloseHandle(hThreadSnapshot);
    return 1;
}
DWORD inputHandleThread(){
    DWORD cNumRead;
    INPUT_RECORD buffer[128];
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    while(TRUE){
        if (!ReadConsoleInput(hStdin, buffer, 128, &cNumRead))
            return 1;
        for (int i=0;i<cNumRead;i++){
            if (buffer[i].EventType == KEY_EVENT){
                KEY_EVENT_RECORD ker = buffer[i].Event.KeyEvent;
                if (ker.bKeyDown && ker.wVirtualKeyCode == 27) return 1;
                break;
            }
        }
    }
}

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <tchar.h>
#include <wchar.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <sys/types.h>

DWORD createProcess(char **args);
BOOL resumeBgProcess();

int listAllProcess();
int listChildProcess(DWORD pid);
int killBgProcess(DWORD pid);
int stopBgProcess(DWORD pid);

int listAllProcess(){
	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(hProcessSnapshot,&processEntry))	
	{
		return -1;
	}
	
	printf(" %-15s %s\n", "[Process ID]", "[Process Name]");  
    do{
    	printf(" %-15d %s\n", processEntry.th32ProcessID, processEntry.szExeFile);
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
    DWORD tmp = pi.dwProcessId;
    WaitForSingleObject(pi.hProcess,waitTime);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return tmp;
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


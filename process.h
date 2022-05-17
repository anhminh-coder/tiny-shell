#include <stdio.h>
#include <bits/stdc++.h>
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
int killProcessID(DWORD process_id);
int StopBgProcess(DWORD process_id);
BOOL ResumeBgProcess();
BOOL GetProcessListAll(char *args,DWORD pid);

using namespace std;

DWORD createProcess(char **args)
{
    DWORD waitTime;
    if(strcmp(args[1],"-bg")){
        waitTime = INFINITE;
    }else if(strcmp(args[1],"-fg")){
        waitTime = 0;
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
    //TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return tmp;
}

int killProcessID(DWORD process_id) {
    HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    if (hprocess == NULL){
        cout << "ERROR: Failed!" << endl;
        return 1;
    }
    if (!TerminateProcess(hprocess, 0)) {
        return 0;
    }
    CloseHandle(hprocess);
    return 1;
}

int StopBgProcess(DWORD process_id)
	{
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 threadEntry; 
    threadEntry.dwSize = sizeof(THREADENTRY32);
    if (!Thread32First(hThreadSnapshot, &threadEntry)) 
	{
		cout << "Thread32First Fail " <<  GetLastError(); 
		CloseHandle(hThreadSnapshot);          
		return 0;
    }

    do
    {
        if (threadEntry.th32OwnerProcessID == process_id)
        {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE,
                threadEntry.th32ThreadID);
            
            SuspendThread(hThread);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));

    CloseHandle(hThreadSnapshot);
    return 1;
}


BOOL ResumeBgProcess(){

}


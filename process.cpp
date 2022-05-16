#include <bits/stdc++.h>
#include <stdio.h>
#include<stdlib.h>
#include <windows.h>
#include <time.h>
#include <tchar.h>
#include <wchar.h>
#include <psapi.h>
#include "process.h"
#include <tlhelp32.h>
#include <sys/types.h>

using namespace std;

DWORD CreateAProcess(char **args)
{
    DWORD tmp;
    int waitTime;
    if(args[1] == NULL)
    {
        printf("Too few argument\n");
        return EXIT_FAILURE;
    }

    if(strcmp(args[1],"-bg"))
    {
        waitTime = 0;
    }
    else if(strcmp(args[1],"-fg"))
    {
        waitTime = INFINITE;
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    CreateProcess(NULL,
                   args[1],
                   NULL,
                   NULL,
                   FALSE,
                   CREATE_NEW_CONSOLE,
                   NULL,
                   NULL,
                   &si,
                   &pi);
    tmp = pi.dwProcessId;
    WaitForSingleObject(pi.hProcess,waitTime);
    //TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return tmp;

}

int killProcessID(DWORD process_id) {
    HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);

    if (hprocess == NULL) {
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
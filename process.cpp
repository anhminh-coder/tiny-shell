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

void ShowDate()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("%02d-%02d-%d\n",tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

void ShowTime()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("%02d:%02d:%02d\n",tm.tm_hour, tm.tm_min, tm.tm_sec);
}

//BOOL GetProcessListAll(char *args,DWORD pid)
//{
//	printf("%-50s%-20s\n", "Process Name", "Process ID");
//	printf("%-50s%-20s\n", "----------------------------------", "----------");
//	printf("%-50s%-20d%\n", pe32.szExeFile, pe32.th32ProcessID);
//	return(TRUE);
//}

BOOL ShowHelp(){
    printf("\t\t\nHelp");
    printf("\t\nhelp: Show help");
    printf("\t\ndate: Show date");
    printf("\t\ntime: Show time");
    printf("\t\n -bg: Create process in background mode");
    printf("\t\n -fg: Create process in foreground mode");
    printf("\t\nlist: Show the list of process");
    printf("\t\nkill: Terminate a process");
    printf("\t\nstop: Stop a process");
    printf("\n\n");
}


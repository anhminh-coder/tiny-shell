DWORD CreateAProcess(char **args);
int killProcessID(DWORD process_id);
int StopBgProcess(DWORD process_id);
BOOL ResumeBgProcess();
BOOL GetProcessListAll(char *args,DWORD pid);
BOOL ShowHelp();
void ShowDate();
void ShowTime();


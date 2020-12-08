#include <windows.h>
#include <stdio.h>

DWORD Sum;

DWORD WINAPI Summation(LPVOID Param)
{
    DWORD Upper = *(DWORD*)Param;
    for(DWORD i = 1; i<=Upper;i++) Sum += i;
    return 0;
}

int main(int argc, char *argv[])
{
    DWORD ThreadId;
    HANDLE ThreadHandle;
    int Param;

    Param = atoi(argv[1]);

    ThreadHandle = CreateThread(
        NULL, //default security attributes.
        0, //Default stack size
        Summation, //Thread function
        &Param, //param to thread function
        0, //deafualt creation flag
        &ThreadId //return the thread identifier 
    );
    WaitForSingleObject(ThreadHandle, INFINITE);

    printf("sum = %d\n", Sum);

    return 0;
}

#include <windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    HANDLE hFile, hMapFile;
    LPVOID lpMapAddress;

    hFile = CreateFile("temp.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, TEXT("SharedObject"));

    lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0,0,0);

    printf("%p\n", lpMapAddress);

    sprintf(lpMapAddress, "Shared memory message");
    printf("%s\n", lpMapAddress);

    //UnmapViewOfFile(lpMapAddress);
    CloseHandle(hFile);
    CloseHandle(hMapFile);
    while(1) ;
    printf("Done\b");
    return 0;
}

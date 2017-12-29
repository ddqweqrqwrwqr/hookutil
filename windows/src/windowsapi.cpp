#include "windowsapi.h"

namespace windows
{
//1.首先定义一个结构：
typedef struct tagWNDINFO
{
    DWORD dwProcessId;
    HWND hWnd;
} WNDINFO, *LPWNDINFO;

//2.枚举所有进程
BOOL CALLBACK MyEnumProc(HWND hWnd, LPARAM lParam) //枚举所有进程
{
    DWORD dwProcId;
    GetWindowThreadProcessId(hWnd, &dwProcId);
    LPWNDINFO pInfo = (LPWNDINFO)lParam;
    if (dwProcId == pInfo->dwProcessId)
    {
        pInfo->hWnd = hWnd;
        return FALSE;
    }

    return TRUE;
}

//3.获取给定进程ID的窗口handle

HWND GetProcessMainWnd(DWORD dwProcessId) //获取给定进程ID的窗口handle
{
    WNDINFO wi;
    wi.dwProcessId = dwProcessId;
    wi.hWnd = NULL;
    EnumWindows(MyEnumProc, (LPARAM)&wi);

    return wi.hWnd;
}
HWND GetWndHwnd(const char *strExeName)
{
    PROCESSENTRY32 pe32;
    //在使用这个结构前，先设置它的大小
    pe32.dwSize = sizeof(pe32);
    //给系统内所有的进程拍个快照
    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        //printf("CreateToolhelp32Snapshot 调用失败.\n");
        return NULL;
    }
    //遍历进程快照，轮流显示每个进程的信息
    BOOL bMore = ::Process32First(hProcessSnap, &pe32);
    while (bMore)
    {
        //if(pe32.szExeFile == exeName)//找到了进程
        //cout << strExeName << endl;
        printf("%S\n", pe32.szExeFile);
        if (_tcscmp(pe32.szExeFile, strExeName) == 0)
        {
            HWND game_window_hwnd = GetProcessMainWnd(pe32.th32ProcessID);
            if (NULL != game_window_hwnd)
            {
                ::CloseHandle(hProcessSnap);
                return game_window_hwnd;
            }
        }
        bMore = ::Process32Next(hProcessSnap, &pe32);
    }
    //不要忘记清除掉snapshot对象
    ::CloseHandle(hProcessSnap);
    return NULL;
}
LPVOID MyReadProcessMemory(int hWnd,int point,SIZE_T nSize)
{
	LPDWORD hProcId = (LPDWORD)malloc(sizeof(LPDWORD));
	GetWindowThreadProcessId((HWND)hWnd, hProcId);
	void *hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, *hProcId);
	LPVOID eax;
	ReadProcessMemory(hProcess, LPCVOID(point), &eax, nSize, 0);
	free(hProcId);
	return eax;
}
}
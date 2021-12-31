#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

#include <wtypes.h>
#include <WinUser.h>
#include <TlHelp32.h>

const char *targetProcessName = "F8Tray.exe";
const char *targetModuleName = "fwatermarksc.dll";
// 代码偏移
int lpBaseAddress = 0x1CDE3;

DWORD GetProcessID(const char *lpName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) {
        return NULL;
    }
    PROCESSENTRY32 processEntry = {sizeof(processEntry)};
    BOOL fOk;
    for (fOk = Process32First(hSnapshot, &processEntry); fOk; fOk = Process32Next(hSnapshot, &processEntry)) {
        if (strcmp(processEntry.szExeFile, lpName) == 0) {
            CloseHandle(hSnapshot);
            return processEntry.th32ProcessID;
        }
    }
    return NULL;
}

HMODULE GetProcessModuleHandle(DWORD pid, CONST CHAR *moduleName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (INVALID_HANDLE_VALUE == hSnapshot) {
        return nullptr;
    }
    MODULEENTRY32 moduleEntry = {sizeof(moduleEntry)};
    BOOL fOk;
    for (fOk = Module32First(hSnapshot, &moduleEntry); fOk; fOk = Module32Next(hSnapshot, &moduleEntry)) {
        if (strcmp(moduleEntry.szModule, moduleName) == 0) {
            CloseHandle(hSnapshot);
            return moduleEntry.hModule;
        }
    }
    return nullptr;
}

int main() {
    // 获取进程编号
    DWORD processID = GetProcessID(targetProcessName);
    // 获取进程句柄
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (processHandle == nullptr) {
        ::MessageBox(nullptr, "ERROR: Process not found.", "WatermarkCracker", MB_ICONERROR);
        return 0;
    }
    // 取得水印模块基址
    HMODULE moduleBaseAddress = GetProcessModuleHandle(processID, targetModuleName);
    // 代码地址
    int codeAddress = (int) moduleBaseAddress + lpBaseAddress;
    // NOP
    BYTE shellData[5] = {0x90, 0x90, 0x90, 0x90, 0x90};
    SIZE_T lpNumberOfBytesWritten = 0;
    WriteProcessMemory(processHandle, (LPVOID) codeAddress, &shellData, sizeof(shellData), &lpNumberOfBytesWritten);
    CloseHandle(processHandle);
    ::MessageBox(nullptr, "Success", "WatermarkCracker", MB_ICONINFORMATION);
    return 0;
}

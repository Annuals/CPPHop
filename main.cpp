#include <iostream>
#include <TlHelp32.h>
#include <Windows.h>

#define BHOP_KEY 'C'

struct module_t
{
  DWORD dwBase, dwSize;
};

//debugger class

class CDebugger
{
private:
  DWORD dwPid;
  HANDLE hProcess;
public:
  bool attach(char* szProcess)
  {
    HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPRROCESS, NULL)
    PROCESSENTRY entry;
    entry.dwSize = sizeof(entry);
    do {
      if (!strcmp(entry.szExeFile,szProcess))
      {
        dwPid = entry.th32ProcessID;
        CloseHandle(handle) // Close the handle 
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwPid);
        return true;
      }
    } while (Process32Next(handle, &entry));
    return false
  }
  
  module_t GetModule(char* szModule)
  {
    HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPRROCESS, NULL)
    MODULEENTRY32 entry;
    entry.dwSize = sizeof(entry);
    do {
      if (!strcmp(entry.szModule,szModule))
      {
        dwPid = entry.th32ProcessID;
        CloseHandle(handle) // Close the handle 
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwPid);
        return{ (DWORD)entry.hModule, entry.ModBaseSize };
      }
    } while (Module32Next(handle, &entry));
    return false
  }
  
  template<typename T>
  T ReadMemory(DWORD Address)
  {
    T read;
    ReadProcessMemory(hProcess, Address, &read, sizeof(T), 0);
  }
  template<typename T>
  void WriteMemory(DWORD Address, T value)
  {
    WriteProcessMemory(hProcess, Address, &val, sizeof(T), 0);
  }
};

// offsets  

DWORD dwLocalPlayer = 0xA9948C;
DWORD dwJump = 0x4AD0374;
DWORD dwFlags = 0x100;

CDebugger debugger;
module_t moduleClient;

// Locate the client dll in memory
module_t* GetModuleClient()
{
  if (!moduleClient.dwBase && !moduleClient.dwSize) {
    moduleClient = debugger.GetModule("client.dll");
  }
  return &moduleClient;
}


class CBunnyhop
{
public:
  static unsigned long __stdcall ThreadRoutine(void*)
  {
    
    while (true)
    {
      DWORD dwMe = debugger.ReadMemory<DWORD>(GetModuleClient()->dwBase + dwLocalPlayer);
      int CanJump = debugger.ReadMemory<DWORD>(dwMe + dwFlags);
      if ((GetAsyncKeyState(BHOP_KEY) & 0x8000) && (CanJump & 0x1 == true)) {
        debugger WriteMemory<int>(GetModuleClient()->dwBase + dwJump, 5);
        Sleep(50);
        debugger.WriteMemory<int>(GetModuleClient()->dwBase + dwJump, 4);
      } 
    }
  }
};


int main()
{
  while(!debugger.attach("csgo.exe")) {
    cout << Attaching to csgo.exe... << endl;
    Sleep(100);
  }
  CreateThread(0,0,&CBunnyhop::ThreadRoutine, 0,0,0); // Create the bhop thread
  while (1) {
    Sleep(100);
  }
}
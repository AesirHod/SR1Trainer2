// Based on code by Jared Bruni http://www.lostsidedead.com/gameprog
// Additional functions and adaptions added by Vampmaster

#ifndef INC_PROCESS_H
#define INC_PROCESS_H

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>

extern void findMe();

class CProcess 
{
public:
	CProcess();
	CProcess(char *p_WindowTitle);
	virtual ~CProcess();
	BOOL IsRunning();
	BOOL FindProcess(char *p_WindowTitle);
	BOOL FindProcess();
	BOOL Reset();
	BOOL WasUpdated();
	BOOL IsKeyPressed(int p_iKey);
	BOOL IsSpyRunning(char *p_WindowTitle);
	DWORD Alloc(int bytes);
	DWORD Free(DWORD address);
	template<class TData>
	BOOL Write(DWORD p_Address, TData data);
	template<class TData>
	BOOL Write(DWORD p_Address, TData* data, DWORD num);
	BYTE* WriteXString(DWORD p_Address, char* hexStr);
	DWORD WriteString(DWORD p_Address, char* ascStr);
	template<class TData>
	TData Read(TData* p_Address);
	template<class TData>
	TData* Read(TData* p_Address, DWORD size);
	char* CmpString(DWORD p_Address, char* ascStr);
	DWORD AddrDiff(DWORD origin, DWORD dest);
	BOOL WriteDiff(DWORD origin, DWORD dest);

private:
	HANDLE OpenProcess(char *p_ClassName, char *p_WindowTitle);
	HWND hWindow;
	HWND pWindow;
	BOOL updated;
	HANDLE m_hProcess;
	BOOL m_bGameRunning;
	char *windowTitle;
};

#endif
/*
CProcess::CProcess()
{
	m_hProcess = NULL;
	m_bGameRunning = false;
	windowTitle = NULL;
}

CProcess::CProcess(char *p_WindowTitle)
{
	m_hProcess = NULL;
	m_bGameRunning = false;
	this->FindProcess(p_WindowTitle);
}

CProcess::~CProcess()
{
	if (m_bGameRunning)
	{
		CloseHandle(m_hProcess);
	}
}

BOOL CProcess::WasUpdated()
{
	return (pWindow && hWindow != pWindow);
}

BOOL CProcess::Reset()
{
	m_hProcess = NULL;
	m_bGameRunning = false;
	return true;
}

HANDLE CProcess::OpenProcess(char *p_ClassName, char *p_WindowTitle)
{
	DWORD pid; // Process ID
	pWindow = hWindow;
	hWindow = FindWindow(p_ClassName, p_WindowTitle);
	if (hWindow)
	{
		GetWindowThreadProcessId(hWindow, &pid);
		return ::OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	}
	return NULL;
}


BOOL CProcess::FindProcess(char *p_WindowTitle)
{
	windowTitle = p_WindowTitle;
	return this->FindProcess();
}

BOOL CProcess::FindProcess()
{
	m_hProcess = this->OpenProcess(NULL, windowTitle);
	if (m_hProcess)	m_bGameRunning = true;
	else m_bGameRunning = false;
	return m_bGameRunning;
}*/

/*
char* CProcess::CmpString(DWORD p_Address, char* ascStr)
{
	if (!m_bGameRunning)
		return false;
	DWORD charNo = strlen(ascStr);
	char* retStr = (char*) this->Read((char*)p_Address, charNo);
	if (m_bGameRunning && retStr && !strncmp(retStr, ascStr, charNo)) return ascStr;
	return false;
}

BOOL CProcess::IsRunning()
{
	return m_bGameRunning;
}

BOOL CProcess::IsKeyPressed(int iKey)
{
	return ((GetAsyncKeyState(iKey) & 1) == 1);
}

BOOL CProcess::IsSpyRunning(char *p_WindowTitle)
{
	HANDLE hTmp = this->OpenProcess(NULL, "TRAINER SPY");
	if (hTmp)
	{
		CloseHandle(hTmp);
		return true;
	}
	return false;
}

BYTE* CProcess::WriteXString(DWORD p_Address, char* hexStr)
{
	DWORD strSize = strlen(hexStr);										// Size of string

	if (!m_bGameRunning || (strSize <= 0)) return false;				// 0 length or no game

	DWORD dataSize = 0;													// No of bytes to be written
	BYTE* array = new BYTE[strSize/2];									// 2 chars = 1 byte

	for(DWORD i=0; i<strSize; i++)										// For every character
	{
		if (!hexStr[i]) break;											// Maybe that will fix the bug?
		if (!isxdigit(hexStr[i])) continue;								// If it's not hex look at next one
		sscanf(&hexStr[i++], "%02x", &array[dataSize++]);				// If it's hex scan for a byte
		if (!hexStr[i]) break;											// Maybe that will fix the bug?
	}																	// Chance of dataSize being too big???

	if (this->WriteArray(p_Address, array, dataSize))
		return array;
	return false;
}

DWORD CProcess::WriteString(DWORD p_Address, char* ascStr)
{
	DWORD strSize = strlen(ascStr);
	if (strSize && this->WriteArray(p_Address, ascStr, 1+strSize))		// Add 1 to include the null?
		return p_Address;
	return false;
}

DWORD CProcess::Alloc(int bytes)
{
	return (DWORD) VirtualAllocEx(m_hProcess, 0, bytes, MEM_COMMIT, PAGE_READWRITE);
	return 0;
}

DWORD CProcess::Free(DWORD address)
{
	return (DWORD) VirtualFreeEx(m_hProcess, 0, 0, MEM_RELEASE);
	return 0;
}
*/
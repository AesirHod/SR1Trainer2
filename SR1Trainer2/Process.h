// Based on code by Jared Bruni http://www.lostsidedead.com/gameprog
// Additional functions and adaptions added by Vampmaster

#ifndef INC_PROCESS_H
#define INC_PROCESS_H

#include <windows.h>

#include "Types.h"

class CScan;

class CProcess 
{
public:
	CProcess();
	CProcess(char* pszWindowTitle);
	virtual ~CProcess();

	BOOL IsRunning();
	BOOL FindProcess(char* pszWindowTitle);
	BOOL FindProcess();
	BOOL Reset();
	BOOL WasUpdated();

	BOOL IsKeyPressed(int iKey);
	u_int Alloc(u_int uBytes);
	u_int Free(u_int uAddress);
	u_int InjectDll(char* szDllName);

	HANDLE GetProcess() { return m_hProcess; }
	const char* GetWindowTitle() { return m_pszWindowTitle; }

	template<class TData>
	BOOL Write(u_int pxAddress, TData data);
	template<class TData>
	BOOL Write(u_int pxAddress, TData* data, u_int size);
	BOOL WriteXString(u_int uAddress, char* szHexStr);
	u_int WriteString (u_int uAddress, char* szAscStr);
	template<class TData>
	TData Read(TData* pxAddress);
	template<class TData>
	TData* Read(TData* pxAddress, u_int uSize);
	template<class TData>
	BOOL CmpData(u_int uAddress, TData xTestData);
	char* CmpString(u_int uAddress, char* szAscStr);
	u_int AddrDiff(u_int uOrigin, u_int uDest);
	BOOL WriteDiff(u_int uOrigin, u_int uDest);
	u_int SizeOfLast();

private:
	HANDLE OpenProcess(char* pszClassName, char* pszWindowTitle);
	HWND m_hWindow;
	HWND m_hPrevWindow;
	BOOL m_bUpdated;
	HANDLE m_hProcess;
	BOOL m_bGameRunning;
	char* m_pszWindowTitle;
	u_long m_ulBytesWritten;
};

#endif
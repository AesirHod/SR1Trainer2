#include "process.h"

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
	BOOL ud = updated;
	updated = false;
	return ud;
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
	hWindow = FindWindow(p_ClassName, p_WindowTitle);
    if (pWindow != hWindow) updated = true;
	pWindow = hWindow;
	if (hWindow)
	{
		GetWindowThreadProcessId(hWindow, &pid);
		return ::OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	}
	return NULL;
}

BOOL CProcess::FindProcess()
{
	m_hProcess = this->OpenProcess(NULL, windowTitle);
	if (m_hProcess)	m_bGameRunning = true;
	else m_bGameRunning = false;
	return m_bGameRunning;
}

BOOL CProcess::FindProcess(char *p_WindowTitle)
{
	windowTitle = p_WindowTitle;
	return this->FindProcess();
}


template<class TData>
BOOL CProcess::Write(DWORD p_Address, TData data)
{
	if (!m_bGameRunning)
		return false;
	DWORD BytesRead = 0;
	return WriteProcessMemory(m_hProcess,(void*)p_Address,
		(void*)&data,sizeof(data),&BytesRead);
}

template<class TData>
BOOL CProcess::Write(DWORD p_Address, TData* data, DWORD byteNo)
{
	if (!(m_bGameRunning && byteNo))
		return false;
	DWORD BytesRead = 0;
	return WriteProcessMemory(m_hProcess,(void*)p_Address,
		(void*)data,byteNo*sizeof(data[0]),&BytesRead);
}
template<class TData>
TData CProcess::Read(TData* p_Address)
{
	DWORD BytesRead = 0;
	TData retVal;

	if (m_bGameRunning)
	{
		if (!ReadProcessMemory(m_hProcess, (void*)p_Address,
			(void *)&retVal, sizeof(retVal), &BytesRead)) return 0;
		return retVal;
	}
	return false;
}

template<class TData>
TData* CProcess::Read(TData* p_Address, DWORD size)
{
    DWORD bytes;
	TData* retVal = new TData[size];
	if (m_bGameRunning)
	{
		if (!ReadProcessMemory(m_hProcess, (void*)p_Address,
			(void *)retVal, size*sizeof(*retVal), &bytes)) return 0;
		return retVal;
	}
	return false;
}

BYTE* CProcess::WriteXString(DWORD p_Address, char* hexStr)				// In dire need of reworking
{
	DWORD strSize = strlen(hexStr);										// Size of string

	if (!m_bGameRunning || (strSize <= 0)) return false;				// 0 length or no game

	DWORD dataSize = 0;													// No of bytes to be written
	BYTE* array = new BYTE[1+(strSize/2)];								// 2 chars = 1 byte

	while (*hexStr) {													// For every character
		if (isxdigit(*hexStr)) {
			if (isxdigit(hexStr[1])) {
				sscanf(hexStr++, "%02x", (DWORD*) &array[dataSize++]);	// If it's hex scan for a byte
			}
		}
		hexStr++;
	}

	if (this->Write(p_Address, array, dataSize))
		return array;
	return false;
}

DWORD CProcess::WriteString(DWORD p_Address, char* ascStr)
{
	DWORD strSize = strlen(ascStr);
	if (strSize && this->Write(p_Address, ascStr, 1+strSize))		// Add 1 to include the null?
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
char* CProcess::CmpString(DWORD p_Address, char* ascStr)
{
	if (!m_bGameRunning) return false;
	DWORD charNo = strlen(ascStr);
	char* retStr = (char*) this->Read((char*)p_Address, charNo);
	if (m_bGameRunning && retStr && !strncmp(retStr, ascStr, charNo)) return ascStr;
	return false;
}

DWORD CProcess::AddrDiff(DWORD origin, DWORD dest)
{
	return dest-(origin+0x04);
}

BOOL CProcess::WriteDiff(DWORD origin, DWORD dest)
{
	if (this->Write(origin, this->AddrDiff(origin, dest))) return true;
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

template BOOL	CProcess::Write		<UINT>	(DWORD	p_Address, UINT		data);
template BOOL	CProcess::Write		<INT>	(DWORD	p_Address, INT		data);
template BOOL	CProcess::Write		<BYTE>	(DWORD	p_Address, BYTE		data);
template BOOL	CProcess::Write		<DWORD>	(DWORD	p_Address, DWORD	data);
template BOOL	CProcess::Write		<WORD>	(DWORD	p_Address, WORD		data);
template BOOL	CProcess::Write		<DWORD>	(DWORD	p_Address, DWORD*	data,	DWORD byteNo);
template BYTE	CProcess::Read		<BYTE>	(BYTE*	p_Address);
template DWORD	CProcess::Read		<DWORD>	(DWORD*	p_Address);
template BYTE*	CProcess::Read		<BYTE>	(BYTE*	p_Address, DWORD	size);
template DWORD*	CProcess::Read		<DWORD>	(DWORD*	p_Address, DWORD	size);
/*
template<class TData>
TData CProcess::Read(TData* p_Address)
{
	DWORD BytesRead = 0;
	TData retVal;

	if (m_bGameRunning)
	{
		if (!ReadProcessMemory(m_hProcess, (void*)p_Address,
			(void *)&retVal, sizeof(retVal), &BytesRead)) return 0;
		return retVal;
	}
	return false;
}

template<class TData>
TData* CProcess::Read(TData* p_Address, DWORD size)
{
    DWORD bytes;
	TData* retVal = new TData[size];
	if (m_bGameRunning)
	{
		if (!ReadProcessMemory(m_hProcess, (void*)p_Address,
			(void *)retVal, size*sizeof(*retVal), &bytes)) return 0;
		return retVal;
	}
	return false;
}
*/

/*class CProcess 
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
	BOOL WriteArray(DWORD p_Address, TData* data, DWORD num);
	BYTE* WriteXString(DWORD p_Address, char* hexStr);
	DWORD WriteString(DWORD p_Address, char* ascStr);
	template<class TData>
	TData Read(TData* p_Address);
	template<class TData>
	TData* Read(TData* p_Address, DWORD size);
	char* CmpString(DWORD p_Address, char* ascStr);

private:
	HANDLE OpenProcess(char *p_ClassName, char *p_WindowTitle);
	HWND hWindow;
	HWND pWindow;
	HANDLE m_hProcess;
	BOOL m_bGameRunning;
	char *windowTitle;

};
*/
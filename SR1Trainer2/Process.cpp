#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>

#include "process.h"

CProcess::CProcess()
{
	m_hWindow = NULL;
	m_hPrevWindow = NULL;
	m_bUpdated = false;
	m_hProcess = NULL;
	m_bGameRunning = false;
	m_pszWindowTitle = NULL;
	m_ulBytesWritten = 0;
}

CProcess::CProcess(char* pszWindowTitle)
{
	m_hWindow = NULL;
	m_hPrevWindow = NULL;
	m_bUpdated = false;
	m_hProcess = NULL;
	m_bGameRunning = false;
	this->FindProcess(pszWindowTitle);
	m_ulBytesWritten = 0;
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
	BOOL bWasUpdated = m_bUpdated;
	m_bUpdated = false;
	m_ulBytesWritten = 0;
	return bWasUpdated;
}

BOOL CProcess::Reset()
{
	m_hProcess = NULL;
	m_bGameRunning = false;
	return true;
}

HANDLE CProcess::OpenProcess(char* pszClassName, char* pszWindowTitle)
{
	u_int CREATE_THREAD_ACCESS =
		PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
		PROCESS_VM_OPERATION | PROCESS_VM_WRITE |
		PROCESS_VM_READ | PROCESS_VM_OPERATION;

	u_long ulPID; // Process ID
	if (pszWindowTitle)
	{
		m_hWindow = FindWindow(pszClassName, pszWindowTitle);
	}

	if (m_hPrevWindow != m_hWindow)
	{
		m_bUpdated = true;
	}

	m_hPrevWindow = m_hWindow;

	if (m_hWindow)
	{
		GetWindowThreadProcessId(m_hWindow, &ulPID);

		//HANDLE hToken = NULL;
		//LUID luid;
		//ZeroMemory(&luid, sizeof(luid));

		//if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		//{
		//	return NULL;
		//}

		//if (!LookupPrivilegeValue(NULL, "SeDebugPrivilege", &luid))
		//{
		//	return NULL;
		//}

		//TOKEN_PRIVILEGES xNewPrivilages;
		//DWORD uReturnLength = 0;
		//xNewPrivilages.PrivilegeCount = 1;
		//xNewPrivilages.Privileges[0].Luid = luid;
		//xNewPrivilages.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		//AdjustTokenPrivileges(hToken, FALSE, &xNewPrivilages, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
		//DWORD uErrorCode = ::GetLastError();

		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, false, ulPID);
		return hProcess;
	}

	return NULL;
}

BOOL CProcess::FindProcess()
{
	m_hProcess = this->OpenProcess(NULL, m_pszWindowTitle);

	if (m_hProcess)
	{
		m_bGameRunning = true;
	}
	else
	{
		m_bGameRunning = false;
	}

	return m_bGameRunning;
}

BOOL CProcess::FindProcess(char* pszWindowTitle)
{
	m_pszWindowTitle = pszWindowTitle;
	return this->FindProcess();
}

template<class TData>
BOOL CProcess::Write(u_int uAddress, TData xData)
{
	if (!m_bGameRunning)
	{
		return false;
	}

	DWORD uSize = sizeof(xData);
	BOOL uResult = FALSE;
	DWORD uOldProtect = 0;
	VirtualProtectEx(m_hProcess, (LPVOID)uAddress, uSize, PAGE_EXECUTE_READWRITE, &uOldProtect);
	uResult = WriteProcessMemory(m_hProcess,(void*)uAddress,(void*)&xData,sizeof(xData),&m_ulBytesWritten);
	VirtualProtectEx(m_hProcess, (LPVOID)uAddress, uSize, uOldProtect, 0);

	return uResult;
}

template<class TData>
BOOL CProcess::Write(u_int pxAddress, TData* pxData, u_int uSize)
{
	if (!(m_bGameRunning && uSize))
	{
		return false;
	}

	BOOL uResult = FALSE;
	DWORD uOldProtect = 0;
	VirtualProtectEx(m_hProcess, (LPVOID)pxAddress, uSize, PAGE_EXECUTE_READWRITE, &uOldProtect);
	uResult = WriteProcessMemory(m_hProcess, (void*)pxAddress, (void*)pxData, (u_long)uSize * sizeof(pxData[0]), &m_ulBytesWritten);
	VirtualProtectEx(m_hProcess, (LPVOID)pxAddress, uSize, uOldProtect, 0);

	return uResult;
}

template<class TData>
TData CProcess::Read(TData* pxAddress)
{
	u_long ulBytesRead = 0;
	TData xRetVal;

	if (m_bGameRunning)
	{
		if (!ReadProcessMemory(m_hProcess, (void*)pxAddress,(void*)&xRetVal, sizeof(xRetVal), &ulBytesRead))
		{
			return 0;
		}

		return xRetVal;
	}

	return false;
}

template<class TData>
TData* CProcess::Read(TData* pxAddress, u_int uSize)
{
	u_long ulBytesRead = 0;
	TData* xRetVal = new TData[uSize];

	if (m_bGameRunning)
	{
		if (!ReadProcessMemory(m_hProcess, (void*)pxAddress,(void*)xRetVal, (u_long)uSize*sizeof(*xRetVal), &ulBytesRead))
		{
			return 0;
		}

		return xRetVal;
	}

	return false;
}

template<class TData>
BOOL CProcess::CmpData(u_int uAddress, TData xTestData)
{
	TData xActualData = Read((TData*) uAddress);
	if (xActualData == xTestData)
	{
		return true;
	}

	return false;
}

BOOL CProcess::WriteXString(u_int uAddress, char* szHexStr)				// In dire need of reworking
{
	u_int uStrSize = (u_int) strlen(szHexStr);								// Size of string

	if (!m_bGameRunning || (uStrSize <= 0))									// 0 length or no game
	{
		return false;
	}

	u_int uDataSize = 0;													// No of bytes to be written
	u_char* pucAarray = new u_char[1+(uStrSize/2)];								// 2 chars = 1 byte

	while (*szHexStr)														// For every character
	{
		if (isxdigit(*szHexStr))											// If it's hex scan for a byte
		{
			if (isxdigit(szHexStr[1]))
			{
				u_int uBytes = 0;
				sscanf(szHexStr++, "%02x", &uBytes);
				pucAarray[uDataSize++] = static_cast<u_char>(uBytes);
			}
		}

		szHexStr++;
	}

	if (this->Write(uAddress, pucAarray, uDataSize)==TRUE)
	{
		return true;
	}
	
	return false;
}

u_int CProcess::WriteString(u_int uAddress, char* szAscStr)
{
	u_int strSize = (u_int)strlen(szAscStr);
	if (strSize && this->Write(uAddress, szAscStr, 1+strSize))		// Add 1 to include the null?
	{
		return uAddress;
	}

	return false;
}

u_int CProcess::Alloc(u_int uBytes)
{
	return (u_int) VirtualAllocEx(m_hProcess, 0, (int)uBytes, MEM_COMMIT, PAGE_READWRITE);
}

u_int CProcess::Free(u_int uAddress)
{
	return (u_int) VirtualFreeEx(m_hProcess, 0, 0, MEM_RELEASE);
}

u_int CProcess::SizeOfLast()
{
	return m_ulBytesWritten;
}

char* CProcess::CmpString(u_int uAddress, char* szAscStr)
{
	if (!m_bGameRunning)
	{
		return false;
	}

	u_int charNo = (u_int)strlen(szAscStr);
	char* retStr = (char*) this->Read((char*)uAddress, charNo);
	if (m_bGameRunning && retStr && !strncmp(retStr, szAscStr, charNo))
	{
		return szAscStr;
	}

	return false;
}

u_int CProcess::AddrDiff(u_int uOrigin, u_int uDest)
{
	return uDest-(uOrigin+0x04);
}

BOOL CProcess::WriteDiff(u_int uOrigin, u_int uDest)
{
	if (this->Write(uOrigin, this->AddrDiff(uOrigin, uDest)))
	{
		return true;
	}

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

__declspec(naked) void loadDll(void)
{
	_asm {
		// Save the flags and registers
		pushfd
			pushad
			// Placeholder for the string address and LoadLibrary
			push 0xDEADBEEF
			mov eax, 0xDEADBEEF
			// Call LoadLibrary with the string parameter
			call eax
			// Store the dll start address
			mov dword ptr ds:[0xDEADBEEF],eax
			// Restore the registers and flags
			popad
			popfd
			// Return control to the hijacked thread
			ret
			// Space for dll start address
			nop
			nop
			nop
			nop
	} 
} 
__declspec(naked) void loadDll_end(void) {} 

u_int CProcess::InjectDll(char* szDllName)
{
	void* pucStub = NULL;
	void* pucDllString = NULL;

	u_int uStubLen = (u_int)loadDll_end - (u_int)loadDll;
	FARPROC pfnLoadLibAddy = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

	// If nessacary allocate memory
	u_int uDllNameLength = (u_int)strlen(szDllName)+1;
	pucDllString = VirtualAllocEx(m_hProcess, NULL, uStubLen+uDllNameLength, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	pucStub = (u_char*)pucDllString + uDllNameLength;

	// Fill in the place holders
	u_long ulOldProt; 
	u_char* loadLibRetVal = (u_char*) pucStub + 23;
	VirtualProtect(loadDll, uStubLen, PAGE_EXECUTE_READWRITE, &ulOldProt);
	memcpy((void*)((u_int)loadDll + 3), &pucDllString, 4);
	memcpy((void*)((u_int)loadDll + 8), &pfnLoadLibAddy, 4);
	memcpy((void*)((u_int)loadDll + 16), &loadLibRetVal, 4);

	// Paste in the loading function
	WriteProcessMemory(m_hProcess, pucDllString, szDllName, strlen(szDllName), NULL);
	WriteProcessMemory(m_hProcess, pucStub, loadDll, uStubLen, NULL);

	// Create a thread to load the dll
	HANDLE hQuickThread = CreateRemoteThread(m_hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)pucStub, (LPVOID)NULL, NULL, NULL);
	CloseHandle(hQuickThread);
	WaitForSingleObject(hQuickThread, INFINITE);

	// Get the dll location
	Sleep(3000); // If it reads the memory too early, it crashes
	u_int uDllLocation = NULL;
	u_long ulBytesRead = NULL;
	ReadProcessMemory(m_hProcess, (void*)loadLibRetVal, (u_char*)&uDllLocation, 4, &ulBytesRead);

	// Free the memory
	VirtualFreeEx(m_hProcess, pucDllString, uStubLen+uDllNameLength, MEM_DECOMMIT);

	// Return the location of the dll
	return uDllLocation;
} 

// Write single
template BOOL CProcess::Write<u_char>(u_int uAddress, u_char ucData);
template BOOL CProcess::Write<u_short>(u_int uAddress, u_short usData);
template BOOL CProcess::Write<u_int>(u_int uAddress, u_int uData);
template BOOL CProcess::Write<int>(u_int uAddress, int uData);
template BOOL CProcess::Write<float>(u_int uAddress, float fData);
// Write array
template BOOL CProcess::Write<u_char>(u_int uAddress, u_char* pucData, u_int uSize);
template BOOL CProcess::Write<u_short>(u_int uAddress, u_short* pusData, u_int uSize);
template BOOL CProcess::Write<u_int>(u_int uAddress, u_int* puData, u_int uSize);
template BOOL CProcess::Write<int>(u_int uAddress, int* puData, u_int uSize);
template BOOL CProcess::Write<float>(u_int uAddress, float* pfData, u_int uSize);
// Read Single
template u_char CProcess::Read<u_char>(u_char* pucAddress);
template u_short CProcess::Read<u_short>(u_short* pusAddress);
template u_int CProcess::Read<u_int>(u_int* puAddress);
template int CProcess::Read<int>(int* puAddress);
template float CProcess::Read<float>(float* puAddress);
// Read array
template u_char* CProcess::Read<u_char>(u_char* pucAddress, u_int uSize);
template u_short* CProcess::Read<u_short>(u_short* pusAddress, u_int uSize);
template u_int* CProcess::Read<u_int>(u_int* puAddress, u_int uSize);
template int* CProcess::Read<int>(int* puAddress, u_int uSize);
template float* CProcess::Read<float>(float* puAddress, u_int uSize);
// Compare Single
template BOOL CProcess::CmpData<u_char>(u_int uAddress, u_char ucData);
// CProcess class based on code by Jared Bruni http://www.lostsidedead.com/gameprog

#include <stdio.h>
#include <iostream>
#include <sstream>

#include "SR1Trainer2.h"
#include "resource.h"

// Poke every 500 milliseconds
#define TIMER_INTERRUPT 500
#define ID_TIMER 1

HINSTANCE SR1Trainer2::s_hInst;

HCURSOR* SR1Trainer2::s_hSRCursor;
WPARAM SR1Trainer2::s_hButtonFont = SR1Trainer2::ButtonFont();

HWND SR1Trainer2::s_hMainWin;
HWND SR1Trainer2::s_hMemWin;

CProcess SR1Trainer2::s_xProcess("Legacy of Kain: Soul Reaver");

SR_Key SR1Trainer2::s_axKeys[107];

SR_Button SR1Trainer2::s_axButtons[] =
{
	{"Modify HUD", SR1_ModifyHUDBtn},
};

SR_Combo SR1Trainer2::s_axComboBoxes[] =
{
	{SR1_ModifyHUDCmb, VK_F11},
};

u_char SR1Trainer2::SR1_ModifyHUDKey = VK_F11;

u_int SR1Trainer2::s_uAllocated = 0x00000000;
u_int SR1Trainer2::s_uMemTracker = 0x00000000;

bool SR1Trainer2::s_bInitialised = false;

int APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR line,int CmdShow)
{
	SR1Trainer2::Initialise(hInst);

	MSG msg;
	while (GetMessage(&msg,0,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void SR1Trainer2::Initialise(HINSTANCE hInst)
{
	s_hInst = hInst;

	WNDCLASS wc;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = CreatePatternBrush(LoadBitmap(s_hInst,MAKEINTRESOURCE(IDB_BITMAP1)));
	wc.hInstance = s_hInst;
	wc.hIcon = LoadIcon(s_hInst,MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(s_hInst,MAKEINTRESOURCE(IDC_CURSOR1));// LoadCursor(NULL,IDC_ARROW);
	s_hSRCursor = &wc.hCursor;
	wc.lpfnWndProc = (WNDPROC) SR1Trainer2::MainWindowProc;
	wc.lpszClassName = "MainWindow";
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);

	wc.lpszMenuName = NULL;
	wc.hIcon = LoadIcon(s_hInst,MAKEINTRESOURCE(IDI_ICON2));

	wc.lpszClassName = "MemWindow";
	wc.lpfnWndProc = (WNDPROC) SR1Trainer2::MemWindowProc;

	RegisterClass(&wc);

	CreateMainWindow(0,0,345,210);
}

LRESULT APIENTRY SR1Trainer2::MainWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	u_short hw = HIWORD(wParam);
	switch(msg)
	{
		case WM_DESTROY:
		{
			CleanUp();
			KillTimer(hWnd, ID_TIMER);
			PostQuitMessage(0);
			break;
		}
		case WM_CREATE:
		{
			SetTimer(hWnd, ID_TIMER, TIMER_INTERRUPT, NULL);
			break;
		}
		case WM_SIZE:
		{
			RECT rcClient;
			GetClientRect(hWnd, &rcClient);
			LPRECT rcParent = (LPRECT)(LPARAM)&rcClient;
			MoveWindow(
				s_hMemWin,
				0,
				0,
				rcParent->right,
				rcParent->bottom,
				TRUE
			);
			break;
		}
		case WM_TIMER:
		{
			WriteMem(hWnd);
			break;
		}
		case WM_COMMAND:
		{			
			OnMenu(wParam);
			break;
		}
		default:
		{
			break;
		}
	}

	return DefWindowProc(hWnd,msg,wParam,lParam);
}

void SR1Trainer2::OnMenu(WPARAM wParam)
{
	switch(wParam)
	{
		case ID_MODE_SR1:
			//ShowWindow(s_hMemWin,SW_SHOW);
			//ShowWindow(s_hSearchWin,SW_HIDE);
			break;
		case ID_MODE_SR2:
			//ShowWindow(s_hMemWin,SW_HIDE);
			//ShowWindow(s_hSearchWin,SW_SHOW);
			break;
		case ID_EXIT_TRAINER:
			SendMessage(s_hMainWin,WM_CLOSE, 0, 0);
			break;
	}
}

LRESULT APIENTRY SR1Trainer2::MemWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	u_short hw = HIWORD(wParam);
    switch (msg)
	{
		case WM_CLOSE:
			ShowWindow(hWnd,SW_HIDE);
			break;
		case WM_CREATE:
	    	break;
		case WM_COMMAND:
		{
			switch (hw)
			{
				case BN_CLICKED:
				{
					if ((LOWORD(wParam))==SR1_ModifyHUDBtn)
					{
						ModifyHUD();
					}

					break;
				}
				case CBN_DROPDOWN:
				{
					break;
				}
				case CBN_SELCHANGE:
				{
					if ((LOWORD(wParam))==SR1_ModifyHUDCmb)
					{
						SR1_ModifyHUDKey = s_axKeys[SendDlgItemMessage(hWnd, SR1_ModifyHUDCmb, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)].vk;
					}

					break;
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}

	return DefWindowProc(hWnd,msg,wParam,lParam);
}

WPARAM SR1Trainer2::ButtonFont()
{
	LOGFONT lf;
	HFONT hFont = 0;
	GetObject(hFont, sizeof(LOGFONT), &lf);
	strcpy(lf.lfFaceName,"Microsoft Sans Serif");
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfItalic = 0;
	lf.lfWeight = 100;
	lf.lfHeight = 14;
	lf.lfWidth = 5;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	// lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	hFont = CreateFontIndirect(&lf);
	return (WPARAM) hFont;
}

void SR1Trainer2::CreateKeyStrings()
{
	int	kName =	0;
	char* str = new char[20];
	UINT nScanCode;
	for	(int kCode = 0;	kCode <	255; kCode++)
	{
		nScanCode =	MapVirtualKeyEx(kCode, 0, GetKeyboardLayout(0));
		if ((kCode >= 162 && kCode <= 165) || (kCode>=33 && kCode <= 46))
		{
			nScanCode |= 0x100;
		}

		switch (kCode)
		{
			case VK_CLEAR:
			case VK_RETURN:
			case VK_LWIN:
			case VK_RWIN:
			case VK_APPS:
			case VK_SEPARATOR:
			case VK_DIVIDE:
				nScanCode |= 0x100;
		}

		GetKeyNameText(nScanCode <<	16,	str, 20);

		if (kCode>=172 && kCode<=183) *str=0;
		if (kCode==VK_SNAPSHOT)	strcpy(str,	"PRINT SCREEN");
		if (kCode==VK_LSHIFT)	strcpy(str,	"LEFT SHIFT");
		if (kCode==VK_LCONTROL)	strcpy(str,	"LEFT CTRL");
		if (kCode==VK_LMENU)	strcpy(str,	"LEFT ALT");

		if (str==0 || *str==0) continue;

		s_axKeys[kName].name = str;
		s_axKeys[kName].vk = kCode;
		str = new char[20];
		kName++;
	}
}

void SR1Trainer2::CreateHKComboCol(SR_Combo c, HWND hWnd)
{
	for (int j=0; j<107; j++)
	{
		SendDlgItemMessage(hWnd,c.id,CB_ADDSTRING,(WPARAM)0,(LPARAM)s_axKeys[j].name);
		if (s_axKeys[j].vk == c.defKey)
		{
			SendDlgItemMessage(hWnd,c.id,CB_SETCURSEL,(WPARAM)j,(LPARAM)0);
		}
	}
}

void SR1Trainer2::CreateMainWindow(int x, int y, int w, int h)
{
	s_hMainWin = CreateWindow("MainWindow", "Soul Reaver 1 Trainer v2.0", WS_OVERLAPPEDWINDOW, x, y, w, h, 0, 0, s_hInst, 0);

	CreateMemWindow(s_hMainWin, 100, 100, 100, 100);

	ShowWindow(s_hMainWin, SW_SHOW);
	UpdateWindow(s_hMainWin);
}

void SR1Trainer2::CreateMemWindow(HWND hParent, int x, int y, int w, int h)
{
	s_hMemWin = CreateWindowEx(
		WS_EX_CLIENTEDGE, "MemWindow", "", WS_VISIBLE | WS_CHILD | WS_MAXIMIZE,
		x, y, w, h, hParent, (HMENU) SR1_SubWin, s_hInst, 0
	);

	CreateButtonCol(s_axButtons, s_hMemWin, 1, 50, 30, 100, 20);
	CreateKeyStrings();
	CreateComboCol(s_axComboBoxes, s_hMemWin, 1, 180, 30, 100, 200);

	ShowWindow(s_hMemWin, SW_SHOW); // SW_MAXIMIZE);
}

void SR1Trainer2::CreateButtonCol(SR_Button* b, HWND hParent, int noOfButtons, int x, int y, int w, int h)
{
	HWND hButton;
	for (int i = 0; i < noOfButtons; i++)
	{
		hButton = CreateWindowEx(
			0, "Button", b[i].text, BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			x, y, w, h, hParent, (HMENU)b[i].id, s_hInst, 0
		);

		SendDlgItemMessage(hParent, b[i].id,WM_SETFONT,s_hButtonFont,FALSE);
		SetClassLong(hButton, GCL_HCURSOR, (long) *s_hSRCursor);
		y += 25;
	}
}

void SR1Trainer2::CreateComboCol(SR_Combo* c, HWND hParent, int noOfComboBoxes, int x, int y, int w, int h)
{
	HWND theCombo;
	for (int i = 0; i < noOfComboBoxes; i++)
	{
		theCombo = CreateWindowEx(
			0, "ComboBox","", WS_CHILD | WS_VISIBLE | 0x00010003 | CBS_UPPERCASE | WS_VSCROLL,
			x, y, w, h, hParent, (HMENU) c[i].id, s_hInst, 0
		);

		SendDlgItemMessage(hParent, c[i].id, WM_SETFONT, s_hButtonFont, FALSE);
		SetClassLong(theCombo, GCL_HCURSOR, (long) *s_hSRCursor);

		y += 25;
		CreateHKComboCol(c[i], hParent);
	}
}

bool SR1Trainer2::SetUpProcess()
{
	s_xProcess.FindProcess();

	if (s_xProcess.WasUpdated())
	{
		s_uAllocated = 0x00000000;
		s_uMemTracker = 0x00000000;
		s_bInitialised = false;
	}

	if (s_xProcess.IsRunning())
	{
		return true;
	}

	return false;
}

void SR1Trainer2::CleanUp()
{
}

void SR1Trainer2::FindMe()
{
	u_int *address;
	__asm
	{
		mov eax, ebp; // Should be esp for stack pointer?
		add eax, 0x04;
		mov [address],eax;
		mov eax, 00000001;
	}
	char* errInfo = new char[50];
	sprintf(errInfo,"Made it to address 0x%08X without crashing!", *address);
	MessageBox(NULL, errInfo, "ERROR", MB_ICONERROR | MB_OK);
}

void SR1Trainer2::WriteMem(HWND hWnd)
{
	if (s_xProcess.IsKeyPressed(SR1_ModifyHUDKey))
	{
		ModifyHUD();
	}
}

void SR1Trainer2::ModifyHUD()
{
	if (SetUpProcess())
	{
		if (!s_bInitialised)
		{
			s_bInitialised = true;
			s_uAllocated = s_xProcess.Alloc(2048);
			s_uMemTracker = s_uAllocated;

			bool bIsWidescreen = true;
			bool bIsUsingHealthFromSR1Beta = true;

			u_int uSpectralColour = 0x00004000;
			u_int uMaterialColour = bIsUsingHealthFromSR1Beta ? 0x00400000 : 0x00004000;

			if (bIsWidescreen)
			{
				// u_int uWorldScaleX = 0x004EC354;
				u_int uWorldScaleY = 0x004EC358;
				float fWorldScaleY = 16.0f / 9.0f; // 1.777778
				s_xProcess.Write(uWorldScaleY, fWorldScaleY);

				ModifyHealthSpiral(s_uMemTracker, 0u, 0u, 0.8f, 1.0f);

				// The game stretches the glow in both directions when 1280 * 768 is used,
				// so the Y scale is the same as the X scale this time.
				ModifyHealthGlow(s_uMemTracker, 0u, 0u, 0.8f, 0.8f, uSpectralColour, uMaterialColour);
			}
			else
			{
				ModifyHealthGlow(s_uMemTracker, 0u, 0u, 1.0f, 1.0f, uSpectralColour, uMaterialColour);
			}

			//SR_MenuItem xMainMenuItem = { 8, 0, 0, s_uStandardMenuText, s_uStandardMenu, 0x00000000 };

			//SR_MenuItem* pxMoveMenu = NULL;
			//delete[] pxMoveMenu;
		}

		//s_xProcess.WriteXString(0x0049EA02, "E829010000");
		//s_xProcess.WriteXString(0x0049EB32, "C705C452730000007043C3");
		//s_xProcess.WriteXString(0x0049EF8B, "E9A2FBFFFF");
		//s_xProcess.WriteXString(0x00702E16, "04");
		//s_xProcess.WriteXString(0x007352C6, "D0");
	}
}

void SR1Trainer2::ModifyHealthSpiral(u_int& uMemTracker,
	int iXOffset, int iYOffset, float fXScale, float fYScale)
{
	u_int uSpiralCentreXAddress = 0x00C54CF8;
	u_int uSpiralCentreYAddress = 0x00C54CFA;

	u_int uSpiralOffsetXAddress = uMemTracker;
	s_xProcess.Write(uMemTracker, iXOffset);
	uMemTracker += s_xProcess.SizeOfLast();

	u_int uSpiralOffsetYAddress = uMemTracker;
	s_xProcess.Write(uMemTracker, iYOffset);
	uMemTracker += s_xProcess.SizeOfLast();

	u_int uSpiralScaleXAddress = uMemTracker;
	s_xProcess.Write(uMemTracker, fXScale);
	uMemTracker += s_xProcess.SizeOfLast();

	u_int uSpiralScaleYAddress = uMemTracker;
	s_xProcess.Write(uMemTracker, fYScale);
	uMemTracker += s_xProcess.SizeOfLast();

	u_int uSpiralFunctionXAddress = uMemTracker;
	s_xProcess.Write(uMemTracker, uSpiralFunctionXAddress + 0x04);
	uMemTracker += s_xProcess.SizeOfLast();
	// 0FBF 1D F84CC500      - movsx ebx,word ptr [00C54CF8] { [0282043B] }
	// Subtract, the centre of the coil, then scale, then subtract again, otherwise it will move incorrectly.
	s_xProcess.WriteXString(uMemTracker, "50 8B 44 24 08 53 0F BF 1D F8 4C C5 00");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "29 D8 83 EC 04 D9 1C 24 50 D9 05");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralScaleXAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "DA 0C 24 DB 1C 24 58 D9 04 24 83 C4 04 03 05");
	// 83 EC 04 - sub esp, 4
	// D9 1C 24 - fstp dword ptr[esp] // Stores old data on the stack
	// 50 push eax
	// D9 05 08 00 31 00 - fld dword ptr[00310008]{ [1.00] } // Loads my scale value
	// DA 0C 24 - fimul [esp] // Multiplies my scale value by the value on the stack
	// DB 1C 24 - fistp dword ptr [esp] // Stores result on stack
	// 58 pop eax
	// D9 04 24 - fld dword ptr [esp] // Loads old data from the stack
	// 83 C4 04 - add esp, 4
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralOffsetXAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "01 D8 5B 89 44 24 08 58 C3 90");
	uMemTracker += s_xProcess.SizeOfLast();

	u_int uSpiralFunctionYAddress = uMemTracker;
	s_xProcess.Write(uMemTracker, uSpiralFunctionYAddress + 0x04);
	uMemTracker += s_xProcess.SizeOfLast();
	// 0FBF 1D FA4CC500      - movsx ebx,word ptr [00C54CFA] { [130] }
	// Subtract, the centre of the coil, then scale, then subtract again, otherwise it will move incorrectly.
	s_xProcess.WriteXString(uMemTracker, "50 8B 44 24 08 53 0F BF 1D FA 4C C5 00 29 D8 83 EC 04 D9 1C 24 50 D9 05");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralScaleYAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "DA 0C 24 DB 1C 24 58 D9 04 24 83 C4 04 03 05");
	// 83 EC 04 - sub esp, 4
	// D9 1C 24 - fstp dword ptr[esp] // Stores old data on the stack
	// 50 push eax
	// D9 05 08 00 31 00 - fld dword ptr[00310008]{ [1.00] } // Loads my scale value
	// DA 0C 24 - fimul [esp] // Multiplies my scale value by the value on the stack
	// DB 1C 24 - fistp dword ptr [esp] // Stores result on stack
	// 58 pop eax
	// D9 04 24 - fld dword ptr [esp] // Loads old data from the stack
	// 83 C4 04 - add esp, 4
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralOffsetYAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "01 D8 5B 89 44 24 08 58 C3 90");
	uMemTracker += s_xProcess.SizeOfLast();

	s_xProcess.WriteXString(0x004A8D2B, "E8");
	s_xProcess.WriteDiff(0x004A8D2C, uMemTracker);
	s_xProcess.WriteXString(0x004A8D30, "90 90");
	// 004A8D2B - 0FBF 8E FA4BC500      - movsx ecx,word ptr [esi+00C54BFA]
	s_xProcess.WriteXString(uMemTracker, "0F BF 8E FA 4B C5 00 51 FF 15");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralFunctionYAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "59 C3 90");
	uMemTracker += s_xProcess.SizeOfLast();

	s_xProcess.WriteXString(0x004A8D3B, "E8");
	s_xProcess.WriteDiff(0x004A8D3C, uMemTracker);
	s_xProcess.WriteXString(0x004A8D40, "90 90");
	// 004A8D3B - 0FBF 96 204EC500 - movsx edx, word ptr[esi + 00C54E20]
	s_xProcess.WriteXString(uMemTracker, "0F BF 96 20 4E C5 00 52 FF 15");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralFunctionXAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "5A C3 90");
	uMemTracker += s_xProcess.SizeOfLast();

	s_xProcess.WriteXString(0x004A8D54, "E8");
	s_xProcess.WriteDiff(0x004A8D55, uMemTracker);
	// 004A8D54 - 0FBF 86 F84BC500      - movsx eax,word ptr [esi+00C54BF8]
	s_xProcess.WriteXString(uMemTracker, "0F BF 86 F8 4B C5 00 50 FF 15");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralFunctionXAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "58");
	uMemTracker += s_xProcess.SizeOfLast();

	s_xProcess.WriteXString(0x004A8D59, "90 90 90 90 90");
	// 004A8D5B - 0FBF 3F - movsx edi, word ptr[edi]
	s_xProcess.WriteXString(uMemTracker, "0F BF 3F 57 FF 15");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralFunctionYAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "5F");
	uMemTracker += s_xProcess.SizeOfLast();

	s_xProcess.WriteXString(0x004A8D6E, "90 90 90 90");
	// 004A8D6E - 0FBF 6D FE            - movsx ebp,word ptr [ebp-02]
	s_xProcess.WriteXString(uMemTracker, "0F BF 6D FE 55 FF 15");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralFunctionXAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "5D C3 90");
	uMemTracker += s_xProcess.SizeOfLast();

	s_xProcess.WriteXString(0x004A8D7E, "E8");
	s_xProcess.WriteDiff(0x004A8D7F, uMemTracker);
	// 004A8D7E - 0FBF 36               - movsx esi,word ptr [esi]
	// 004A8D81 - D9CD - fxch st(5)
	s_xProcess.WriteXString(uMemTracker, "0F BF 36 56 FF 15");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralFunctionYAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "5E D9 CD C3 90");
	uMemTracker += s_xProcess.SizeOfLast();

	s_xProcess.WriteXString(0x004A8B6E, "E8");
	s_xProcess.WriteDiff(0x004A8B6F, uMemTracker);
	s_xProcess.WriteXString(0x004A8B73, "90 90");
	// 004A8B6E - 0FBF 0D FA4CC500      - movsx ecx, word ptr[00C54CFA]
	s_xProcess.WriteXString(uMemTracker, "0F BF 0D FA 4C C5 00 51 FF 15");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralFunctionYAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "59 C3 90");
	uMemTracker += s_xProcess.SizeOfLast();

	s_xProcess.WriteXString(0x004A8B7F, "E8");
	s_xProcess.WriteDiff(0x004A8B80, uMemTracker);
	s_xProcess.WriteXString(0x004A8B84, "90 90");
	// 004A8B7F - 0FBF 05 F84CC500      - movsx eax,word ptr [00C54CF8]
	s_xProcess.WriteXString(uMemTracker, "0F BF 05 F8 4C C5 00 50 FF 15");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uSpiralFunctionXAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "58 C3 90");
	uMemTracker += s_xProcess.SizeOfLast();
}

void SR1Trainer2::ModifyHealthGlow(u_int& uMemTracker,
	int iXOffset, int iYOffset, float fXScale, float fYScale,
	u_int uABGRSpectralColour, u_int uABGRMaterialColour)
{
	u_int uGlowOffsetXAddress = uMemTracker;
	s_xProcess.Write(uMemTracker, iXOffset);
	uMemTracker += s_xProcess.SizeOfLast();

	u_int uGlowOffsetYAddress = uMemTracker;
	s_xProcess.Write(uMemTracker, iYOffset);
	uMemTracker += s_xProcess.SizeOfLast();

	u_int uGlowScaleXAddress = uMemTracker;
	s_xProcess.Write(uMemTracker, fXScale);
	uMemTracker += s_xProcess.SizeOfLast();

	u_int uGlowScaleYAddress = uMemTracker;
	s_xProcess.Write(uMemTracker, fYScale);
	uMemTracker += s_xProcess.SizeOfLast();

	s_xProcess.WriteXString(0x004A8EF9, "E8");
	s_xProcess.WriteDiff(0x004A8EFA, uMemTracker);
	s_xProcess.WriteXString(0x004A8EFE, "90");
	// 004A8EF9 - D8 0D 88C44E00        - fmul dword ptr [004EC488] { [1.70] }
	s_xProcess.WriteXString(uMemTracker, "D8 0D 88 C4 4E 00 D8 0D");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uGlowScaleYAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "C3 90");
	uMemTracker += s_xProcess.SizeOfLast();

	s_xProcess.WriteXString(0x004A8F16, "E8");
	s_xProcess.WriteDiff(0x004A8F17, uMemTracker);
	s_xProcess.WriteXString(0x004A8F1B, "90 90 90 90 90 90 90 90 90");
	// 004A8F16 - DCC0 - fadd st(0), st(0)
	// 004A8F18 - DB 05 0C4DC500 - fild dword ptr[00C54D0C]{ [00000449] }
	// 004A8F1E - DB 05 104DC500 - fild dword ptr[00C54D10]{ [00000284] }
	s_xProcess.WriteXString(uMemTracker, "DC C0 D8 0D");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uGlowScaleXAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "DB 05 0C 4D C5 00 DA 05");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uGlowOffsetXAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "DB 05 10 4D C5 00 DA 05");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uGlowOffsetYAddress);
	uMemTracker += s_xProcess.SizeOfLast();
	// 83 3D 5088B000 01     - cmp dword ptr [00B08850],01 { [00000001] } // Checks for material realm
	s_xProcess.WriteXString(uMemTracker, "83 3D 50 88 b0 00 01 74 0A C7 44 24 04");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uABGRSpectralColour);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "EB 08 C7 44 24 04");
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.Write(uMemTracker, uABGRMaterialColour);
	uMemTracker += s_xProcess.SizeOfLast();
	s_xProcess.WriteXString(uMemTracker, "C3 90");
	uMemTracker += s_xProcess.SizeOfLast();
}
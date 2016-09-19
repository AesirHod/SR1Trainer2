// CProcess class based on code by Jared Bruni http://www.lostsidedead.com/gameprog

/*
#pragma comment(lib,"comctl32.lib")
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>
#include <commctrl.h>
#include "process.h"
*/
#include "resource.h"
#include "sr1.h"

// Enumerated constants for the child handles
enum ctrlIDs {
	SR1_DebugOn = 1, SR1_DbgOnKey, SR1_DebugOff, SR1_DbgOffKey,
	SR1_SwimOn, SR1_SwmOnKey, SR1_SwimOff, SR1_SwmOffKey,
	SR1_SubWin,
	SR2_DebugOn = 1, SR2_DbgOnKey, SR2_DebugOff, SR2_DbgOffKey,
	SR2_SubWin
};
struct button {
	char* text;
	int id;
};
struct combo {
	int id;
	BYTE defKey;
};

// Key configurations
struct key {
	int vk;
	char* name;
};

key keys[107];

// Main functions
LRESULT APIENTRY mainWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
LRESULT APIENTRY subWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
LRESULT APIENTRY subWindow2Proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
WPARAM ButtonFont();
WPARAM buttonFont = ButtonFont();
void onMenu(WPARAM);
void hkComboCol(combo c, HWND hwnd);
void keyStrings();
void buttonCol	(button* b,	HWND hwnd,	int noOfButtons, int X, int Y, int W, int H);
void comboCol	(combo* c,	HWND hwnd,	int noOfButtons, int X, int Y, int W, int H);

// Windows
WNDCLASS wc;
HINSTANCE g_hInst;
HWND mainWin;
HWND sr1Win, sr2Win;
HCURSOR* sr1Cur;

// Poke every 500 milliseconds
#define TIMER_INTERRUPT			500	
#define ID_TIMER				1	

// SR1 Controls
button SR1Buttons[] = {
	{"Debug On", SR1_DebugOn},	{"Debug Off", SR1_DebugOff},
	{"Swim On", SR1_SwimOn},	{"Swim Off", SR1_SwimOff}
};
combo SR1ComboBoxes[] = {
	{SR1_DbgOnKey, VK_F11},	{SR1_DbgOffKey, VK_F12},
	{SR1_SwmOnKey, VK_F9},	{SR1_SwmOffKey, VK_F10}
};
BYTE SR1_dbgOn = VK_F11, SR1_dbgOff = VK_F12, SR1_swmOn = VK_F9, SR1_swmOff = VK_F10, SR1_blkCr = VK_F8;

// SR2 Controls
button SR2Buttons[] = {
	{"Debug On", SR2_DebugOn},	{"Debug Off", SR2_DebugOff}
};
combo SR2ComboBoxes[] = {
	{SR2_DbgOnKey, VK_F11},	{SR2_DbgOffKey, VK_F12}
};
BYTE SR2_dbgOn = VK_F11, SR2_dbgOff = VK_F12, SR2_swmOn = VK_F9, SR2_swmOff = VK_F10;

void WriteMem(HWND hwnd)
{
	// [SR1]
	// Debug On
	if (SR1.IsKeyPressed(SR1_dbgOn))	debugOn();
	// Debug Off
	if (SR1.IsKeyPressed(SR1_dbgOff))	debugOff();
	// Flood On
	if (SR1.IsKeyPressed(SR1_swmOn))	swimOn();
	// Flood Off
	if (SR1.IsKeyPressed(SR1_swmOff))	swimOff();
	// Black Coil
	if (SR1.IsKeyPressed(SR1_blkCr))	blackCore();
	// [/SR1]

	// [SR2]
	// Debug On
	// if (SR1.IsKeyPressed(SR1_dbgOn))	debugOn();
	// Debug Off
	// if (SR1.IsKeyPressed(SR1_dbgOff))debugOff();
	// [/SR2]
}

int APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR line,int CmdShow)
{
	g_hInst = hInst;
	MSG msg;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = CreatePatternBrush(LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP1)));//(HBRUSH) COLOR_BACKGROUND;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(hInst,MAKEINTRESOURCE(IDC_CURSOR1));// LoadCursor(NULL,IDC_ARROW);
	sr1Cur = &wc.hCursor;
	wc.lpfnWndProc = (WNDPROC) mainWindowProc;
	wc.lpszClassName = "MainWindow";
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);

	wc.lpszMenuName = NULL;
	wc.lpszClassName = "SubWindow";
	wc.lpfnWndProc = (WNDPROC) subWindowProc;
	wc.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON2));

	RegisterClass(&wc);

	mainWin = CreateWindow("MainWindow","Soul Reaver Trainer v2.3 Beta",WS_OVERLAPPEDWINDOW,0,0,345,210,0,0,hInst,0);
	ShowWindow(mainWin,SW_SHOW);
	UpdateWindow(mainWin);

	while(GetMessage(&msg,0,0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


LRESULT APIENTRY mainWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

	switch(msg) {
		case WM_DESTROY: {
			cleanUp();
			KillTimer(hwnd, ID_TIMER);
			PostQuitMessage(0);
			break;
		}
		case WM_CREATE: {
			SetTimer(hwnd, ID_TIMER, TIMER_INTERRUPT, NULL);
			// Soul Reaver 1 UI
			sr1Win = CreateWindowEx(
				WS_EX_CLIENTEDGE, "SubWindow", "", WS_VISIBLE | WS_CHILD | WS_MAXIMIZE,
				100,100,100,100, hwnd, (HMENU) SR1_SubWin, g_hInst, 0
			);
			buttonCol(SR1Buttons,	sr1Win, 4, 50, 30, 100, 20);
			keyStrings();
			comboCol(SR1ComboBoxes,	sr1Win, 4, 180, 30, 100, 200);
			ShowWindow(sr1Win, SW_MAXIMIZE);

			// Soul Reaver 2 UI
			sr2Win = CreateWindowEx(
				WS_EX_CLIENTEDGE, "SubWindow", "", WS_VISIBLE | WS_CHILD | WS_MAXIMIZE,
				100,100,100,100, hwnd, (HMENU) SR2_SubWin, g_hInst, 0
			);
			buttonCol(SR2Buttons,	sr2Win, 2, 50, 30, 100, 20);
			keyStrings();
			comboCol(SR2ComboBoxes, sr2Win,	2, 180, 30, 100, 200);
			ShowWindow(sr2Win, SW_HIDE);	// SW_MAXIMIZE
			break;
		}
		// case WM_PAINT:
		case WM_SIZE: {
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			LPRECT rcParent = (LPRECT)(LPARAM)&rcClient;
			MoveWindow(
				sr1Win, 
				0, 
				0, 
				rcParent->right, 
				rcParent->bottom, 
				TRUE
			);
			UpdateWindow(sr1Win);
			MoveWindow(
				sr2Win, 
				0, 
				0, 
				rcParent->right, 
				rcParent->bottom, 
				TRUE
			);
			UpdateWindow(sr2Win);
			break;
		}

		case WM_TIMER: {
			// The function to poke the codes:
			WriteMem(hwnd);
			break;
		}
		case WM_COMMAND:
		{
			onMenu(wParam);
			switch(HIWORD(wParam))
			{
				case BN_CLICKED:
					break;
				case LBN_DBLCLK:
					break;
				case EN_CHANGE:
					break;
			}
			break;
		}
		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}

void onMenu(WPARAM wParam) {
	switch(wParam) {
		case ID_MODE_SR1:
			ShowWindow(sr1Win,SW_SHOW);
			ShowWindow(sr2Win,SW_HIDE);
			break;
		case ID_MODE_SR2:
			ShowWindow(sr1Win,SW_HIDE);
			ShowWindow(sr2Win,SW_SHOW);
			break;
		case ID_EXIT_TRAINER:
			SendMessage(mainWin,WM_CLOSE, 0, 0);
			break;
	}
}

LRESULT APIENTRY subWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	WORD hw = HIWORD(wParam);
    switch(msg)
	{
	case WM_CLOSE:
		ShowWindow(hwnd,SW_HIDE);
		break;
	case WM_CREATE:
	    	break;
	case WM_COMMAND:
		switch(hw)
		{
			case BN_CLICKED:
				{
					if ((LOWORD(wParam))==SR1_DebugOn)	debugOn();
					if ((LOWORD(wParam))==SR1_DebugOff)	debugOff();
					if ((LOWORD(wParam))==SR1_SwimOn)	swimOn();
					if ((LOWORD(wParam))==SR1_SwimOff)	swimOff();
					break;
				}
			case CBN_SELCHANGE:
				{
					if ((LOWORD(wParam))==SR1_DbgOnKey)
						SR1_dbgOn = keys[SendMessage(GetDlgItem(hwnd,SR1_DbgOnKey),CB_GETCURSEL,(WPARAM)0,(LPARAM)0)].vk;
					if ((LOWORD(wParam))==SR1_DbgOffKey)
						SR1_dbgOff = keys[SendMessage(GetDlgItem(hwnd,SR1_DbgOffKey),CB_GETCURSEL,(WPARAM)0,(LPARAM)0)].vk;
					if ((LOWORD(wParam))==SR1_SwmOnKey)
						SR1_swmOn = keys[SendMessage(GetDlgItem(hwnd,SR1_SwmOnKey),CB_GETCURSEL,(WPARAM)0,(LPARAM)0)].vk;
					if ((LOWORD(wParam))==SR1_SwmOffKey)
						SR1_swmOff = keys[SendMessage(GetDlgItem(hwnd,SR1_SwmOffKey),CB_GETCURSEL,(WPARAM)0,(LPARAM)0)].vk;
				break;
				}
		}
		break;
	default: return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}

LRESULT APIENTRY subWindow2Proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	WORD hw = HIWORD(wParam);
    switch(msg) {
		case WM_CLOSE:
			ShowWindow(hwnd,SW_HIDE);
			break;
		case WM_CREATE:
	    	break;
		case WM_COMMAND:
			switch(hw) {
				case BN_CLICKED: {
					break;
				}
				case CBN_SELCHANGE: {
					break;
				}
			}
			break;
		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}


WPARAM ButtonFont() {
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

void keyStrings() {
	int	kName =	0;
	char* str = new char[20];
	UINT nScanCode;
	for	(int kCode = 0;	kCode <	255; kCode++) {
		nScanCode =	MapVirtualKeyEx(kCode, 0, GetKeyboardLayout(0));
		if ((kCode>=162	&& kCode<=165)||(kCode>=33 && kCode<=46)) nScanCode	|= 0x100;
		switch(kCode) {
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
		keys[kName].name = str;
		keys[kName].vk = kCode;
		str = new char[20];
		kName++;
	}
}

void hkComboCol(combo c, HWND hwnd) {
		for (int j=0; j<107; j++) {
			SendMessage(GetDlgItem(hwnd,c.id),CB_ADDSTRING,(WPARAM)0,(LPARAM)keys[j].name);
			if (keys[j].vk == c.defKey)
			SendMessage(GetDlgItem(hwnd,c.id),CB_SETCURSEL,(WPARAM)j,(LPARAM)0);
		}
}

void buttonCol (button* b, HWND hwnd, int noOfButtons, int X, int Y, int W, int H) {
	HWND theButton;
	for (int i=0; i<noOfButtons; i++) {
		theButton = CreateWindowEx(
			0, "Button", b[i].text, BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
			X, Y, W, H, hwnd, (HMENU)b[i].id, g_hInst, 0
		);
		SendDlgItemMessage(hwnd, b[i].id,WM_SETFONT,buttonFont,FALSE);
		SetClassLong(theButton, GCL_HCURSOR, (long) *sr1Cur);
		Y += 25;
	}
}

void comboCol (combo* c, HWND hwnd, int noOfComboBoxes, int X, int Y, int W, int H) {
	HWND theCombo;
	for (int i=0; i<noOfComboBoxes; i++) {
		theCombo = CreateWindowEx (
			0, "ComboBox","",0x50010003 | CBS_UPPERCASE | WS_VSCROLL, // | CBS_SORT,
			X, Y, W, H, hwnd, (HMENU) c[i].id, g_hInst, 0
		);
		SendDlgItemMessage(hwnd, c[i].id, WM_SETFONT, buttonFont, FALSE);
		SetClassLong(theCombo, GCL_HCURSOR, (long) *sr1Cur);
		Y += 25;
		hkComboCol(c[i], hwnd);
	}
}
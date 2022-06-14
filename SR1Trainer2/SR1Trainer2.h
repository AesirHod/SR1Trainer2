#ifndef INC_PRIMEMOVER_H
#define INC_PRIMEMOVER_H

#include <windows.h>
#include <commctrl.h>
#include "Types.h"
#include "process.h"

//#define SOUL_REAVER_FUNCTIONS (1)

enum ControlID
{
	SR1_ModifyHUDBtn = 1, SR1_ModifyHUDCmb,
	SR1_SubWin,
};

struct SR_Button
{
	char* text;
	int id;
};

struct SR_Combo
{
	int id;
	u_char defKey;
};

struct SR_Key
{
	int vk;
	char* name;
};

class SR1Trainer2
{
public:

	static void Initialise(HINSTANCE hInst);

	static LRESULT APIENTRY MainWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT APIENTRY MemWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static WPARAM ButtonFont();

	static void OnMenu(WPARAM);
	static void CreateHKComboCol(SR_Combo c, HWND hWnd);
	static void CreateKeyStrings();
	static void CreateMainWindow(int x, int y, int w, int h);
	static void CreateMemWindow(HWND hParent, int x, int y, int w, int h);
	static void CreateButtonCol(SR_Button* b, HWND hParent, int noOfButtons, int x, int y, int w, int h);
	static void CreateComboCol(SR_Combo* c, HWND hParent, int noOfButtons, int x, int y, int w, int h);

	static bool SetUpProcess();
	static void CleanUp();
	static void FindMe();

	static void WriteMem(HWND hWnd);
	static void ModifyHUD();
	static void ModifyHealthSpiral(u_int& uMemTracker,
		int iXOffset, int iYOffset, float fXScale, float fYScale);
	static void ModifyHealthGlow(u_int& uMemTracker,
		int iXOffset, int iYOffset, float fXScale, float fYScale,
		u_int uABGRSpectralColour, u_int uABGRMaterialColour);

	static HINSTANCE s_hInst;

	static HCURSOR* s_hSRCursor;
	static WPARAM s_hButtonFont;

	static HWND s_hMainWin;
	static HWND s_hMemWin;

	static CProcess s_xProcess;

	static SR_Key s_axKeys[];
	static SR_Button s_axButtons[];
	static SR_Combo s_axComboBoxes[];
	static u_char SR1_ModifyHUDKey;

	static u_int s_uAllocated;
	static u_int s_uMemTracker;

	static bool s_bInitialised;
};

#endif
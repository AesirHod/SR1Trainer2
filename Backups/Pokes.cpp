
#define ID_TIMER				1		// ID for timer
#define TIMER_INTERRUPT			500		// timer msg interval in msec

/* ---------------------------------------- */
/* -- The mem addies found with GameHack -- */
/* ---------------------------------------- */
#define DEBUG_MENU_ADDRESS		0x00534A9D
#define POSSESSION_ADDRESS		0x005322ED


/* ------------------------------------------ */
/* -- Global variables... oh well... ;)    -- */
/* ------------------------------------------ */

CProcess gameProcess;					// instance of the CProcess class

/* ------------------------------------------------------------- */

BOOL CALLBACK DialogProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
		case WM_INITDIALOG:
			// set the time to generate a timer message (WM_TIMER)
			// at a small interval
			SetTimer(hwnd, ID_TIMER, TIMER_INTERRUPT, NULL);
			return TRUE;

		case WM_TIMER:
			// a timer msg was received so call our main function!
			WriteMem(hwnd);
			return TRUE;

		case WM_DESTROY:
			// this app is about to be closed so kill the timer
			KillTimer(hwnd, ID_TIMER);
			return TRUE;

		case WM_PAINT:
			//
			// paint msg so we can show a bmp on the screen (or something else)
			//
			// PaintIt(hwnd);
			return TRUE;
    }
    return FALSE;
}


void WriteMem(HWND hwnd)
{
	// find out if we already found the game before
	bool bOk = gameProcess.IsRunning();
	// if not then try to find it
	if (!bOk)
		bOk = gameProcess.FindProcess("Soul Reaver");
	// if and only if game is running -> write mem
	if (bOk) {
		if (gameProcess.IsSpyRunning("TRAINER SPY"))
			// someone is trying to rip us off! Close this trainer
			PostQuitMessage(0);
		// Debug On
		if (gameProcess.IsKeyPressed(VK_F11))
			if (!gameProcess.WriteByte(DEBUG_MENU_ADDRESS, 0xFF))
				PostQuitMessage(0);
		//Debug Off
		if (gameProcess.IsKeyPressed(VK_F12))
			if (!gameProcess.WriteByte(DEBUG_MENU_ADDRESS, 0xFF))
				PostQuitMessage(0);

	}
}


/*void PaintIt(HWND hwnd)
{
	//
	// To do (or not to do :)
	//  = show a neat pic on your trainer's window
	//
}
*/
#include "global.h"

//win32 stuff
// HWND hWindow;
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

char szClassName[] = "WindowsApp";
HINSTANCE ghInstance;
HICON iWindow;
HFONT hFont, hNewFont;
HWND hwnd, inputArea;
MSG messages2; // INCLUDED SOMETHING
//THAT HAS MESSAGES ALREADY
WNDCLASSEX wincl;

u16 gameid=0, codeCount = 0;
u8 dogameid=1,region=0,fdevice=0,tdevice=0;
const char* hexValues = "0123456789abcdef";
const u8 ar2e1[] = {0x0E,0x3C,0x7D,0xF2,0x16,0x45,0xEB,0xB3};
const u8 ar2e2[] = {0x0E,0x3C,0x7D,0xF2,0x18,0x53,0xE5,0x9E};
const u8 ar2d1[] = {0xDE,0xAD,0xFA,0xCE,0x02,0x11,0x04,0x16};
const u8 ar2d2[] = {0xDE,0xAD,0xFA,0xCE,0x04,0x03,0x02,0x09};
const u32 cb2seeds[16][3] = {
         {0x00288596, 0x0A0B8D9B, 0x1DD9A10A},
         {0x0037DD28, 0x0A0133F8, 0xB95AB9B0},
         {0x003BEEF1, 0x0AF733EC, 0x5CF5D328},
         {0x000BC822, 0x0A15C574, 0x95FE7F10},
         {0x00BC935D, 0x0A50AC20, 0x8E2D6303},
         {0x00A139F2, 0x0A920FB9, 0x16BB6286},
         {0x00E9BBF8, 0x0A599F0B, 0xE389324C},
         {0x00F57F7B, 0x0A4AA0E3, 0x07AC6EA8},
         {0x0090D704, 0x0A21C012, 0xAA4811D8},
         {0x001814D4, 0x0A906254, 0x76CE4E18},
         {0x00C5848E, 0x0A31FD54, 0xFE447516},
         {0x005B83E7, 0x0A091C0E, 0xF9CD94D0},
         {0x00108CF7, 0x0A372B38, 0x4C24DEDB},
         {0x0046CE5A, 0x0A6F266C, 0x68275C4E},
         {0x003A5BF4, 0x0A61DD4A, 0x72494382},
         {0x006FAFFC, 0x0A0DBF92, 0xC8AA88E8}};
char* inputC;
char* outputC;
u32 rLen;
u16 withoutNulls;
u8* byteArray;
const u8 lineLength = 8;
Code* firstCode = new Code();
string testing;

#define INPUT 1
#define OUTPUT 2
#define CONVERT 3
#define CLOSE 4
#define CLEARIN 5
#define CLEAROUT 6
#define AUTOID 7
#define INPUTLABEL 8
#define OUTPUTLABEL 9
#define AUTOIDLABEL 10
#define GAMEIDLABEL 11
#define GAMEID 12
#define REIGONLABEL 13
#define REIGON 14
#define FDEVICE 15
#define TDEVICE 16
#define TOLABEL 17
#define FROMLABEL 18


string toString(long i) 
{ 
stringstream s; 
s << i; 
return s.str(); 
}

/*void encrypt () {
	testing = toString(emptyLine(20, 5));
	char* testing2 = (char*)testing.c_str();
    delete inputC;
	inputC = new char[testing.length()];
	inputC = testing2;
}*/

void convert(HWND hwndDlg) {
	inputC = new char[TEXTSIZE];
	memset(inputC,0,TEXTSIZE);
	GetDlgItemText(hwndDlg,INPUT,inputC,TEXTSIZE);
	fdevice = (u8) SendMessage(GetDlgItem(hwnd,FDEVICE),CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
	tdevice = (u8) SendMessage(GetDlgItem(hwnd,TDEVICE),CB_GETCURSEL,(WPARAM)0,(LPARAM)0);
	hexByte();
	// if (fdevice == 0) DecryptParseList();
	if (fdevice == 1) ar2Decrypt();
	if (fdevice == 2) decryption4();
    // if (tdevice == 0) EncryptParseList();
	if (tdevice == 1) ar2Encrypt();
	if (tdevice == 2) encryption4();
    byteHex();
    SetDlgItemText(hwndDlg,OUTPUT,outputC);
	delete inputC;
	delete outputC;
	delete byteArray;
}

void gcncryptKill() {
	DeleteObject(hNewFont);
	DestroyWindow(hwnd);
	hwnd=NULL;
}

LRESULT CALLBACK WindowProcedure (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
		case WM_CLOSE:
		case WM_QUIT:
			gcncryptKill();
			// return TRUE;
			break;
        case WM_DESTROY:
            PostQuitMessage(0);
		case WM_COMMAND:
			switch(HIWORD(wParam)) {
				case BN_CLICKED:
					switch(LOWORD(wParam)) {
						case CONVERT:
							convert(hwndDlg);
							break;
						case CLOSE:
							gcncryptKill();
							break;
						case CLEARIN:
							SetDlgItemText(hwndDlg,INPUT,"");
							break;
						case CLEAROUT:
							SetDlgItemText(hwndDlg,OUTPUT,"");
							break;
						case AUTOID:
							dogameid^=1;
							SendDlgItemMessage(hwndDlg,12,EM_SETREADONLY,dogameid^1,0);
							EnableWindow(GetDlgItem(hwndDlg,14),dogameid);
							break;
					}
					break;
			}
			break;
			default:
            return DefWindowProc (hwndDlg, uMsg, wParam, lParam);
	}
	return FALSE;
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil) {

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "MAXCrypt",          /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           588,                 /* The programs width */
           478,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

           CreateWindowEx (
           0,
           "Static",
           "Input:",
           0x50000000,
           9,39,76,16,
           hwnd,
           (HMENU) INPUTLABEL,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "Static",
           "Output:",
           0x50000000,
           300,39,76,16,
           hwnd,
           (HMENU) OUTPUTLABEL,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "Static",
           "Auto Identifier:",
           0x50000000,
           21,396,90,19,
           hwnd,
           (HMENU) AUTOIDLABEL,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "Static",
           "Reigon:",
           0x50000000,
           111,396,90,19,
           hwnd,
           (HMENU) REIGONLABEL,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "Static",
           "Game ID:",
           0x50000000,
           3,420,46,16,
           hwnd,
           (HMENU) GAMEIDLABEL,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "Button",
           "Clear",
           0x50010000,
           216,30,70,22,
           hwnd,
           (HMENU) CLEARIN,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "Button",
           "Clear",
           0x50010000,
           507,30,70,22,
           hwnd,
           (HMENU) CLEAROUT,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "Button",
           "Convert",
           0x50010000,
           387,420,70,22,
           hwnd,
           (HMENU) CONVERT,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "Button",
           "",
           0x50010003,
           3,396,16,16,
           hwnd,
           (HMENU) AUTOID,   // Checkbox
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "Button",
           "Close",
           0x50010000,
           507,420,70,22,
           hwnd,
           (HMENU) CLOSE,
           GetModuleHandle(NULL),
           NULL
           );
           
           inputArea = CreateWindowEx (
           0x00000200,
           "Edit",
           "",
           0x50210044,
           3,57,283,334,
           hwnd,
           (HMENU) INPUT,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0x00000200,
           "Edit",
           "",
           0x50210844,
           294,57,283,334,
           hwnd,
           (HMENU) OUTPUT,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0x00000200,
           "Edit",
           "",
           0x50010000,
           51,420,49,20,
           hwnd,
           (HMENU) GAMEID,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "ComboBox",
           "",
           0x50010003,
           111,420,82,90,
           hwnd,
           (HMENU) REIGON,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "ComboBox",
           "",
           0x50010003,
           204,420,84,100,
           hwnd,
           (HMENU) FDEVICE,
           GetModuleHandle(NULL),
           NULL
           );

           CreateWindowEx (
           0,
           "ComboBox",
           "",
           0x50010003,
           294,420,84,90,
           hwnd,
           (HMENU) TDEVICE,
           GetModuleHandle(NULL),
           NULL
           );
           
           CreateWindowEx (
           0,
           "Static",
           "To:",
           0x50000000,
           294,396,90,19,
           hwnd,
           (HMENU) TOLABEL,
           GetModuleHandle(NULL),
           NULL
           );
           
           CreateWindowEx (
           0,
           "Static",
           "From:",
           0x50000000,
           204,396,90,19,
           hwnd,
           (HMENU) FROMLABEL,
           GetModuleHandle(NULL),
           NULL
           );

			LOGFONT lf;

			hFont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
			GetObject(hFont, sizeof(LOGFONT), &lf);
			strcpy(lf.lfFaceName,"MS Sans Serif");
			lf.lfUnderline = 0;
			lf.lfStrikeOut = 0;
			lf.lfItalic = 0;
			lf.lfWeight = FW_THIN;
			lf.lfHeight = 6;
			lf.lfWidth = 6;
            lf.lfEscapement = 0;
			lf.lfOrientation = 0;
			// lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
			hNewFont = CreateFontIndirect(&lf);


			SendDlgItemMessage(hwnd,1,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,2,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,3,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,4,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,5,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,6,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,7,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,8,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,9,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,10,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,11,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,12,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,13,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,14,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,15,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,16,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,17,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,18,WM_SETFONT,(WPARAM)hNewFont,FALSE);

			hFont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
			GetObject(hFont, sizeof(LOGFONT), &lf);
			strcpy(lf.lfFaceName,"Courier");
			lf.lfUnderline = 0;
			lf.lfStrikeOut = 0;
			lf.lfItalic = 0;
			lf.lfWeight = FW_THIN;
			lf.lfHeight = 8;
			lf.lfWidth = 8;
            lf.lfEscapement = 0;
			lf.lfOrientation = 0;
			lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
			hNewFont = CreateFontIndirect(&lf);

			SendDlgItemMessage(hwnd,INPUT,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,OUTPUT,WM_SETFONT,(WPARAM)hNewFont,FALSE);
			SendDlgItemMessage(hwnd,GAMEID,WM_SETFONT,(WPARAM)hNewFont,FALSE);

			//set text limits
			// SendDlgItemMessage(hwnd,INPUT,EM_SETLIMITTEXT,(TEXTSIZE-1),0);
			// SendDlgItemMessage(hwnd,GAMEID,EM_SETLIMITTEXT,3,0);

			//tick checkbox
			CheckDlgButton(hwnd, AUTOID, BST_CHECKED);

			//fill region list box
			SendMessage(GetDlgItem(hwnd,REIGON),CB_ADDSTRING,(WPARAM)0,(LPARAM)("USA"));
			SendMessage(GetDlgItem(hwnd,REIGON),CB_ADDSTRING,(WPARAM)0,(LPARAM)("PAL"));
			SendMessage(GetDlgItem(hwnd,REIGON),CB_ADDSTRING,(WPARAM)0,(LPARAM)("Japan"));
			SendMessage(GetDlgItem(hwnd,REIGON),CB_ADDSTRING,(WPARAM)0,(LPARAM)("Null"));
			SendMessage(GetDlgItem(hwnd,REIGON),CB_SETCURSEL,(WPARAM)0,(LPARAM)0);

			SendMessage(GetDlgItem(hwnd,TDEVICE),CB_ADDSTRING,(WPARAM)0,(LPARAM)("AR MAX"));
			SendMessage(GetDlgItem(hwnd,TDEVICE),CB_ADDSTRING,(WPARAM)0,(LPARAM)("AR2"));
			SendMessage(GetDlgItem(hwnd,TDEVICE),CB_ADDSTRING,(WPARAM)0,(LPARAM)("CB2"));
			SendMessage(GetDlgItem(hwnd,TDEVICE),CB_ADDSTRING,(WPARAM)0,(LPARAM)("Hex"));
			SendMessage(GetDlgItem(hwnd,TDEVICE),CB_SETCURSEL,(WPARAM)0,(LPARAM)0);

			SendMessage(GetDlgItem(hwnd,FDEVICE),CB_ADDSTRING,(WPARAM)0,(LPARAM)("AR MAX"));
			SendMessage(GetDlgItem(hwnd,FDEVICE),CB_ADDSTRING,(WPARAM)0,(LPARAM)("AR2"));
			SendMessage(GetDlgItem(hwnd,FDEVICE),CB_ADDSTRING,(WPARAM)0,(LPARAM)("CB2"));
			SendMessage(GetDlgItem(hwnd,FDEVICE),CB_ADDSTRING,(WPARAM)0,(LPARAM)("Hex"));
			SendMessage(GetDlgItem(hwnd,FDEVICE),CB_SETCURSEL,(WPARAM)0,(LPARAM)0);

    ShowWindow (hwnd, nFunsterStil);
    while (GetMessage (&messages2, NULL, 0, 0)) {
        TranslateMessage(&messages2);
        DispatchMessage(&messages2);
    }
    return messages2.wParam;
}

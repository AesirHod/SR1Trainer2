// CProcess class based on code by Jared Bruni http://www.lostsidedead.com/gameprog

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "resource.h"
#include "Process.h"

// enumerated constants for the child handles
enum {ID_DebugOn = 1, ID_DebugOff, ID_SwimOn, ID_SwimOff, ID_SubWin, ID_TLWBK};

// Key configurations
BYTE dbgOn = VK_F11, dbgOff = VK_F12, swmOn = VK_F9, swmOff = VK_F10;
DWORD allocated = 0;
BYTE initialised = 0, dbgInit = 0x01, swmInit = 0x02, brcInit = 0x04, version = 0;

// Soul Reaver 1 variables
DWORD	RazielAdditionalCollide,FX_MakeWaterBubble,PhysicsCheckSwim,STREAM_GetLevelWithID,STREAM_GetWaterZLevel,
		FX_DrawGlowingLine, endOfMenu, endOfMenu2, mainMenu, saveMenu, razielMenu, cameraMenu, AIMenu, goodiesMenu,
		debugImbueSoulReaverMenu, debugSoundMenu, eventMenu, debugRazielFlags1, debugRazielFlags2, mainMenuStr,
		razielMenuStr, holdSRvrStr, specRvrStr, mateRvrStr, fireRvrStr, cameraMenuStr, statsAndGhostCam, menuState,
		lockRaziel;
// Soul Reaver 1 functions
void setupSR1(/*int version*/);
void debugOn();
void debugOff();
void swimOn();
void swimOff();
void blackCore();

// Main functions
LRESULT APIENTRY mainWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
LRESULT APIENTRY SubWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
WPARAM ButtonFont();
void onMenu(WPARAM);

HINSTANCE g_hInst;

HWND mainWin;
HWND hWindow;
HWND tlwbk;
HWND DebugOnButton;
HWND DebugOffButton;
HWND swimButton;
WNDCLASS wc;
// DWORD counter = 0;

#define ID_TIMER				1		// ID for timer
#define TIMER_INTERRUPT			500		// Poke every 500 milliseconds  THE THING THAT MAY SLOW YOU COMPUTER

CProcess SR1("Legacy of Kain: Soul Reaver");						// instance of the CProcess class

void setupSR1()
{
	// Check Version
	if (SR1.CmpString(0x004f1b50, "Oct 29 1999"))
		if (version == 1) return;
		else version = 1;
	else
		if (version == 2) return;
		else version = 2;

	switch(version)
	{
		case 1:
		{
			menuState = 0x00C66E92;
			lockRaziel = 0x00AF8A79;
			RazielAdditionalCollide = 0x004559E0;
			FX_MakeWaterBubble = 0x004A1BD0;
			PhysicsCheckSwim = 0x004BA020;
			STREAM_GetLevelWithID = 0x004d0fa0;
			STREAM_GetWaterZLevel = 0x004d1140;
			FX_DrawGlowingLine = 0x004A96D0;
			endOfMenu = 0x00B08388;
			endOfMenu2 = 0x00C66DE0;
			mainMenu = 0x004F5F58;
			saveMenu = mainMenu + 0x0120;
			razielMenu = mainMenu + 0x00000618;
			cameraMenu = 0x004f6de0;
			AIMenu = 0x004f6e40;
			eventMenu = 0x004f60d8;
			goodiesMenu = 0x004f6288;
			debugImbueSoulReaverMenu = 0x004f6978;
			debugSoundMenu = mainMenu + 0x04E0;
			debugRazielFlags1 = 0x00C644F4;
			debugRazielFlags2 = debugRazielFlags1 + 0x04;
			statsAndGhostCam = 0x00C66DE0;
			mainMenuStr = 0x004F8D78;
			razielMenuStr = 0x004F88AC;
			cameraMenuStr = 0x004F86EC;
			holdSRvrStr = 0x004F870C;
			specRvrStr = 0x004F87CC;
			mateRvrStr = 0x004F87BC;
			fireRvrStr = 0x004F87B0;
			break;
		}
		case 2:
		{
			menuState = 0x00C43912;
			lockRaziel = 0x00AD5789;
			RazielAdditionalCollide = 0x0044aed0;
			FX_MakeWaterBubble = 0x0048f330;
			PhysicsCheckSwim = 0x004a4890;
			STREAM_GetLevelWithID = 0x004b7550;
			STREAM_GetWaterZLevel = 0x004b76f0;
			FX_DrawGlowingLine = 0x00496050;
			endOfMenu = 0x00AE5098;
			endOfMenu2 = 0x00C43860;
			mainMenu = 0x004D4DC8;
			saveMenu = mainMenu + 0x0120;
			razielMenu = mainMenu + 0x00000618;
			cameraMenu = 0x004d5c50;
			AIMenu = 0x004d5cb0;
			eventMenu = 0x004d4f48;
			goodiesMenu = 0x004d50f8;
			debugImbueSoulReaverMenu = 0x004d57e8;
			debugSoundMenu = mainMenu + 0x04E0;
			debugRazielFlags1 = 0x00C41070;
			debugRazielFlags2 = debugRazielFlags1 + 0x04;
			statsAndGhostCam = 0x00C43860;
			mainMenuStr = 0x004D7BE8;
			razielMenuStr = 0x004D771C;
			cameraMenuStr = 0x004D755C;
			holdSRvrStr = 0x004D757C;
			specRvrStr = 0x004D763C;
			mateRvrStr = 0x004D762C;
			fireRvrStr = 0x004D7620;
			break;
		}
		default:
			break;
	}
	return;
}


DWORD projectile (DWORD projectile, DWORD particle_FX)
{
	// [PROJECTILE]
	SR1.WriteXString(projectile, "01 64 64 64 09 00 64 00 09 00 64 00 09 00 64 00");
	SR1.Write(projectile+0x10, particle_FX);
	SR1.Write(projectile+0x14, projectile+0x1C);	// Used to have value 0053B358
	SR1.WriteXString(projectile+0x18, "01 08 00 CD C7 01 8E 03 55 05 1C 07 E4 08 AB 0A 72 0C 39 0E 00 00 CD CD"/* 00 00 00 00"*/);
	return 0x40;
}
DWORD blast (DWORD blast, DWORD blast_FX)
{
	SR1.WriteXString(blast, "01 64 64 64 14 00 64 00 14 00 64 00 14 00 64 00");
	SR1.Write(blast+0x10, blast_FX);
	SR1.Write(blast+0x14, blast+0x1E);	// Used to have value ????????
	// Size of this array seems to correspond to the number of particle types.  Interesting:
	SR1.WriteXString(blast+0x18, "05 01 00 20 00 CD 00 20 00 04 04 60 11 00 23 50 E0 4F 01 00");
	return 0;
}

DWORD particles (DWORD newParticles, DWORD locations...)  // 401430
{
	DWORD* locs = &locations;
	DWORD noOfBytes = 0;
	DWORD* stack = 0;
	// Maybe push 0x00FF at the begining to protect against overflow???
	if (locations == 0x00FF)
	{
		SR1.Write(newParticles, (WORD) 0xFF00);
		return 0x02;
	}
    __asm
    {
		push EBX
		push ECX
		mov ECX, locs
Particle:
		mov EBX, [ECX]
		add ECX, 0x04
		cmp EBX, 0x00FF
		jne short Particle
		sub ECX, locs
		shl ECX, 0x01
		sub ECX, 0x06
		mov noOfBytes, ECX
    }
	BYTE* particleFX = new BYTE[noOfBytes];
	// particleFX is already in ECX.  No need to mov it.
    __asm
    {
		mov stack, ESP
		mov ESP, locs
AddFX:
		mov dword ptr [ECX], 0x01002001
		pop EBX
		and EBX, 0x0000FFFF
		// [For Rings]
		test EBX, 0x00000A00
		jne short Ring
		or EBX, 0x00100000
Ring:
		// [/For Rings]
		mov dword ptr [ECX+4], EBX
		add ECX, 0x08
		cmp [ESP], 0x00FF
		jne short AddFX
		pop word ptr [ECX]
		mov ESP, stack
		pop EBX
		pop ECX
	}
	swab((char*)particleFX, (char*)particleFX, noOfBytes);
	SR1.WriteArray(newParticles, particleFX, noOfBytes);
	return noOfBytes;
}

DWORD bParticles (DWORD newParticles, DWORD locations...)
{
	DWORD noOfBytes;
	// locations = 0x00FF;
	SR1.WriteXString(newParticles, "30 00 00 00 A2 01 7C 00 88 FF A8 61 30 00 00 00 0D 00 7C 00 00 00 A8 61");
	// particles(newParticles+0x18, locations);
	__asm
	{
		lea         eax,[ebp+0x0C]
		push        eax  
		mov         ecx,dword ptr [ebp+8] 
		add         ecx,0x18
		push        ecx  
		call        pFunc
		jmp			short endFunc
pFunc:
		//
		push        ebp
		mov         ebp,esp
		sub         esp,0xFC
		push        ebx
		push        esi
		push        edi
		lea         edi,[ebp+0xFFFFFF04] 
		mov         ecx,0x3F 
		mov         eax,0xCCCCCCCC
		rep stos    dword ptr [edi] 
		// DWORD* locs = &locations;
		mov         eax,[ebp+0x0C]
		//
		jmp        [particles+33]
endFunc:
		add         esp,8
		add         eax,0x18
		mov			noOfBytes, EAX
	}
	return noOfBytes;
}
void debugOn()
{
	if (!allocated) 
	{
		allocated = SR1.Alloc(500);
	}

	if (allocated && !(initialised & dbgInit))
	{{
		// BUILD EXTENDED IMBUE MENU
		initialised |= dbgInit;

		SR1.WriteString(allocated+0xA0, "Water Reaver");
		SR1.WriteString(allocated+0xB0, "Sunlight Reaver");
		SR1.WriteString(allocated+0xC0, "Sound Reaver");
		SR1.WriteString(allocated+0xD0, "Ariel Reaver");
		SR1.WriteString(allocated+0xE0, "Kain Reaver");

		DWORD menuOption = allocated+0xF0;
		SR1.Write(mainMenu + 0x000006A0, menuOption);

		DWORD optVals[] = {0x00000400, 0x00000800, 0x000008000, 0x00002000,
						   0x00004000,0x00020000,0x00001000,0x00010000};

		DWORD optNames[] = {specRvrStr, mateRvrStr, fireRvrStr,
			allocated+0xA0, allocated+0xB0, allocated+0xC0,
			allocated+0xD0, allocated+0xE0};

		SR1.Write(menuOption, 0x00000004);				// 04 so it knows the address is a submenu
		SR1.Write(menuOption+0x0C, mainMenuStr);		// To main menu
		SR1.Write(menuOption+0x10, mainMenu);
		menuOption+=0x18;

		SR1.Write(menuOption, 0x00000004);				// 04 so it knows the address is a submenu
		SR1.Write(menuOption+0x0C, razielMenuStr);		// Needs to be able to go home
		SR1.Write(menuOption+0x10, razielMenu);
		menuOption+=0x18;

		SR1.Write(menuOption+0x0C, holdSRvrStr);		// Hold Soul Reaver
		SR1.Write(menuOption+0x10, debugRazielFlags1);
		SR1.Write(menuOption+0x14, 0x08);

		for (int i=0; i<8; i++)							// Loop through toggle menu
		{
			menuOption+=0x18;
			// No optType needed cause toggles are zeros and zero tells it they are toggle options.
			// I assume zero is default when the dynamic memory is allocated
			// SR1.Write(menuOption, 0x00000000);
			SR1.Write(menuOption+0x0C, optNames[i]);			// Reavers Name
			SR1.Write(menuOption+0x10, debugRazielFlags2);		// Reavers Address
			SR1.Write(menuOption+0x14, optVals[i]);				// Reavers Number
		}

		menuOption+=0x18;
		SR1.Write(menuOption, 0x00000005);						// The 05 tells it that it's the end of the menu
		SR1.Write(menuOption+0x0C, endOfMenu);
		SR1.Write(menuOption+0x10, endOfMenu2);

		// REBUILD COMPLETE MENU FROM BETA VERSION

		menuOption = allocated+0x0230;

		DWORD betaNames[] =
		{
			SR1.WriteString(menuOption+0x10, "Short Stats"),
			cameraMenuStr,
			SR1.WriteString(menuOption+0x20, "AI/Script Menu..."),
			razielMenuStr,
			SR1.WriteString(menuOption+0x40, "Ghost Camera"),
			SR1.WriteString(menuOption+0x50, "Goodies Menu..."),
			SR1.WriteString(menuOption, "Sound Menu..."),
			SR1.WriteString(menuOption+0x60, "Debug CD Loads"),
			SR1.WriteString(menuOption+0x70, "Save Menu..."),
			SR1.WriteString(menuOption+0x80, "Debug Events"),
		};

		DWORD betaTypes[]  = {0, 4, 4, 4, 0, 4, 4, 0, 4, 4, 5};
		DWORD betaOpts[]   = {statsAndGhostCam, cameraMenu, AIMenu, razielMenu, statsAndGhostCam, 
			goodiesMenu, debugSoundMenu, statsAndGhostCam, debugImbueSoulReaverMenu, eventMenu, endOfMenu};
		DWORD arguements[] = {4, 0, 0, 0, 0x00010000, 0, 0, 0x80000000, 0, 0, endOfMenu2};

		SR1.Write(mainMenu+0xF0, (BYTE)0x04);				// The 4 tells it to look for a menu
		SR1.Write(mainMenu+0xFC, menuOption);				// Points to the string
		SR1.Write(mainMenu+0x0100, debugSoundMenu);			// Points to the Submenu
		SR1.Write(mainMenu+0x0105, (BYTE)0x00);				// Gets rid of old code

		BYTE* moveMenu = SR1.ReadArray(saveMenu, 6*4*4);
		SR1.WriteArray(debugImbueSoulReaverMenu, moveMenu, 6*4*4);

		for (int i=0; i<11; i++)
		{
			SR1.Write(mainMenu+(6*4*(i+4)), betaTypes[i]);
			SR1.Write(mainMenu+(6*4*(i+4))+0x0C, betaNames[i]);
			SR1.Write(mainMenu+(6*4*(i+4))+0x10, betaOpts[i]);// Error???  I think I got it.
			SR1.Write(mainMenu+(6*4*(i+4))+0x14, arguements[i]);
		}

		if (version == 1)
		{
			DWORD newParticles = menuOption + 0x100;
			// [PARTICLES]																							Velocity    Colour
			/*Kain Rising  */ SR1.WriteXString(newParticles+=0x00, "60 00 01 00 80 00 00 00 00 01 00 01 00 01 00 00 01 01 01 00 A8 18 18 00 00 00 00 01 FF FF 10 00 00 00 FF FF 01 00 64 00 00 00 00 00 00 00 00 10");
			/*Kain Black   */ SR1.WriteXString(newParticles+=0x30, "90 00 01 01 80 00 00 00 80 00 80 00 80 00 00 00 00 00 00 00 40 40 40 01 00 00 00 00 FF FF 0F 00 00 00 FF FF FF 00 64 00 00 00 00 00 00 00 00 10");
			/*Kain Falling */ SR1.WriteXString(newParticles+=0x30, "90 00 01 00 80 00 01 00 00 00 00 00 00 00 00 00 00 00 FF 00 A8 18 18 00 00 00 00 01 FF FF 1E 00 00 00 FF FF FE 00 64 00 00 00 00 00 00 00 00 10");
			/*Kain Ring    */ SR1.WriteXString(newParticles+=0x30, "00 00 FF FF 00 00 00 00 00 00 01 00 03 01 14 00 60 00 C0 00 00 02 80 01 3C 00 00 00 00 00 00 00 38 FF 38 FF 38 FF 00 00 A8 10 10 00 00 00 00 00");
			/*Water Splash */ SR1.WriteXString(newParticles+=0x30, "50 00 01 01 64 00 00 00 2C 01 2C 01 2C 01 00 00 00 00 FE 00 14 32 80 00 14 14 3C 00 FF FF 0F 00 0B 00 FF FF 02 00 C8 00 00 00 00 00 00 00 00 10");
			/*Water Main   */ SR1.WriteXString(newParticles+=0x30, "64 00 01 00 82 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 00 0E 64 A8 00 00 00 00 00 FF FF 1E 00 00 00 FF FF 02 00 78 00 00 00 00 00 00 00 00 10");
			/*Water Ripple?*/ // SR1.WriteXString(newParticles+=0x30, "3C 00 01 00 30 00 01 00 5C 00 5C 00 5C 00 00 00 00 00 FF 00 07 32 54 00 00 00 00 03 FF FF 0A 00 05 00 FF FF 02 00 00 00 00 00 00 00 00 00 00 10");
			/*Water Falling*/ SR1.WriteXString(newParticles+=0x30, "90 00 01 00 80 00 01 00 00 00 00 00 00 00 00 00 00 00 FD 00 0E 64 A8 00 14 14 3C 00 FF FF 1E 00 00 00 FF FF FE 00 64 00 00 00 00 00 00 00 00 10");
			/*Water Ring   */ SR1.WriteXString(newParticles+=0x30, "00 00 FF FF 00 00 00 00 00 00 01 00 03 01 14 00 60 00 C0 00 00 02 80 01 3C 00 00 00 00 00 00 00 38 FF 38 FF 38 FF 00 00 0E 64 A8 00 00 00 00 00");
			newParticles+=0x30;
			// [/PARTICLES]


			// [BLASTRING CHOICE FUNCTION]
			
			SR1.WriteXString(newParticles, "B9 FF FF FF FF 3B 4E 1C 8B 74 24 40 8B 40 1C 75 07 B9");
			SR1.Write(newParticles+0x12, menuOption+0x100);
			SR1.WriteXString(newParticles+0x16, "EB 03 8B 48 10 E9");
			SR1.Write(newParticles+0x1C, 0x004a7752 - (newParticles+0x20));
			
			SR1.Write(0x004A7748, (BYTE) 0xE9);
			SR1.Write(0x004A7749, newParticles - 0x004A774D/*0x004A7748*/);
			SR1.WriteXString(0x004A774D, "90 90 90 90 90");
			
			newParticles+=0x40;
			// [/BLASTRING CHOICE FUNCTION]

			// [PARTICLE CHOICE FUNCTION]
			SR1.WriteXString(newParticles, "B8 FF FF FF FF 39 46 1C 8B 44 24 1C 8B 51 1C 75 07 BF");
			SR1.Write(newParticles+0x12, menuOption+0x100);
			SR1.WriteXString(newParticles+0x16, "EB 02 8B 3A E9");
			SR1.Write(newParticles+0x1B, 0x004a7046 - (newParticles+0x1F));

			SR1.Write(0x004A703D, (BYTE) 0xE9);
			SR1.Write(0x004A703E, newParticles - 0x004A7042);
			SR1.Write(0x004A7042, 0x90909090);
			// [/PARTICLE CHOICE FUNCTION]


			BYTE* ffsArray = new BYTE[44];		// Need something better than the ffsArray
			memset(ffsArray, 0xFF, 44);

			DWORD projectiles[] = {
				0x0053B2B0, // 0
				0x0053B2CC, // 1
				0x0053B2EC, // 2
				newParticles+0xA0+0x46, // */0x0053B30C, // 3
				newParticles+0xA0+0xB0+0xD0+0x46, // */0x0053B33C, // 4
				0x0053B36C, // 5
				newParticles+0xA0+0xB0+0xD0+0xB0, // */0x0053B39C, // 6
				newParticles+0xA0+0xB0, // */0x0053B3C8, // 7
				0x0053B3F4, // 8
				0x0053B420, // 9
				0x0053B450, // A
				0x0053B47C, // B
				0x0053B4AC, // C
				0x0053B4D8, // D
				newParticles+0xA0,
				newParticles+0xA0+0xB0
			};
			SR1.WriteArray(newParticles+0x30, projectiles, 0x0E);
			SR1.Write(0x0053ACD0, newParticles+0x30);  // If this is false when imbuing, then make it true and copy the array.
			// Do it automatically the first time.
			newParticles+=0xA0;

			// [PROJECTILE]
			// List of particles used by the projectile:  (length = 0x1A)
			// SR1.WriteXString(newParticles, "20 01 01 00 05 00 00 10 20 01 01 00 05 01 00 10 20 01 01 00 05 02 00 10 00 FF");
			particles(newParticles, 0x0500, 0x0501, 0x0502, 0x00FF);
			// Data used to determine particle location:
			SR1.WriteArray(newParticles+0x1A, ffsArray, 44);
			projectile(newParticles+0x46, newParticles);
			newParticles = newParticles + 0xB0;
			// [/PROJECTILE]
			// [PROJECTILE]
			blast(newParticles, newParticles+0x40);
			// List of particles used by the projectile:																			       05					   02					   03
			// SR1.WriteXString(newParticles+0x40, "30 00 00 00 A2 01 7C 00 88 FF A8 61 30 00 00 00 0D 00 7C 00 00 00 A8 61 20 01 01 00 0F 03 00 00 20 01 01 00 05 02 01 10 20 01 01 00 05 01 01 10 00 FF");
			bParticles(newParticles+0x40, 0x0F03, 0x0502, 0x0501, 0x00FF);
			// Data used to determine particle location:
			SR1.WriteArray(newParticles+0x72, ffsArray, 44);
			newParticles+=0xD0;
			// [/PROJECTILE]
			// [PROJECTILE]
			// List of particles used by the projectile:   28					   29
			// SR1.WriteXString(newParticles, "20 01 01 00 05 04 00 10 20 01 01 00 05 05 00 10 20 01 01 00 05 06 00 10 00 FF");
			particles(newParticles, 0x0504, 0x0505, 0x0506, 0x00FF);
			// Data used to determine particle location:
			SR1.WriteArray(newParticles+0x1A, ffsArray, 44);
			projectile(newParticles+0x46, newParticles);
			newParticles = newParticles + 0xB0;
			// [/PROJECTILE]
			// [PROJECTILE]
			blast(newParticles, newParticles+0x40);
			// List of particles used by the projectile:
			// SR1.WriteXString(newParticles+0x40, "30 00 00 00 A2 01 7C 00 88 FF A8 61 30 00 00 00 0D 00 7C 00 00 00 A8 61 20 01 01 00 0F 07 00 00 20 01 01 00 05 04 01 10 20 01 01 00 05 05 01 10 00 FF");
			bParticles(newParticles+0x40, 0x0F07, 0x0504, 0x0505, 0x00FF);
			// Data used to determine particle location:
			SR1.WriteArray(newParticles+0x72, ffsArray, 44);
			newParticles+=0xD0;
			// [/PROJECTILE]

			// SR1.WriteXString(0x0053B5E0, "01 FF 0E 0F");                                         .
			/*SR1.WriteXString(newParticles, "8B 44 24 08 53 25 FF 00 00 00 56 57 8B 7C 24 18 8D 0C 40 8B 44 24 10 C1 E1 04 57 50 8D B4 01 B8 01 00 00 B8");
			SR1.Write(newParticles+0x24, projectiles[4]);
			SR1.Write(newParticles+0x28, (BYTE) 0xE9);
			SR1.Write(newParticles+0x29, 0x004cef88-(newParticles+0x2D));*/
			// OK, It works when I add that.  Now I need to include:  if it's greater than 0x0D then use 0x04.
		}
	}}

	if (SR1.ReadArray(menuState,1)[0]==0x00)
	SR1.Write(menuState, (BYTE)0x04);	// Activate the menu
}

void debugOff()
{
	if (SR1.ReadArray(menuState,1)[0]==0x04)
	{
		SR1.Write(menuState, (BYTE) 0x00);	// Deactivates the menu.  Used to cause shadowlessness.  Fixed now.
		SR1.Write(lockRaziel, (BYTE) 0x06); // Lets Raz move again
	}
}

void swimOn()
{
	if (!allocated) allocated = SR1.Alloc(500);
	if (allocated && !(initialised & swmInit))
	{
		initialised |= swmInit;
		SR1.WriteXString(allocated, "83 EC 3C 53 55 56 8B 74 24 4C 57 8B 46 38 50 E8 2C 00 00 00 83 C4 04 56 50 E8 29 00 00 00 8B C8 8B 46 40 8B 6C 24 5C 83 C4 08 8B 58 3C 89 4C 24 50 2B D9 81 F9 01 80 FF FF BB 72 A0 4B 00 FF E3 B8 A0 0F 4D 00 FF E0 B8 40 11 4D 00 FF E0 ");
		SR1.Write(allocated+0x3A, PhysicsCheckSwim+0x52);
		SR1.Write(allocated+0x41, STREAM_GetLevelWithID);
		SR1.Write(allocated+0x48, STREAM_GetWaterZLevel);
	}
	SR1.Write(FX_MakeWaterBubble, 0xC3);
	SR1.Write(RazielAdditionalCollide+0x278,
		allocated-(RazielAdditionalCollide+0x278+4));
}

void swimOff()
{
	if (allocated && (initialised & swmInit))
	{			
		SR1.Write(RazielAdditionalCollide+0x278,
			PhysicsCheckSwim - (RazielAdditionalCollide+0x278+4));
		SR1.Write(FX_MakeWaterBubble, 0x53);
	}
}

void blackCore()
{
	if (!allocated) allocated = SR1.Alloc(500);
	if (allocated && !(initialised & brcInit))
	{
		initialised |= brcInit;
		SR1.WriteXString(allocated+0x70, "83 E1 00 BE 01 00 00 00 81 C9 00 00 00 39 C7 40 08 00 00 00 39 89 48 18 89 48 28 BA");
		SR1.Write(allocated+0x1C+0x70, FX_DrawGlowingLine+0x69);
		SR1.Write(allocated+0x20+0x70, 0xE2FF);
		SR1.Write(FX_DrawGlowingLine+0x47, 0xBA);
		SR1.Write(FX_DrawGlowingLine+0x48, allocated+0x70);
		SR1.Write(FX_DrawGlowingLine+0x4C, 0xE2FF);
	}
}
void WriteMem(HWND hwnd)
{
	if (SR1.FindProcess())
	{
		setupSR1();
		// Debug On
		if (SR1.IsKeyPressed(dbgOn))	debugOn();
		// Debug Off
		if (SR1.IsKeyPressed(dbgOff))	debugOff();
		// Flood On
		if (SR1.IsKeyPressed(swmOn))	swimOn();
		// Flood Off
		if (SR1.IsKeyPressed(swmOff))	swimOff();
		// Black Coil
		if (SR1.IsKeyPressed(VK_F8))	blackCore();
	}
}

int APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR line,int CmdShow)
{
	g_hInst = hInst;
	MSG msg;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.lpfnWndProc = (WNDPROC) mainWindowProc;
	wc.lpszClassName = "MainWindow";
	wc.lpszMenuName = MAKEINTRESOURCE(0);//IDR_MENU1);
	wc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);

	wc.lpszMenuName = NULL;
	wc.lpszClassName = "SubWindow";
	wc.lpfnWndProc = (WNDPROC) SubWindowProc;
	wc.hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON2));

	RegisterClass(&wc);

	mainWin = CreateWindow("MainWindow","New Trainer Test",WS_OVERLAPPEDWINDOW,0,0,640,480,0,0,hInst,0);

	ShowWindow(mainWin, SW_MAXIMIZE);
	ShowWindow(mainWin,SW_SHOW);
	// ShowWindow(mainWin,SW_HIDE);  // Invisible window
	UpdateWindow(mainWin);

	while(GetMessage(&msg,0,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


LRESULT APIENTRY mainWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

	switch(msg) {
		case WM_DESTROY:
			if (allocated) SR1.Free(allocated);
			KillTimer(hwnd, ID_TIMER);
			PostQuitMessage(0);
			break;
		case WM_CREATE: {
			SetTimer(hwnd, ID_TIMER, TIMER_INTERRUPT, NULL);

			hWindow = CreateWindowEx (
				0x00000200, "SubWindow", "", WS_VISIBLE | WS_CHILD | WS_MAXIMIZE,
				// | WS_OVERLAPPEDWINDOW, // WS_CAPTION | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
				100,100,100,100, hwnd, (HMENU) ID_SubWin, g_hInst, 0
			);
			tlwbk = CreateWindow("Static",NULL,WS_CHILD | WS_VISIBLE | SS_BITMAP,0,0,0,0,hWindow,(HMENU) ID_TLWBK,g_hInst,0);
			SendMessage(tlwbk,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(HBITMAP)LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP1)));

			DebugOnButton = CreateWindowEx(
				0, "Button", "Debug On", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				50,30,100,20, hWindow, (HMENU)ID_DebugOn, g_hInst, 0
			);
			SendDlgItemMessage(hWindow,ID_DebugOn,WM_SETFONT,ButtonFont(),FALSE);

			DebugOffButton = CreateWindowEx(
				0, "Button", "Debug Off", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				50,55,100,20, hWindow, (HMENU)ID_DebugOff, g_hInst, 0
			);
			SendDlgItemMessage(hWindow,ID_DebugOff,WM_SETFONT,ButtonFont(),FALSE);

			swimButton = CreateWindowEx(
				0, "Button", "Swim On", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				50,80,100,20, hWindow, (HMENU)ID_SwimOn, g_hInst, 0
			);
			SendDlgItemMessage(hWindow,ID_SwimOn,WM_SETFONT,ButtonFont(),FALSE);
			SendDlgItemMessage(hWindow,ID_SwimOn,WM_SETFONT,ButtonFont(),FALSE);

			swimButton = CreateWindowEx(
				0, "Button", "Swim Off", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
				50,105,100,20, hWindow, (HMENU)ID_SwimOff, g_hInst, 0
			);
			SendDlgItemMessage(hWindow,ID_SwimOff,WM_SETFONT,ButtonFont(),FALSE);
			ShowWindow(hWindow, SW_MAXIMIZE);
		}
		break;
		// case WM_PAINT:
		case WM_SIZE: {
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			LPRECT rcParent = (LPRECT)(LPARAM)&rcClient;
			MoveWindow(
				hWindow, 
				0, 
				0, 
				rcParent->right, 
				rcParent->bottom, 
				TRUE
			); 
			UpdateWindow(hWindow);
			break;
		}

		case WM_TIMER:
			// The function to poke the codes:
			WriteMem(hwnd);
			break;

		case WM_COMMAND:
				onMenu(wParam);
				switch(HIWORD(wParam)) {

				case BN_CLICKED: // Thing was clicked
					/*switch(LOWORD(wParam)) {
						//case ID_SubWin:
						//break;
					}*/
					break;

				case LBN_DBLCLK: // Thing was double clicked
					/*switch(LOWORD(wParam)) {
						//case ID_LIST:
						//break;
					}*/
					break;

				case EN_CHANGE:  // Thing with data had changed
					/*switch(LOWORD(wParam)) {
						//case ID_EDIT:
						//break;
					}*/
					break;
			}
			break;
		default:
			//if(time(0) > counter+(1000/25)) {
			//	WriteMem(hwnd);
			//	counter = time(0);
			//}
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}

void onMenu(WPARAM wParam)
{
	switch(wParam)
	{
	case ID_FILE_EXIT:
		SendMessage(mainWin,WM_CLOSE,0,0);
		break;
	case ID_FILE_MSGABOUT:
		ShowWindow(hWindow, SW_MAXIMIZE);
		break;
	case ID_FILE_WINDOWABOUT:
		break;
	}


}

LRESULT APIENTRY SubWindowProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
		ShowWindow(hwnd,SW_HIDE);
		break;
	case WM_CREATE:
	    	break;
	case WM_COMMAND:
		switch(HIWORD(wParam))
		{
			case BN_CLICKED:
				{
					if (SR1.FindProcess())
					{
						setupSR1();
						if ((LOWORD(wParam))==ID_DebugOn)	debugOn();
						if ((LOWORD(wParam))==ID_DebugOff)	debugOff();
						if ((LOWORD(wParam))==ID_SwimOn)	swimOn();
						if ((LOWORD(wParam))==ID_SwimOff)	swimOff();
					}
				}
		}
		break;
	default: return DefWindowProc(hwnd,msg,wParam,lParam);
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

			// aboutwin = CreateWindow("About:)","About main", WS_OVERLAPPED | WS_SYSMENU |WS_MINIMIZEBOX,0,0,250,250,0,0,hInst,0);
			// HWND stx;
			// stx = CreateWindow("Static","A Random Static Label",WS_CHILD | WS_VISIBLE ,0,0,200,25,hwnd,0,g_hInst,0);
			// static_label = CreateWindow("Static","This static label",WS_CHILD | WS_VISIBLE,205,0,200,25,hwnd,0,g_hInst,0);
			// edit = CreateWindow("Edit","A Windows Edit Box",WS_BORDER | WS_VSCROLL | WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL,5,35,200,200,hwnd,(HMENU)ID_EDIT,g_hInst,0);
			// list = CreateWindow("ListBox",NULL, LBS_NOTIFY | WS_CHILD | WS_VISIBLE | WS_VSCROLL|WS_BORDER,5,250,200,200,hwnd,(HMENU)ID_LIST,g_hInst,0);
			// button = CreateWindow("Button","About",BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE ,350,5,100,25,hwnd,(HMENU)ID_BUTTON,g_hInst,0);
			// static_image = CreateWindow("Static",NULL,WS_CHILD | WS_VISIBLE | SS_BITMAP,350,40,0,0,hwnd,(HMENU)ID_DebugOff,g_hInst,0);
			// SendMessage(static_image,STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)(HBITMAP)LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP1)));
			// combo = CreateWindow("ComboBox",NULL,WS_BORDER | CBS_DISABLENOSCROLL | WS_CHILD | CBS_DROPDOWNLIST | WS_VISIBLE,350,400,200,200,hwnd,(HMENU)ID_COMBO,g_hInst,0);
			// SendMessage(combo,CB_ADDSTRING,255,(LPARAM)(LPCSTR)"addstuff");
			// hWindow = CreateDialog(g_hInst,"IDD_DIALOG1 (Dialog)",NULL,0);


//  For x86:

void DoSomething(DWORD address, DWORD locations...)
{
    // DWORD localVar;
	// DWORD result;
	DWORD* locs = &locations;

    __asm
    {
		push EAX
		push EBX
		push ECX
		push EDX
		xor ECX, locs
Particle:
		mov EBX, [ECX]
		cmp EBX, 0x00FF
		je short Finish
        push 20010100
		and EBX, 0xFFFF0000
		or EBX, 0x00100000
		push EBX
		jne short Particle
Finish:
		push EBX
		pop EAX
		pop EBX
		pop ECX
		pop EDX
		
    }
}

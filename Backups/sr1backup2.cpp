// #include "process.h"
#include "sr1.h"

DWORD allocated = 0, allocated2 = 0;
BYTE initialised = 0, dbgInit = 0x01, swmInit = 0x02, brcInit = 0x04, version = 0;

// Soul Reaver 1 variables
DWORD RazielAdditionalCollide,FX_MakeWaterBubble,PhysicsCheckSwim,STREAM_GetLevelWithID,STREAM_GetWaterZLevel,
FX_DrawGlowingLine, endOfMenu, statsAndGhostCam, mainMenu, saveMenu, razielMenu, cameraMenu, AIMenu,
goodiesMenu, debugImbueSoulReaverMenu, debugSoundMenu, eventMenu, debugRazielFlags1, debugRazielFlags2,
debugRazielFlags3, mainMenuStr, razielMenuStr, holdSRvrStr, specRvrStr, mateRvrStr, fireRvrStr, cameraMenuStr,
menuState, lockRaziel, objectAccess, FX_StartGenericParticle, FX_StartGenericBlastRing, CollideReaverProjectile,
TrailWaterFX, MEMPACK_Malloc, optsMenu, CityMenu, levelSelectMenu, level2SelectMenu, cheatsMenu, renderSetupMenu,
debugSpecialAbilitiesMenu, INSTANCE_BirthObject, memmove_SR1, PHYSOB_BirthProjectile, do_start_game, FX_Spiral,
debugForgedAbilitiesMenu, RazielInit, DEBUG_Process, draw_menu, handle_line_type_bit, MainG2, PCLoadSavedGame,
FONT_Print, MENU_SELECT_RIGHT, InitStates, HealthUpgradeNo, ManaLevel, debugHealthLevel, debugManaLevel,
MonsterCaptionFlag, yesStr, noStr, curStr, menudefs_pause_menu, debugHealthSystemMenu, SpectralState;

enum menuTypes {
	HLBit, HLLong, HLAction, HLActionLn, HLMenu, HLEndMenu
};

struct menuItem {
	DWORD type;
	DWORD arg1;
	DWORD arg2;
	DWORD text;
	DWORD address1;
	DWORD address2;
};

// Soul Reaver 1 functions
/*
void projectiles(DWORD location);
void setupSR1();
void debugOn();
void debugOff();
void swimOn();
void swimOff();
void blackCore();
*/

CProcess SR1("Legacy of Kain: Soul Reaver");                        // instance of the CProcess class

void findMe() {
	DWORD *address;
	__asm
	{
		mov eax, ebp;
		add eax, 0x04;
		mov [address],eax;
		mov eax, 00000001;
	}
	char* errInfo = new char[50];
	sprintf(errInfo,"Made it to address 0x%08X without crashing!", *address);
	// sprintf("Reached address %08x before crashing", errInfo, address);
	MessageBox(NULL, errInfo, "ERROR", MB_ICONERROR | MB_OK);
}

void setupSR1()
{
	SR1.FindProcess();

	if (SR1.WasUpdated())
	{
	// Check Version
	if		(SR1.CmpString(0x004d0a68, "Aug 30 1999"))	version = 1;
	else if (SR1.CmpString(0x004f1b50, "Oct 29 1999"))	version = 2;
	else if (version == 0) return;
	else version = 0;

	switch(version)
	{
		case 1:
			{
				menuState = 0x00C43912;
				lockRaziel = 0x00AD5789;
				RazielAdditionalCollide = 0x0044aed0;
				FX_MakeWaterBubble = 0x0048f330;
				TrailWaterFX = 0x0044c370;
				PhysicsCheckSwim = 0x004a4890;
				STREAM_GetLevelWithID = 0x004b7550;
				STREAM_GetWaterZLevel = 0x004b76f0;
				FX_DrawGlowingLine = 0x00496050;
				endOfMenu = 0x00AE5098;
				statsAndGhostCam = 0x00C43860;
				MonsterCaptionFlag = statsAndGhostCam+0x04;
				mainMenu = 0x004D4DC8;
				saveMenu = mainMenu + 0x0120;
				razielMenu = mainMenu + 0x00000618;
				cameraMenu = 0x004d5c50;
				AIMenu = 0x004d5cb0;
				eventMenu = 0x004d4f48;
				goodiesMenu = 0x004d50f8;
				debugImbueSoulReaverMenu = 0x004d57e8;
				debugSoundMenu = mainMenu + 0x04E0;
				CityMenu = 0x004d6418;
				optsMenu = 0x004d5968;
				debugRazielFlags1 = 0x00C41070;
				debugRazielFlags2 = debugRazielFlags1 + 0x04;
				debugRazielFlags3 = debugRazielFlags2 + 0x04;
				mainMenuStr = 0x004D7BE8;
				razielMenuStr = 0x004D771C;
				cameraMenuStr = 0x004D755C;
				holdSRvrStr = 0x004D757C;
				specRvrStr = 0x004D763C;
				mateRvrStr = 0x004D762C;
				fireRvrStr = 0x004D7620;
				FX_StartGenericBlastRing = 0x00494410;
				FX_StartGenericParticle = 0x00493d00;
				CollideReaverProjectile = 0x004ae1a0;
				objectAccess = 0x004d9718;
				MEMPACK_Malloc = 0x004a0f20;
				levelSelectMenu = 0x004d6988;
				level2SelectMenu = 0x004d6880;
				cheatsMenu = 0x004d5890;
				renderSetupMenu = 0x004d6c58;
				INSTANCE_BirthObject = 0x0049d6d0;
				PHYSOB_BirthProjectile = 0x004a60f0;
				memmove_SR1 = 0x004c0dc0;
				do_start_game = 0x00470740;
				FX_Spiral = 0x00495450;
				debugSpecialAbilitiesMenu = 0x004d5548;
				debugForgedAbilitiesMenu = 0x004d5638;
				RazielInit = 0x0044bb00;
				DEBUG_Process = 0x004824d0;
				draw_menu = 0x00482e00;
				handle_line_type_bit = 0x00482c50;
				MainG2 = 0x0042f360;
				PCLoadSavedGame = 0x004afde0;
				FONT_Print = 0x0048c700;
				MENU_SELECT_RIGHT = 0x0046c840;
				InitStates = 0x00436120;
				HealthUpgradeNo = 0x00AE5520;
				ManaLevel = 0x00AE5538;
				debugHealthLevel = 0x004d4dbc;
				debugManaLevel = debugHealthLevel+0x04;
				noStr = 0x004d7c24;
				yesStr = noStr+0x04;
				curStr = 0x004d4a20;
				menudefs_pause_menu = 0x00C438E4;
				debugHealthSystemMenu = 0x004d54a0;
				SpectralState = 0x00ae5560;
				break;
			}
		case 2:
			{
				menuState = 0x00C66E92;
				lockRaziel = 0x00AF8A79;
				RazielAdditionalCollide = 0x004559E0;
				FX_MakeWaterBubble = 0x004A1BD0;
				TrailWaterFX = 0x00456e80;
				PhysicsCheckSwim = 0x004BA020;
				STREAM_GetLevelWithID = 0x004d0fa0;
				STREAM_GetWaterZLevel = 0x004d1140;
				FX_DrawGlowingLine = 0x004A96D0;
				endOfMenu = 0x00B08388;
				statsAndGhostCam = 0x00C66DE0;
				MonsterCaptionFlag = statsAndGhostCam+0x04;
				mainMenu = 0x004F5F58;
				saveMenu = mainMenu + 0x0120;
				razielMenu = mainMenu + 0x00000618;
				cameraMenu = 0x004f6de0;
				AIMenu = 0x004f6e40;
				eventMenu = 0x004f60d8;
				goodiesMenu = 0x004f6288;
				debugImbueSoulReaverMenu = 0x004f6978;
				debugSoundMenu = mainMenu + 0x04E0;
				optsMenu = 0x004f6af8;
				CityMenu = 0x004f75a8;
				debugRazielFlags1 = 0x00C644F4;
				debugRazielFlags2 = debugRazielFlags1 + 0x04;
				debugRazielFlags3 = debugRazielFlags2 + 0x04;
				mainMenuStr = 0x004F8D78;
				razielMenuStr = 0x004F88AC;
				cameraMenuStr = 0x004F86EC;
				holdSRvrStr = 0x004F870C;
				specRvrStr = 0x004F87CC;
				mateRvrStr = 0x004F87BC;
				fireRvrStr = 0x004F87B0;
				FX_StartGenericBlastRing = 0x004a7730;
				FX_StartGenericParticle = 0x004a7020;
				CollideReaverProjectile = 0x004c4be0;
				objectAccess = 0x004fa9e8;
				MEMPACK_Malloc = 0x004b5c40;
				levelSelectMenu = 0x004f7b18;
				level2SelectMenu = 0x004f7a10;
				cheatsMenu = 0x004f6a20;
				renderSetupMenu = 0x004f7de8;
				INSTANCE_BirthObject = 0x004b1630;
				PHYSOB_BirthProjectile = 0x004bbbd0;
				memmove_SR1 = 0x004dcd60;
				do_start_game = 0x0047e9c0;
				FX_Spiral = 0x004a89a0;
				debugSpecialAbilitiesMenu = 0x004f66d8;
				debugForgedAbilitiesMenu = 0x004f67c8;
				RazielInit = 0x00456610;
				DEBUG_Process = 0x00491f10;
				draw_menu = 0x00492840;
				handle_line_type_bit = 0x00492690;
				MainG2 = 0x00439880;
				PCLoadSavedGame = 0x004c6c20;
				FONT_Print = 0x0049e290;
				MENU_SELECT_RIGHT = 0x0047a530;
				InitStates = 0x00440A80;
				HealthUpgradeNo = 0x00B08810;
				ManaLevel = 0x00B08828;
				debugHealthLevel = 0x004f5f4c;
				debugManaLevel = debugHealthLevel+0x04;
				noStr = 0x004f8db4;
				yesStr = noStr + 0x04;
				curStr = 0x004F5BB4;
				menudefs_pause_menu = 0x00c66e64;
				debugHealthSystemMenu = 0x004f6630;
				SpectralState = 0x00b08850;
				break;
			}
		default:
			break;
	}

	allocated = SR1.Read((DWORD*)(MEMPACK_Malloc + 0x58));		//  But what if it's 90909090
	if (!version || allocated==0x90909090)	{
		allocated=0;
		allocated2=0;
		initialised=0;
	}
    else initialised = SR1.Read((BYTE*)(MEMPACK_Malloc + 0x5C));

}
	return;
}

void cleanUp() {
	swimOff();
	debugOff();
	// if (allocated) SR1.Free(allocated);
	if (version) {
		SR1.Write((MEMPACK_Malloc + 0x58), allocated);
		SR1.Write((MEMPACK_Malloc + 0x5C), initialised/*(DWORD)0*/);
	}
}

DWORD particles (DWORD  newParticles, DWORD* locations)
{
	// 0x004012f0 in release build
	if (locations[0] == 0x00FF)
	{
		SR1.Write(newParticles, (WORD) 0xFF00);
		return 0x02;
	}
	DWORD locs = 0;
	while (locations[locs] != 0x00FF) locs++;
	locs *= 0x08;
	locs += 0x02;
	BYTE* particleFX = new BYTE[locs];
	/*DWORD* question = (DWORD*) particleFX;
	while (*locations != 0x00FF) {
	*question = 0x01002001;
	question+=0x01;
	if (*locations & 0x0000A000) 
	*question = (*locations & 0x00000FFF);
	else  *question = ((*locations & 0x00000FFF) | 0x00100000);
	question+=0x01;
	locations+=0x01;
	}
	WORD* question2 = (WORD*) question;
	*question2 = (WORD) *locations;*/
	__asm
	{
		push eax;
		push ebx;
		push ecx;
		mov eax, locations;
		mov ecx, particleFX;
AddFX:
		mov dword ptr [ecx], 0x01002001;
		mov ebx, [eax];
		and EBX, 0x00000FFF;
		// [For Rings]
		test EBX, 0x00000A00;
		jne short Ring;
		or EBX, 0x00100000;
Ring:
		// [/For Rings]
		mov [ecx+0x04], ebx;
		add ecx, 0x08;
		add eax, 0x04;
		cmp [eax], 0x00FF;
		jne short AddFX;
		mov word ptr [ecx], 0x00FF;
		pop ecx;
		pop ebx;
		pop eax;
	}
	swab((char*)particleFX, (char*)particleFX, locs);		// Swap every other byte
	SR1.Write(newParticles, particleFX, locs);
	return locs;
}
DWORD forceFX (DWORD newParticles,  DWORD fxLocations...)
{
	// [PROJECTILE] 0x004013E0 in release build
	SR1.WriteXString(newParticles, "01 64 64 64 09 00 64 00 09 00 64 00 09 00 64 00");
	SR1.Write(newParticles+0x10, newParticles+0x30);
	SR1.Write(newParticles+0x14, newParticles+0x1C);    // Used to have value 0053B358
	SR1.WriteXString(newParticles+0x18, "01 08 00 CD C7 01 8E 03 55 05 1C 07 E4 08 AB 0A 72 0C 39 0E 00 00 CD CD");
	particles (newParticles+0x30, &fxLocations);
	return 0;
}
DWORD blastFX (DWORD blastData, DWORD fxLocations...)
{
	SR1.WriteXString(blastData, "01 64 64 64 14 00 64 00 14 00 64 00 14 00 64 00");
	SR1.Write(blastData+0x10, blastData+0x40);
	SR1.Write(blastData+0x14, blastData+0x1E);
	// Size of this array seems to correspond to the number of particle types.  Interesting:
	SR1.WriteXString(blastData+0x18, "05 01 00 20 00 CD 00 20 00 04 04 60 11 00 23 50 E0 4F 01 00");
	SR1.WriteXString(blastData+=0x40, "30 00 00 00 A2 01 7C 00 88 FF A8 61 30 00 00 00 0D 00 7C 00 00 00 A8 61");
	particles(blastData+=0x18, &fxLocations);
	return blastData+=0x18;
}
void projectiles(DWORD  location)
{
	DWORD newParticles = location;
	// [PARTICLES]                                          Size                                            Velocity    Colour      Texture		Fade/Speed
	/*Kain Rising  */ SR1.WriteXString(newParticles+=0x00, "60 00 01 00 80 00 00 00 00 01 00 01 00 01 00 00 01 01 01 00 A8 18 18 00 00 00 00 01 FF FF 10 00 00 00 FF FF 01 00 64 00 00 00 00 00 00 00 00 10");
	/*Kain Black   */ SR1.WriteXString(newParticles+=0x30, "90 00 01 01 80 00 00 00 80 00 80 00 80 00 00 00 00 00 00 00 40 40 40 01 00 00 00 00 FF FF 0F 00 00 00 FF FF FF 00 64 00 00 00 00 00 00 00 00 10");
	/*Kain Falling */ SR1.WriteXString(newParticles+=0x30, "90 00 01 00 80 00 01 00 00 00 00 00 00 00 00 00 00 00 FF 00 A8 18 18 00 00 00 00 01 FF FF 1E 00 00 00 FF FF FE 00 64 00 00 00 00 00 00 00 00 10");
	/*Kain Ring    */ SR1.WriteXString(newParticles+=0x30, "00 00 FF FF 00 00 00 00 00 00 01 00 03 01 14 00 60 00 C0 00 00 02 80 01 3C 00 00 00 00 00 00 00 38 FF 38 FF 38 FF 00 00 A8 10 10 00 00 00 00 00");
	/*Water Splash */ SR1.WriteXString(newParticles+=0x30, "50 00 01 01 64 00 00 00 2C 01 2C 01 2C 01 00 00 00 00 FE 00 14 32 80 00 14 14 3C 00 FF FF 0F 00 0B 00 FF FF 02 00 C8 00 00 00 00 00 00 00 00 10");
	/*Water Main   */ SR1.WriteXString(newParticles+=0x30, "64 00 01 00 82 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 00 0E 64 A8 00 00 00 00 00 FF FF 1E 00 00 00 FF FF 02 00 78 00 00 00 00 00 00 00 00 10");
	/*Water Falling*/ SR1.WriteXString(newParticles+=0x30, "90 00 01 00 80 00 01 00 00 00 00 00 00 00 00 00 00 00 FD 00 0E 64 A8 00 14 14 3C 00 FF FF 1E 00 00 00 FF FF FE 00 64 00 00 00 00 00 00 00 00 10");
	/*Water Ring   */ SR1.WriteXString(newParticles+=0x30, "00 00 FF FF 00 00 00 00 00 00 01 00 03 01 14 00 60 00 C0 00 00 02 80 01 3C 00 00 00 00 00 00 00 38 FF 38 FF 38 FF 00 00 0E 64 A8 00 00 00 00 00");
	/*Sunlight A   */ SR1.WriteXString(newParticles+=0x30, "90 00 01 01 B0 00 01 00 80 00 80 00 80 00 00 00 00 00 00 00 A8 A8 A8 00 00 00 00 01 08 08 08 00 00 00 FF FF 01 00 64 00 00 00 00 00 00 00 00 10");
	/*Sunlight B   */ SR1.WriteXString(newParticles+=0x30, "78 00 01 00 C8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A8 A8 A8 00 00 00 00 01 1E 1E 1E 00 00 00 FF FF 01 00 64 00 00 00 00 00 00 00 00 10");
	/*Sunlight Orb */ SR1.WriteXString(newParticles+=0x30, "90 00 01 01 80 00 00 00 80 00 80 00 80 00 00 00 00 00 00 00 A8 A8 A8 00 00 00 00 01 FF FF 0F 00 00 00 FF FF FF 00 64 00 00 00 00 00 00 00 00 10");
	/*Sunlight Ring*/ SR1.WriteXString(newParticles+=0x30, "00 00 FF FF 00 00 00 00 00 00 01 00 03 01 14 00 60 00 C0 00 00 02 80 01 3C 00 00 00 00 00 00 00 38 FF 38 FF 38 FF 00 00 80 80 80 00 00 00 00 00");
	/*Sound Small  */ SR1.WriteXString(newParticles+=0x30, "64 00 01 00 82 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 A8 10 A8 00 00 00 00 00 FF FF 1E 00 00 00 FF FF 02 00 78 00 00 00 00 00 00 00 00 10");
	/*Sound Large  */ SR1.WriteXString(newParticles+=0x30, "78 00 01 00 C8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 86 2E 9C 00 00 00 00 00 1E FF FF 00 00 00 FF FF 01 00 64 00 00 00 00 00 00 00 00 10");
	/*Sound Large 2*/ SR1.WriteXString(newParticles+=0x30, "90 00 01 01 B0 00 01 00 80 00 80 00 80 00 00 00 00 00 00 00 54 1E 64 00 00 00 00 00 08 FF FF 00 00 00 FF FF 01 00 64 00 00 00 00 00 00 00 00 10");
	/*Sound Dusty  */ SR1.WriteXString(newParticles+=0x30, "20 00 01 00 80 00 01 00 40 01 40 01 40 01 00 00 00 00 00 00 86 1E 96 00 00 00 00 00 1E FF FF 00 00 00 FF FF 01 00 64 00 00 00 00 00 00 00 00 10");
	/*Useless Ring */ newParticles+=0x30;//SR1.WriteXString(newParticles+=0x30, "00 00 FF FF 00 00 00 00 00 00 01 00 03 01 14 00 60 00 C0 00 00 02 80 01 3C 00 00 00 00 00 00 00 38 FF 38 FF 38 FF 00 00 0E 64 A8 00 00 00 00 00");
	/*Ariel Rising */ SR1.WriteXString(newParticles+=0x00, "60 00 01 00 80 00 00 00 00 01 00 01 00 01 00 00 01 01 01 00 A8 A8 80 00 00 00 00 01 FF FF 10 00 00 00 FF FF 01 00 64 00 00 00 00 00 00 00 00 10");
	/*Ariel Black  */ SR1.WriteXString(newParticles+=0x30, "90 00 01 01 80 00 00 00 80 00 80 00 80 00 00 00 00 00 00 00 A8 A8 80 00 00 00 00 00 FF FF 0F 00 00 00 FF FF FF 00 64 00 00 00 00 00 00 00 00 10");
	/*Ariel Falling*/ SR1.WriteXString(newParticles+=0x30, "90 00 01 00 80 00 01 00 00 00 00 00 00 00 00 00 00 00 FF 00 A8 A8 80 00 00 00 00 01 FF FF 1E 00 00 00 FF FF FE 00 64 00 00 00 00 00 00 00 00 10");
	/*Ariel Ring   */ SR1.WriteXString(newParticles+=0x30, "00 00 FF FF 00 00 00 00 00 00 01 00 03 01 14 00 60 00 C0 00 00 02 80 01 3C 00 00 00 00 00 00 00 38 FF 38 FF 38 FF 00 00 A8 A8 80 00 00 00 00 00");
	/*Sound Ring 2 */ SR1.WriteXString(newParticles+=0x30, "00 00 FF FF 00 00 00 00 00 00 01 00 03 01 14 00 60 00 C0 00 00 02 80 01 3C 00 00 00 00 00 00 00 38 FF 38 FF 38 FF 00 00 A8 10 A8 00 00 00 00 00");
	/*Water Ripple?*/ // SR1.WriteXString(newParticles+=0x30, "3C 00 01 00 30 00 01 00 5C 00 5C 00 5C 00 00 00 00 00 FF 00 07 32 54 00 00 00 00 03 FF FF 0A 00 05 00 FF FF 02 00 00 00 00 00 00 00 00 00 00 10");
	/*Fire A	   */ SR1.WriteXString(newParticles+=0x30, "60 00 01 01 B0 00 01 00 80 00 80 00 80 00 00 00 00 00 00 00 FC 80 20 00 00 00 00 08 FF FF 08 00 00 00 FF FF 01 00 64 00 00 00 00 00 00 00 00 10");
	/*Fire B	   */ SR1.WriteXString(newParticles+=0x30, "50 00 01 00 C8 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FC 80 20 00 00 00 00 08 FF FF 1E 00 00 00 FF FF 01 00 64 00 00 00 00 00 00 00 00 10");
	/*Fire Ring	   */ SR1.WriteXString(newParticles+=0x30, "00 00 FF FF 00 00 00 00 00 00 01 00 03 01 14 00 60 00 C0 00 00 02 80 01 3C 00 00 00 00 00 00 00 38 FF 38 FF 38 FF 00 00 F0 43 18 00 00 00 00 00");
	// Fire use A0 80 00
	newParticles+=0x30;
	// [/PARTICLES]

	// [BLASTRING CHOICE FUNCTION]
	SR1.WriteXString(newParticles, "B9 FF FF FF FF 3B 4E 1C 8B 74 24 40 8B 40 1C 75 07 B9");
	SR1.Write(newParticles+0x12, location);
	SR1.WriteXString(newParticles+0x16, "EB 03 8B 48 10 E9 ");  // Needs the space at the end???
	SR1.Write(newParticles+0x1C, (FX_StartGenericBlastRing + 0x22) - (newParticles+0x20));
	SR1.Write((FX_StartGenericBlastRing + 0x18), (BYTE) 0xE9);
	SR1.Write((FX_StartGenericBlastRing + 0x19), newParticles - (FX_StartGenericBlastRing + 0x1D));
	SR1.WriteXString((FX_StartGenericBlastRing + 0x1D), "90 90 90 90 90");
	newParticles+=0x40;
	// [/BLASTRING CHOICE FUNCTION]

	// [PARTICLE CHOICE FUNCTION]
	SR1.WriteXString(newParticles, "B8 FF FF FF FF 39 46 1C 8B 44 24 1C 8B 51 1C 75 07 BF");
	SR1.Write(newParticles+0x12, location);
	SR1.WriteXString(newParticles+0x16, "EB 02 8B 3A E9");
	SR1.Write(newParticles+0x1B, (FX_StartGenericParticle + 0x26) - (newParticles+0x1F));
	SR1.Write((FX_StartGenericParticle + 0x1D), (BYTE) 0xE9);
	SR1.Write((FX_StartGenericParticle + 0x1E), newParticles - (FX_StartGenericParticle + 0x22));
	SR1.Write((FX_StartGenericParticle + 0x22), 0x90909090);
	newParticles+=0x030;
	// [/PARTICLE CHOICE FUNCTION]

	BYTE* ffsArray = new BYTE[44];      // Need something better than the ffsArray
	memset(ffsArray, 0xFF, 44);
	DWORD* projectiles = new DWORD[0x1A];

	if (!projectiles) {
		MessageBox(NULL, "Failed to create projectile array!", "ERROR", MB_ICONERROR | MB_OK);
		return;
	}

	// [PROJECTILES]
	// Water Projectile
	projectiles[0x0E]=newParticles;
	forceFX(newParticles, 0x0504, 0x0505, 0x0506, 0x00FF);
	SR1.Write(newParticles+0x4A, ffsArray, 44);
	newParticles+=0x80;
	// Water Blast
	projectiles[0x0F]=newParticles;
	blastFX(newParticles, 0x0F07, 0x0504, 0x0505, 0x00FF);
	SR1.Write(newParticles+0x72, ffsArray, 44);
	newParticles+=0xA0;

	// Sunlight Projectile
	projectiles[0x10]=newParticles;
	forceFX(newParticles, 0x0508, 0x0509,/* 0x050A,*/ 0x00FF);
	SR1.Write(newParticles+0x4A, ffsArray, 44);
	newParticles+=0x80;
	// Sunlight Blast
	projectiles[0x11]=newParticles;
	blastFX(newParticles, 0x0F0B, 0x0508, 0x0509, 0x00FF);
	SR1.Write(newParticles+0x72, ffsArray, 44);
	newParticles+=0xA0;

	// Sound Projectile
	projectiles[0x12]=newParticles;
	forceFX(newParticles, 0x050C, 0x050D, 0x050E, 0x00FF);
	SR1.Write(newParticles+0x4A, ffsArray, 44);
	newParticles+=0x80;
	// Sound Blast
	projectiles[0x13]=newParticles;
	blastFX(newParticles, 0x0F14, 0x050F, 0x050C, 0x00FF);
	SR1.Write(newParticles+0x72, ffsArray, 44);
	newParticles+=0xA0;

	// Kain Projectile
	projectiles[0x14]=newParticles;
	forceFX(newParticles, 0x0500, 0x0501, 0x0502, 0x00FF);
	SR1.Write(newParticles+0x4A, ffsArray, 44);
	newParticles+=0x80;
	// Kain Blast
	projectiles[0x15]=newParticles;
	blastFX(newParticles, 0x0F03, 0x0502, 0x0501, 0x00FF);
	SR1.Write(newParticles+0x72, ffsArray, 44);
	newParticles+=0xA0;

	// Ariel Projectile
	projectiles[0x16]=newParticles;
	forceFX(newParticles, 0x0510, 0x0511, 0x0512, 0x00FF);
	SR1.Write(newParticles+0x4A, ffsArray, 44);
	newParticles+=0x80;
	// Ariel Blast
	projectiles[0x17]=newParticles;
	blastFX(newParticles, 0x0F13, 0x0512, 0x0511, 0x00FF);
	SR1.Write(newParticles+0x72, ffsArray, 44);
	newParticles+=0xA0;

	// Fire Projectile
	projectiles[0x18]=newParticles;
	forceFX(newParticles, 0x0515, 0x0516,/* 0x0512,*/ 0x00FF);
	SR1.Write(newParticles+0x4A, ffsArray, 44);
	newParticles+=0x80;
	// Fire Blast
	projectiles[0x19]=newParticles;
	blastFX(newParticles, 0x0F17, 0x0516, 0x0515, 0x00FF);
	SR1.Write(newParticles+0x72, ffsArray, 44);
	newParticles+=0xA0;
	// [/PROJECTILES]

	DWORD pArray = newParticles;
	SR1.Write(pArray, projectiles, 0x1A);
	newParticles+=0x6A;

	// [DAMAGE EFFECTS FUNCTION]
	SR1.WriteXString(newParticles, "89 5C 24 F8 BB 00 00 08 00 3C 02 75 03 80 C3 05 3C 03 75 03 80 C3 04 3C 04 75 03 80 C3 06 3C 05 75 03 80 C3 05 3C 06 75 03 80 C3 05 3C 07 75 03 80 C3 03 53 8B 5C 24 FC E9");
	SR1.Write(newParticles+0x39, (CollideReaverProjectile + 0x4F) - (newParticles + 0x3D));
	SR1.WriteXString(CollideReaverProjectile + 0x42, "3C 01 7C 14 74 3B 90 56 E9");
	SR1.Write(CollideReaverProjectile + 0x4B, newParticles - (CollideReaverProjectile + 0x4F));
	SR1.Write(CollideReaverProjectile + 0x4B, newParticles - (CollideReaverProjectile + 0x4F));
	SR1.Write(CollideReaverProjectile+0x58, (WORD) 0x9090);		// Should make humans effected...
	newParticles+=0x40;
	// [/DAMAGE EFFECTS FUNCTION]

	// [PROJECTILES UPDATE FUNCTION]
	SR1.WriteXString(newParticles,		"52 55 50 83 C0 10 8B 00 BA");
	SR1.Write		(newParticles+0x09, pArray);												// The new projectile array
	SR1.WriteXString(newParticles+0x0D, "39 D0 74 43 68 34 00 00 00 50 52 E8");
	SR1.Write		(newParticles+0x19, memmove_SR1-(newParticles+0x1D));						// Call memmove
	SR1.WriteXString(newParticles+0x1D, "5A 58 5D 8B 04 24 89 50 10 05 1C 00 00 00 8B 00 8D 40 0C");
	SR1.WriteXString(newParticles+0x30, "8B 50 14 C7 02 01FF1617");
	SR1.WriteXString(newParticles+0x39, "8B 50 18 C7 02 01FF0E0F");
	SR1.WriteXString(newParticles+0x42, "8B 50 1C C7 02 01FF1011");
	SR1.WriteXString(newParticles+0x4B, "8B 50 28 C7 02 01FF1213");
	SR1.WriteXString(newParticles+0x54, "8B 50 24 C7 02 01FF1415");
	SR1.WriteXString(newParticles+0x5D, "8B 50 20 C7 02 01FF1819");
	SR1.WriteXString(newParticles+0x66, "58 5D 5A E9");
	SR1.Write		(newParticles+0x6A, INSTANCE_BirthObject-(newParticles+0x6E));				// Goto INSTANCE_BirthObject
	SR1.Write		(PHYSOB_BirthProjectile+0x35, newParticles-(PHYSOB_BirthProjectile+0x39));	// Call getProjectiles
	// [/PROJECTILES UPDATE FUNCTION]
}

void betaSpiral() {
	if (!allocated2) {
		allocated2 = SR1.Alloc(128);
	}
	// SR1.WriteXString(allocated2, "83 3D");
	SR1.Write(allocated2, (WORD) 0x3D83);
	SR1.Write(allocated2+0x02, SpectralState);
	SR1.WriteXString(allocated2+0x06, "01 74 07 68 00 40 00 00 EB 05 68 00 00 40 00 E9");
	SR1.Write(allocated2+0x16, (FX_Spiral+0x0574)-(allocated2+0x1A));
	SR1.Write((FX_Spiral+0x056F), (BYTE)0xE9);
	SR1.Write((FX_Spiral+0x0570), allocated2-(FX_Spiral+0x0574));
}

void infoFix() {
	if (!allocated2) {
		allocated2 = SR1.Alloc(128);
	}
	// Keep Health/Mana Levels up to date.		// Works
	DWORD next = allocated2;
	SR1.WriteXString(next+=0x1A, "90 90 8A 05");
	SR1.Write(next+=0x04, HealthUpgradeNo);
	SR1.WriteXString(next+=0x04, "88 05");
	SR1.Write(next+=0x02, debugHealthLevel);
	SR1.WriteXString(next+=0x04, "8A 05");
	SR1.Write(next+=0x02, ManaLevel+0x02);
	SR1.WriteXString(next+=0x04, "C0 E8 02");
	SR1.WriteXString(next+=0x03, "88 05");
	SR1.Write(next+=0x02, debugManaLevel);
	SR1.WriteXString(next+=0x04, "C3 90 90");

	SR1.Write(draw_menu-0x05, (BYTE) 0xE8);
	SR1.Write(draw_menu-0x04, (allocated2+0x1C)-draw_menu);
	SR1.Write(DEBUG_Process+0xAB, (draw_menu-0x05)-(DEBUG_Process+0xAB)-0x04);
	SR1.Write(DEBUG_Process+0x049C, (draw_menu-0x05)-(DEBUG_Process+0x049C)-0x04);
	
	// Allow for invincible working		// Works
	BYTE* nops = new BYTE[44];
	memset(nops, 0x90, 29);
	SR1.Write(draw_menu+0x02B6, (BYTE) 0xE8);
	SR1.Write(draw_menu+0x02B7, (next+0x03)-(draw_menu+0x02BB));
	SR1.Write(draw_menu+0x02BB, nops, 29);
	SR1.WriteXString(next+=0x03, "F6 40 08 01 8B 50 10 8B 48 14 8B 02 74 08 21 C8 3B C1 74 08 EB 0D 21 C8 3B C1 74 07 68");
	SR1.Write(next+=0x1D, noStr);
	SR1.WriteXString(next+=0x04, "EB 05 68");
	SR1.Write(next+=0x03, yesStr);
	SR1.Write(next+=0x04, 0xE8);
	SR1.Write(next+=0x01, FONT_Print-(next+4));
	SR1.WriteXString(next+=4, "83 C4 04 C3 90 90");

	SR1.Write(debugHealthSystemMenu+sizeof(menuItem[4])+0x08, 0x00000001);		// Works
	SR1.Write(debugHealthSystemMenu+sizeof(menuItem[4])+0x14, 0x00000800);

	//	[REALLY KEEP INFO BETWEEN LOADS]
	SR1.Write(RazielInit+0xAB, (next+6)-(RazielInit+0xAF));
	SR1.Write(next+=0x06, (WORD) 0x35FF);
	SR1.Write(next+=0x02, debugRazielFlags1);
	SR1.Write(next+=0x04, (WORD) 0x35FF);
	SR1.Write(next+=0x02, debugRazielFlags2);
	SR1.Write(next+=0x04, (WORD) 0x35FF);
	SR1.Write(next+=0x02, debugRazielFlags3);
	SR1.Write(next+=0x04, (WORD) 0x35FF);
	SR1.Write(next+=0x02, statsAndGhostCam);
	SR1.Write(next+=0x04, (WORD) 0x35FF);
	SR1.Write(next+=0x02, MonsterCaptionFlag);
	SR1.Write(next+=0x04, (WORD) 0x35FF);
	SR1.Write(next+=0x02, HealthUpgradeNo);
	SR1.Write(next+=0x04, (WORD) 0x35FF);
	SR1.Write(next+=0x02, ManaLevel);
	SR1.Write(next+=0x04, (BYTE) 0xB9);
	SR1.Write(next+=0x01, InitStates);
	SR1.WriteXString(next+=0x04, "56 FF D1 5E F7 05");
	SR1.Write(next+=0x06, menudefs_pause_menu);
	SR1.WriteXString(next+=0x04, "00 00 20 00 75 04 83 C4 1C C3");
	SR1.Write(next+=0x0A, (WORD) 0x058F);
	SR1.Write(next+=0x02, ManaLevel);
	SR1.Write(next+=0x04, (WORD) 0x058F);
	SR1.Write(next+=0x02, HealthUpgradeNo);
	SR1.Write(next+=0x04, (WORD) 0x058F);
	SR1.Write(next+=0x02, MonsterCaptionFlag);
	SR1.Write(next+=0x04, (WORD) 0x058F);
	SR1.Write(next+=0x02, statsAndGhostCam);
	SR1.Write(next+=0x04, (WORD) 0x058F);
	SR1.Write(next+=0x02, debugRazielFlags3);
	SR1.Write(next+=0x04, (WORD) 0x058F);
	SR1.Write(next+=0x02, debugRazielFlags2);
	SR1.Write(next+=0x04, (WORD) 0x058F);
	SR1.Write(next+=0x02, debugRazielFlags1);
	SR1.Write(next+=0x04, (BYTE) 0xC3);

	//	[DEACTIVATE KEEP INFO WHEN LOADING A SAVE OR RESTARTING]
	/*	SR1.Write(0x00438E51, (WORD) 0x4374);		// jmp 	0x00438E96			// Starting a new game always turns it off but loading doesn't...
		SR1.WriteXString(0x00438E96, "80 25 66 6E C6 00 DF EB BE");
	*///SR1.WriteXString(0x00438E51, "90 90 81 25 64 6E C6 00 FF FF DF FF");	// Same again but without loading condition
	//SR1.Write(0x0043A053, (WORD)0x0AEB);	// Always skip switch off
	if (version==2)			SR1.Write(MainG2+0x07A5, (WORD) 0x3874);			// Switch Keep_Info off if playing intro
	else if (version==1)	SR1.Write(MainG2+0x0784, (WORD) 0x3874);
	SR1.WriteXString(PCLoadSavedGame+0xE8, "81 E6 FF FF DF FF");	// Switch Keep_Info off if loading a save

	// [NEW TOGGLE BIT CONTROL]		// Works
	SR1.WriteXString(handle_line_type_bit-0x05, "59 09 08 5E C3");
	SR1.WriteXString(handle_line_type_bit+0x1F, "51 23 08 3B 0C 24 75 D4 59 F7 D1 21 08 5E C3 56 E8");
	SR1.Write(handle_line_type_bit+0x30, MENU_SELECT_RIGHT-(handle_line_type_bit+0x34));
	SR1.WriteXString(handle_line_type_bit+0x34, "83 C4 04 85 C0 75 DA 5E C3 90 90 90");
	SR1.Write(handle_line_type_bit+0x13, (WORD) 0x1974);

	SR1.Write(curStr, (BYTE) 0xD2);		// Clearer Curser
}

void debugOn()
{
	setupSR1();
	if (!SR1.IsRunning()) return;

	if (!allocated)	allocated = SR1.Alloc(1024);
	
	if (allocated && !(initialised & dbgInit))
	{{
		// BUILD EXTENDED IMBUE MENU
		initialised |= dbgInit;

		betaSpiral();
		infoFix();

		menuItem mainMI  =	{HLMenu,	NULL,	NULL,	mainMenuStr,	mainMenu,			NULL};
		menuItem end_MI =	{HLEndMenu,	NULL,	NULL,	endOfMenu,		statsAndGhostCam,	NULL};

		menuItem rvrMenu[] = {
			{HLMenu,	0,	0,	mainMenuStr,										mainMenu,			0x00000000},
			{HLMenu,	0,	0,	razielMenuStr,										razielMenu,			0x00000000},
			{HLBit,		0,	0,	holdSRvrStr,										debugRazielFlags1,	0x00000400},
			{HLBit,		0,	0,	specRvrStr,											debugRazielFlags2,	0x00000400},
			{HLBit,		0,	0,	mateRvrStr,											debugRazielFlags2,	0x00000800},
			{HLBit,		0,	0,	fireRvrStr,											debugRazielFlags2,	0x00008000},
			{HLBit,		0,	0,	SR1.WriteString(allocated+0xA0, "Water Reaver"),	debugRazielFlags2,	0x00002000},
			{HLBit,		0,	0,	SR1.WriteString(allocated+0xB0, "Sunlight Reaver"),	debugRazielFlags2,	0x00004000},
			{HLBit,		0,	0,	SR1.WriteString(allocated+0xC0, "Sound Reaver"),	debugRazielFlags2,	0x00020000},
			{HLBit,		0,	0,	SR1.WriteString(allocated+0xD0, "Ariel Reaver"),	debugRazielFlags2,	0x00001000},
			{HLBit,		0,	0,	SR1.WriteString(allocated+0xE0, "Kain Reaver"),		debugRazielFlags2,	0x00010000},
			{HLEndMenu,	0,	0,	endOfMenu,											statsAndGhostCam,	0x00000000}
		};

		DWORD menuOption = allocated+0xF0;
		SR1.Write(mainMenu + 0x000006A0, menuOption);
		SR1.Write(menuOption,	(BYTE*) rvrMenu, sizeof(menuItem[12]));	

		// REBUILD COMPLETE MENU FROM BETA VERSION

		menuOption = allocated+0x0230;

		menuItem mMenu[] = {
			{HLBit,		0,	0,	SR1.WriteString(menuOption+0x10, "Short Stats"),		statsAndGhostCam,			0x00000004},
			{HLMenu,	0,	0,	cameraMenuStr,											cameraMenu,					0x00000000},
			{HLMenu,	0,	0,	SR1.WriteString(menuOption+0x20, "AI/Script Menu..."),	AIMenu,						0x00000000},
			{HLMenu,	0,	0,	razielMenuStr,											razielMenu,					0x00000000},
			{HLBit,		0,	0,	SR1.WriteString(menuOption+0x40, "Ghost Camera"),		statsAndGhostCam,			0x00010000},
			{HLMenu,	0,	0,	SR1.WriteString(menuOption+0x50, "Goodies Menu..."),	goodiesMenu,				0x00000000},
			{HLMenu,	0,	0,	SR1.WriteString(menuOption,		 "Sound Menu..."),		debugSoundMenu,				0x00000000},
			{HLBit,		0,	0,	SR1.WriteString(menuOption+0x60, "Debug CD Loads"),		statsAndGhostCam,			0x80000000},
			{HLMenu,	0,	0,	SR1.WriteString(menuOption+0x70, "Save Menu..."),		debugImbueSoulReaverMenu,	0x00000000},
			{HLMenu,	0,	0,	SR1.WriteString(menuOption+0x80, "Debug Events"),		eventMenu,					0x00000000},
			{HLMenu,	0,	0,	SR1.WriteString(menuOption+0x90, "Cheats Menu..."),		cheatsMenu,					0x00000000},
			{HLEndMenu,	0,	0,	endOfMenu,												statsAndGhostCam,			0x00000000}
		};

		menuItem* moveMenu = (menuItem*) SR1.Read((BYTE*)saveMenu, sizeof(menuItem[4]));	// Copy the save menu
		SR1.Write(debugImbueSoulReaverMenu, (BYTE*) moveMenu, sizeof(menuItem[4]));			// Paste the save menu
		SR1.Write(mainMenu + sizeof(menuItem[4]), (BYTE*) mMenu, sizeof(menuItem[12]));		// Paste new main menu

		for (BYTE i=0; i<6; i++) {
			SR1.Write(debugSpecialAbilitiesMenu+0x14+
				sizeof(menuItem[3])+(i*sizeof(menuItem)), (DWORD) 1<<i);
		}
		SR1.Write(debugForgedAbilitiesMenu+0x14+sizeof(menuItem[2]), 0x00008C00);

		// [THE DELETED AREAS]
		moveMenu = (menuItem*)	SR1.Read((BYTE*)CityMenu, sizeof(menuItem[5]));	// Copy the CityMenu
		moveMenu[4] = moveMenu[3];												// Make room for City 10
		moveMenu[3].text = SR1.WriteString(menuOption+0xA0, "City 10");			// Name City 10
		moveMenu[3].arg1 = 0x0A;												// Confirm City 10
		SR1.Write(optsMenu,	(BYTE*) moveMenu, sizeof(menuItem[5]));				// Paste CityMenu
		SR1.Write(level2SelectMenu + (10<<2), optsMenu);						// Point to CityMenu

		menuItem* moveMenu2 = (menuItem*) 
		SR1.Read((BYTE*)level2SelectMenu, sizeof(menuItem[12]));				// Copy the level2SelectMenu
		moveMenu[3].text = SR1.WriteString(menuOption+0xB0, "Push 1");			// Recycle City 10 as Push 1
		moveMenu[3].arg1 = 0x01;												// Confirm as Push 1
		moveMenu2[10] = moveMenu[3];											// Add Push 1

		moveMenu[3].text = SR1.WriteString(menuOption+0xC0, "Chrono 19");		// Recycle Push 1 as Chrono 19
		moveMenu[3].arg1 = (DWORD)19;											// Confirm as Chrono 19
		moveMenu2[11] = moveMenu[3];											// Add Limbo

		moveMenu2[8].text = SR1.WriteString(menuOption+0xD0, "Fire Glyph...");

		SR1.Write(optsMenu+sizeof(menuItem[6]),									// Paste level2SelectMenu
			(BYTE*) moveMenu2, sizeof(menuItem[12]));

		SR1.Write(levelSelectMenu + sizeof(menuItem[12])+(4<<2),				// Point to level2SelectMenu	
			optsMenu+sizeof(menuItem[6]));
		// [/THE DELETED AREAS]


		SR1.Write(cheatsMenu+sizeof(menuItem[7]),	(BYTE*)&end_MI, sizeof(menuItem));
		SR1.Write(goodiesMenu+sizeof(menuItem[16]),	(BYTE*)&end_MI, sizeof(menuItem));
		SR1.Write(cheatsMenu,						(BYTE*)&mainMI, sizeof(menuItem));
		moveMenu[0]=mainMI;
		moveMenu[0].text = menuOption+0x90;										// Name Cheats Menu
		moveMenu[0].address1 = cheatsMenu;										// Point to Cheats Menu
		SR1.Write(renderSetupMenu, (BYTE*)moveMenu, sizeof(menuItem)); 

		projectiles(menuOption+0xF0);
	}}

	if (SR1.Read((BYTE*) menuState)==0x00)
		SR1.Write(menuState, (BYTE)0x04);   // Activate the menu
}

void debugOff()
{
	setupSR1();
	if (!SR1.IsRunning()) return;

	if (SR1.Read((BYTE*)menuState)==0x04)
	{
		SR1.Write(menuState, (BYTE) 0x00);  // Deactivates the menu.  Used to cause shadowlessness.  Fixed now.
		SR1.Write(lockRaziel, (BYTE) 0x06); // Lets Raz move again
	}
}

void swimOn()
{
	setupSR1();
	if (!SR1.IsRunning()) return;

	if (!allocated) allocated = SR1.Alloc(1024);
	if (allocated && !(initialised & swmInit))
	{
		initialised |= swmInit;
		SR1.WriteXString(allocated, "83 EC 3C 53 55 56 8B 74 24 4C 57 8B 46 38 50 E8 2C 00 00 00 83 C4 04 56 50 E8 29 00 00 00 8B C8 8B 46 40 8B 6C 24 5C 83 C4 08 8B 58 3C 89 4C 24 50 2B D9 81 F9 01 80 FF FF BB 72 A0 4B 00 FF E3 B8 A0 0F 4D 00 FF E0 B8 40 11 4D 00 FF E0 ");
		SR1.Write(allocated+0x3A, PhysicsCheckSwim+0x52);
		SR1.Write(allocated+0x41, STREAM_GetLevelWithID);
		SR1.Write(allocated+0x48, STREAM_GetWaterZLevel);
	}
	SR1.Write(FX_MakeWaterBubble, (BYTE) 0xC3);
	SR1.Write(TrailWaterFX, (BYTE) 0xC3);
	SR1.Write(RazielAdditionalCollide+0x278,
		allocated-(RazielAdditionalCollide+0x278+4));
}

void swimOff()
{
	setupSR1();
	if (!SR1.IsRunning()) return;

	if (allocated && (initialised & swmInit))
	{           
		SR1.Write(RazielAdditionalCollide+0x278,
			PhysicsCheckSwim - (RazielAdditionalCollide+0x278+4));
		SR1.Write(FX_MakeWaterBubble, (BYTE) 0x53);
		SR1.Write(TrailWaterFX, (BYTE) 0x83);
	}
}

void blackCore()
{
	setupSR1();
	if (!SR1.IsRunning()) return;

	if (!allocated) allocated = SR1.Alloc(1024);
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
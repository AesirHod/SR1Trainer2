// #include "process.h"
#include "sr1.h"

DWORD allocated = 0, allocated2 = 0;
BYTE initialised = 0, dbgInit = 0x01, swmInit = 0x02, brcInit = 0x04, version = 0;

// Soul Reaver 1 variables
DWORD MainG2, PCLoadSavedGame, RazielInit, InitStates, menudefs_pause_menu, SAVE_RestoreGame,
DEBUG_ReloadCurrentLevel, MAIN_DoMainInit, MAIN_StartGame, GlobalSave, MON_Say, GAMELOOP_Process,
// Objects
PHYSOB_BirthProjectile, INSTANCE_BirthObject, objectAccess, MONAPI_PossessNext,
// Physics
RazielAdditionalCollide, CollideReaverProjectile,
PhysicsCheckSwim, STREAM_GetLevelWithID, STREAM_GetWaterZLevel,
// Raziel Data
HealthUpgradeNo, ManaLevel, SpectralState,
// Effects
FX_MakeWaterBubble, FX_DrawGlowingLine, FX_StartGenericParticle, FX_StartGenericBlastRing,
FX_Spiral, TrailWaterFX, 
// Debug Menu Flags
debugRazielFlags1, debugRazielFlags2,debugRazielFlags3, debugHealthLevel, debugManaLevel,
statsAndGhostCam, MonsterCaptionFlag, endOfMenu,
// Debug Menu Strings
mainMenuStr, razielMenuStr, holdSRvrStr, specRvrStr, mateRvrStr, fireRvrStr, cameraMenuStr,
yesStr, noStr, curStr, 
// Debug Menus
debugHealthSystemMenu, debugSpecialAbilitiesMenu, debugForgedAbilitiesMenu,
optsMenu, CityMenu, levelSelectMenu, level2SelectMenu, cheatsMenu, renderSetupMenu,
mainMenu, saveMenu, razielMenu, cameraMenu, AIMenu,
goodiesMenu, debugImbueSoulReaverMenu, debugSoundMenu, eventMenu,
// Menu Controls
MENU_SELECT_RIGHT, handle_line_type_bit, FONT_Print, menuState, draw_menu,
lockRaziel,do_start_game, DEBUG_Process, FONT_VaReallyPrint, vsprintf_SR1,
// Memory
MEMPACK_Malloc, memmove_SR1;

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
	MessageBox(NULL, errInfo, "ERROR", MB_ICONERROR | MB_OK);
}

void setup()
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
				// Soul Reaver 1 variables
				MainG2=0x0042f360; PCLoadSavedGame=0x004afde0; RazielInit=0x0044bb00;
				InitStates=0x00436120; menudefs_pause_menu=0x00C438E4; MAIN_DoMainInit=0x00;
				MAIN_StartGame=0x0042f2b0; GlobalSave=0x00c31fb0; MON_Say=0x00402cd0;
				GAMELOOP_Process=0x00498c50;
				// Objects
				PHYSOB_BirthProjectile=0x004a60f0; INSTANCE_BirthObject=0x0049d6d0; objectAccess=0x004d9718;
				MONAPI_PossessNext = 0;
				// Physics
				RazielAdditionalCollide = 0x0044aed0; CollideReaverProjectile=0x004ae1a0;
				PhysicsCheckSwim=0x004a4890; STREAM_GetLevelWithID=0x004b7550; STREAM_GetWaterZLevel=0x004b76f0;
				// Raziel Data
				HealthUpgradeNo=0x00AE5520; ManaLevel=0x00AE5538; SpectralState=0x00ae5560;
				// Effects
				FX_MakeWaterBubble=0x0048f330; FX_DrawGlowingLine=0x00496050; FX_StartGenericParticle=0x00493d00;
				FX_StartGenericBlastRing=0x00494410; FX_Spiral=0x00495450; TrailWaterFX=0x0044c370;
				// Debug Menu Flags
				statsAndGhostCam=0x00C43860; MonsterCaptionFlag=0x00C43864; endOfMenu=0x00AE5098;
				debugRazielFlags1=0x00C41070; debugRazielFlags2=0x00C41074; debugRazielFlags3=0x00C41078;
				debugHealthLevel=0x004d4dbc; debugManaLevel=0x004d4dc0;
				// Debug Menu Strings
				mainMenuStr=0x004D7BE8; razielMenuStr=0x004D771C; holdSRvrStr=0x004D757C;
				specRvrStr=0x004D763C; mateRvrStr=0x004D762C; fireRvrStr=0x004D7620;
				cameraMenuStr=0x004D755C;
				yesStr=0x004d7c28; noStr = 0x004d7c24; curStr=0x004d4a20;
				// Debug Menus
				debugHealthSystemMenu=0x004d54a0; debugSpecialAbilitiesMenu=0x004d5548; debugForgedAbilitiesMenu=0x004d5638;
				optsMenu=0x004d5968; CityMenu=0x004d6418; levelSelectMenu=0x004d6988;
				level2SelectMenu=0x004d6880; cheatsMenu=0x004d5890; renderSetupMenu=0x004d6c58;
				mainMenu=0x004D4DC8; saveMenu=0x004d4ee8; razielMenu=0x004d53e0;
				cameraMenu=0x004d5c50; AIMenu=0x004d5cb0;
				goodiesMenu=0x004d50f8; debugImbueSoulReaverMenu=0x004d57e8; debugSoundMenu=0x004d52a8;
				eventMenu=0x004d4f48;
				// Menu Controls
				MENU_SELECT_RIGHT=0x0046c840; handle_line_type_bit=0x00482c50; FONT_Print=0x0048c700;
				menuState=0x00C43912; draw_menu=0x00482e00; lockRaziel=0x00AD5789;
				do_start_game=0x00470740; DEBUG_Process=0x004824d0; FONT_VaReallyPrint=0x0048c860;
				vsprintf_SR1 = 0x0042fe90;
				// Memory
				MEMPACK_Malloc = 0x004a0f20;
				memmove_SR1 = 0x004c0dc0;
				break;
			}
		case 2:
			{
				// Soul Reaver 1 variables
				MainG2=0x00439880; PCLoadSavedGame=0x004c6c20; RazielInit=0x00456610;
				InitStates=0x00440A80; menudefs_pause_menu=0x00c66e64; SAVE_RestoreGame=0x004C6990;
				DEBUG_ReloadCurrentLevel=0x004933f0; MAIN_DoMainInit=0x004393b0; MAIN_StartGame=0x00439670;
				GlobalSave=0x00c552a0; MON_Say=0x00403750; GAMELOOP_Process=0x004ac540;
				// Objects
				INSTANCE_BirthObject=0x004b1630; PHYSOB_BirthProjectile=0x004bbbd0; objectAccess=0x004fa9e8;
				MONAPI_PossessNext = 0x0040c470;
				// Physics
				RazielAdditionalCollide=0x004559E0; CollideReaverProjectile=0x004c4be0;
				PhysicsCheckSwim=0x004BA020; STREAM_GetLevelWithID=0x004d0fa0; STREAM_GetWaterZLevel=0x004d1140;
				// Raziel Data
				HealthUpgradeNo=0x00B08810; ManaLevel=0x00B08828; SpectralState=0x00b08850;
				// Effects
				FX_MakeWaterBubble=0x004A1BD0; FX_DrawGlowingLine=0x004A96D0; FX_StartGenericParticle=0x004a7020;
				FX_StartGenericBlastRing=0x004a7730; FX_Spiral=0x004a89a0; TrailWaterFX=0x00456e80;
				// Debug Menu Strings
				mainMenuStr=0x004F8D78; razielMenuStr=0x004F88AC; holdSRvrStr=0x004F870C;
				specRvrStr=0x004F87CC; mateRvrStr=0x004F87BC; fireRvrStr=0x004F87B0;
				cameraMenuStr=0x004F86EC;
				noStr=0x004f8db4; yesStr=0x004f8db8; curStr=0x004F5BB4;
				// Debug Menus
				debugHealthSystemMenu=0x004f6630; debugSpecialAbilitiesMenu=0x004f66d8;
				debugForgedAbilitiesMenu=0x004f67c8;
				optsMenu=0x004f6af8; CityMenu=0x004f75a8; levelSelectMenu=0x004f7b18;
				level2SelectMenu=0x004f7a10; cheatsMenu=0x004f6a20; renderSetupMenu=0x004f7de8;
				mainMenu=0x004F5F58; saveMenu=0x004f6078; razielMenu=0x004f6570;
				cameraMenu=0x004f6de0; AIMenu=0x004f6e40; goodiesMenu=0x004f6288;
				debugImbueSoulReaverMenu=0x004f6978; debugSoundMenu=0x004f6438; eventMenu=0x004f60d8;
				// Debug Menu flags
				statsAndGhostCam=0x00C66DE0; MonsterCaptionFlag=statsAndGhostCam+0x04; endOfMenu=0x00B08388;
				debugRazielFlags1=0x00C644F4; debugRazielFlags2=0x00C644F8; debugRazielFlags3=0x00C644FC;
				debugHealthLevel=0x004f5f4c; debugManaLevel=0x004f5f50;
				// Menu Controls
				MENU_SELECT_RIGHT=0x0047a530; handle_line_type_bit=0x00492690; FONT_Print=0x0049e290;
				menuState=0x00C66E92; draw_menu=0x00492840; lockRaziel=0x00AF8A79;
				do_start_game=0x0047e9c0; DEBUG_Process=0x00491f10; FONT_VaReallyPrint=0x0049e680;
				vsprintf_SR1=0x0043a420;
				// Memory
				MEMPACK_Malloc=0x004b5c40;
				memmove_SR1=0x004dcd60;
				break;
			}
		default:
			break;
	}

	allocated = SR1.Read((DWORD*)(MEMPACK_Malloc+0x58));		//  But what if it's 90909090
	if (!version || allocated==0x90909090)	{
		allocated = allocated2 = 0;
		initialised=0;
	}
    else if (version) initialised = SR1.Read((BYTE*)(MEMPACK_Malloc + 0x5C));

}
	return;
}

void cleanUp() {
	swimOff();
	debugOff();
	// if (allocated) SR1.Free(allocated);
	if (version) {
		SR1.Write((MEMPACK_Malloc+0x58), allocated);
		SR1.Write((MEMPACK_Malloc+0x5C), initialised/*(DWORD)0*/);
	}
}

DWORD particles(DWORD newParticles, DWORD* locations)
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
	_swab((char*)particleFX, (char*)particleFX, locs);		// Swap every other byte
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
	particles(newParticles+0x30, &fxLocations);
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
	SR1.Write		(newParticles+0x12, location);
	SR1.WriteXString(newParticles+0x16, "EB 03 8B 48 10 E9");  // Needs the space at the end???
	SR1.WriteDiff	(newParticles+0x1C, FX_StartGenericBlastRing+0x22);
	SR1.Write		(FX_StartGenericBlastRing+0x18, (BYTE) 0xE9);
	SR1.WriteDiff	(FX_StartGenericBlastRing+0x19, newParticles);
	SR1.WriteXString(FX_StartGenericBlastRing+0x1D, "90 90 90 90 90");
	newParticles+=0x40;
	// [/BLASTRING CHOICE FUNCTION]

	// [PARTICLE CHOICE FUNCTION]
	SR1.WriteXString(newParticles, "B8 FF FF FF FF 39 46 1C 8B 44 24 1C 8B 51 1C 75 07 BF");
	SR1.Write		(newParticles+0x12, location);
	SR1.WriteXString(newParticles+0x16, "EB 02 8B 3A E9");
	SR1.WriteDiff	(newParticles+0x1B, FX_StartGenericParticle+0x26);
	SR1.Write		(FX_StartGenericParticle+0x1D, (BYTE) 0xE9);
	SR1.WriteDiff	(FX_StartGenericParticle+0x1E, newParticles);
	SR1.Write		(FX_StartGenericParticle+0x22, 0x90909090);
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

	// [DAMAGE EFFECTS FUNCTION]															//  80 C3 04 3C 04 for regular water projectile and it's 80 C3 07 3C 04 for the new water effect.
	SR1.WriteXString(newParticles, "89 5C 24 F8 BB 00 00 08 00 3C 02 75 03 80 C3 05 3C 03 75 03 80 C3 07 3C 04 75 03 80 C3 06 3C 05 75 03 80 C3 05 3C 06 75 05 BB 14 00 00 01 3C 07 75 03 80 C3 03 53 8B 5C 24 FC E9");
	SR1.WriteDiff(newParticles+0x3B, CollideReaverProjectile+0x4F);
	SR1.Write(newParticles+0x3F, 0x90909090);
	SR1.WriteXString(CollideReaverProjectile + 0x42, "3C 01 7C 14 74 3B 90 56 E9");
	SR1.WriteDiff(CollideReaverProjectile+0x4B, newParticles);
	// SR1.WriteDiff(CollideReaverProjectile+0x4B, newParticles);	// Was there a bug that needed it twice?
	SR1.Write(CollideReaverProjectile+0x58, (WORD) 0x9090);			// Should make humans effected...  Wrecks the water move
	newParticles+=0x43;
	// [/DAMAGE EFFECTS FUNCTION]

	// [PROJECTILES UPDATE FUNCTION]
	SR1.WriteXString(newParticles,		"52 55 50 83 C0 10 8B 00 BA");
	SR1.Write		(newParticles+0x09, pArray);												// The new projectile array
	SR1.WriteXString(newParticles+0x0D, "39 D0 74 43 68 34 00 00 00 50 52 E8");
	SR1.WriteDiff	(newParticles+0x19, memmove_SR1);						// Call memmove
	SR1.WriteXString(newParticles+0x1D, "5A 58 5D 8B 04 24 89 50 10 05 1C 00 00 00 8B 00 8D 40 0C");
	SR1.WriteXString(newParticles+0x30, "8B 50 14 C7 02 01FF1617");
	SR1.WriteXString(newParticles+0x39, "8B 50 18 C7 02 01FF0E0F");
	SR1.WriteXString(newParticles+0x42, "8B 50 1C C7 02 01FF1011");
	SR1.WriteXString(newParticles+0x4B, "8B 50 28 C7 02 01FF1213");
	SR1.WriteXString(newParticles+0x54, "8B 50 24 C7 02 01FF1415");
	SR1.WriteXString(newParticles+0x5D, "8B 50 20 C7 02 01FF1819");
	SR1.WriteXString(newParticles+0x66, "58 5D 5A E9");
	SR1.WriteDiff	(newParticles+0x6A, INSTANCE_BirthObject);		// Goto INSTANCE_BirthObject
	SR1.WriteDiff	(PHYSOB_BirthProjectile+0x35, newParticles);	// Call getProjectiles
	// [/PROJECTILES UPDATE FUNCTION]
}

DWORD betaSpiral(DWORD next) {

	SR1.Write(FX_Spiral+0x056F, (BYTE)0xE9);
	SR1.WriteDiff(FX_Spiral+0x0570, next);

	SR1.Write(next, (WORD) 0x3D83);
	SR1.Write(next+=0x02, SpectralState);
	SR1.WriteXString(next+=0x04, "01 74 07 68 00 40 00 00 EB 05 68 00 00 40 00 E9");
	SR1.WriteDiff(next+=0x10, FX_Spiral+0x0574);
	next+=0x04;



	return next;
}

DWORD infoFix(DWORD next) {

	// Keep Health/Mana Levels up to date
	SR1.Write(draw_menu-0x05, (BYTE) 0xE8);
	SR1.Write(draw_menu-0x04, (next+0x02)-draw_menu);
	SR1.WriteDiff(DEBUG_Process+0xAB, draw_menu-0x05);
	SR1.WriteDiff(DEBUG_Process+0x049C, draw_menu-0x05);

	SR1.WriteXString(next, "90 90 8A 05");
	SR1.Write(next+=0x04, HealthUpgradeNo);
	SR1.WriteXString(next+=0x04, "88 05");
	SR1.Write(next+=0x02, debugHealthLevel);
	SR1.WriteXString(next+=0x04, "8A 05");
	SR1.Write(next+=0x02, ManaLevel+0x02);
	SR1.WriteXString(next+=0x04, "C0 E8 02");
	SR1.WriteXString(next+=0x03, "88 05");
	SR1.Write(next+=0x02, debugManaLevel);
	SR1.WriteXString(next+=0x04, "C3 90 90");
	
	// Allow for invincible working
	BYTE* nops = new BYTE[29];
	memset(nops, 0x90, 29);
	SR1.Write(draw_menu+0x02B6, (BYTE) 0xE8);
	SR1.WriteDiff(draw_menu+0x02B7, next+0x03);
	SR1.Write(draw_menu+0x02BB, nops, 29);
	SR1.WriteXString(next+=0x03, "F6 40 08 01 8B 50 10 8B 48 14 8B 02 74 08 21 C8 3B C1 74 08 EB 0D 21 C8 3B C1 74 07 68");
	SR1.Write(next+=0x1D, noStr);
	SR1.WriteXString(next+=0x04, "EB 05 68");
	SR1.Write(next+=0x03, yesStr);
	SR1.Write(next+=0x04, (BYTE) 0xE8);
	SR1.WriteDiff(next+=0x01, FONT_Print);
	SR1.WriteXString(next+=4, "83 C4 04 C3 90 90");

	SR1.Write(debugHealthSystemMenu+sizeof(menuItem[4])+0x08, 0x00000001);
	SR1.Write(debugHealthSystemMenu+sizeof(menuItem[4])+0x14, 0x00000800);

	//	[REALLY KEEP INFO BETWEEN LOADS]
	SR1.WriteDiff(RazielInit+0xAB, next+6);
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
	SR1.WriteXString(next+=0x04, "53 8B 1D");
	SR1.Write(next+=0x03, 0x00C552A0);
	SR1.WriteXString(next+=0x04, "C6 43 04 00 5B C3 90 90 90");
	next+=0x09;
	// return next;

	// BYTE* FontPrint = SR1.Read((BYTE*)FONT_Print, 0x4E);
	//	Print Monster captions:
	//	MAJOR BUG: In unpatched version, MON_GSay and GXFilePrint are same as MON_Say
	/*
		FontPrint[0x03] = 0x08;
		FontPrint[0x07] = 0x0C;
		FontPrint[0x3E] = 0x0C;
		DWORD*
		FP = (DWORD*)(FontPrint+0x10);
		FP[0] = vsprintf_SR1-(next+0x14);
		FP = (DWORD*)(FontPrint+0x46);
		FP[0] = FONT_VaReallyPrint-(next+0x4A);
		SR1.Write(next, FontPrint, 0x4E);
		SR1.Write(MON_Say, (BYTE)0xE9);
		SR1.Write(MON_Say+1, next-(MON_Say+5));
		next+=0x4E;
		return next;
	 */

	//	[DEACTIVATE KEEP INFO WHEN LOADING A SAVE OR RESTARTING]
	/*	
		SR1.Write(0x00438E51, (WORD) 0x4374);		// jmp 	0x00438E96		// Starting a new game always turns it off but loading doesn't...
		SR1.WriteXString(0x00438E96, "80 25 66 6E C6 00 DF EB BE");
		SR1.WriteXString(0x00438E51, "90 90 81 25 64 6E C6 00 FF FF DF FF");// Same again but without loading condition
		SR1.Write(0x0043A053, (WORD)0x0AEB);								// Always skip switch off
	*/

	if (version==2)			SR1.Write(MainG2+0x07A5, (WORD) 0x3874);		// Switch off Keep_Info if playing intro
	else if (version==1)	SR1.Write(MainG2+0x0784, (WORD) 0x3874);
	SR1.WriteXString(PCLoadSavedGame+0xE8, "81 E6 FF FF DF FF");			// Switch off Keep_Info if loading a save

	// [STUFF TO FIX THE INTRO]
	// Only in patched version :(
	/*
		SR1.Write(do_start_game+0x06, (BYTE) 0x1E);
		SR1.Write(do_start_game+0x15, (BYTE) 0xE8);
		SR1.Write(do_start_game+0x16, MAIN_DoMainInit-(do_start_game+0x1A));
		SR1.Write(do_start_game+0x1A, (BYTE) 0xE8);
		SR1.Write(do_start_game+0x1B, MAIN_StartGame-(do_start_game+0x1F));
		SR1.WriteXString(do_start_game+0x1F, "B8 01 00 00 00 C3 33 C0 C3");
	*/

	// [NEW TOGGLE BIT CONTROL]
	SR1.WriteXString(handle_line_type_bit-0x05, "59 09 08 5E C3");
	SR1.WriteXString(handle_line_type_bit+0x1F, "51 23 08 3B 0C 24 75 D4 59 F7 D1 21 08 5E C3 56 E8");
	SR1.WriteDiff(handle_line_type_bit+0x30, MENU_SELECT_RIGHT);
	SR1.WriteXString(handle_line_type_bit+0x34, "83 C4 04 85 C0 75 DA 5E C3 90 90 90");
	SR1.Write(handle_line_type_bit+0x13, (WORD) 0x1974);

	SR1.Write(curStr, (BYTE) 0xD2);		// Clearer Curser

	//if (version==1) SR1.Write(0x00499213, 0x8F0F0000);	// Pause while debugging (new)
	//if (version==2) SR1.Write(GAMELOOP_Process+0x05C3, 0x8F0F0000);	// Pause while debugging (new)

	return next;
}

DWORD monsterDeath(DWORD next) {
	// [UNCONDITIONAL DEATH]
	if (version==2) {
		// Hahaha!  A waterey grave!
		DWORD INSTANCE_Post = 0x004b1c70;
		DWORD MON_AnimPlaying = 0x0040d100;
		DWORD MON_GetTime = 0x0040f020;
		DWORD MON_PlayAnim = 0x0040d0d0;
		DWORD MON_TurnOffAllSpheres = 0x0040cac0;
		DWORD G2Anim_SetNoLooping = 0x00457840;
		DWORD MON_DefaultMessageHandler = 0x00411ee0;
		DWORD MON_SwitchState = 0x0040cca0;
		DWORD MON_MonsterGlow = 0x004174C0;
		DWORD MON_DeadEntry = 0x004141d0;
		DWORD MON_Dead = 0x00414310;
		DWORD MON_GeneralDeath = 0x00417820;
		DWORD MON_GeneralDeathEntry = 0x00417610;
		DWORD MONTABLE_GetStateFuncs = 0x00418e90;
		DWORD DefaultStateTable = 0x004f1a58;
		DWORD MON_PermanantDeath;
		DWORD MON_PermanantlyDead;
		DWORD currentReaverID = 0x00B0884C;
		DWORD StateHandlerAttack2 = 0x00442b60;

		// Adds the MON_PermanantDeath state
		MON_PermanantDeath = next;
		SR1.WriteXString(next, "53 55 56 8B 74 24 10 33 ED 33 DB 8A 46 18 57 8B BE 4C 01 00 00 A8 10 74 0F 6A 24 56 E8");	// OK
		SR1.WriteDiff(next+=0x1D, MON_AnimPlaying);		// OK
		SR1.WriteXString(next+=0x04, "83 C4 08 85 C0 75 09 80 BF 44 01 00 00 06 75 07 BB 01 00 00 00 EB 2F F6 46 18 12 74 29 56 E8");	// OK
		SR1.WriteDiff(next+=0x1F, MON_GetTime);		// OK
		SR1.WriteXString(next+=0x04, "8B 8F 00 01 00 00 83 C4 04 3B C8 73 16 6A 01 6A 24 56 E8");	// OK
		SR1.WriteDiff(next+=0x13, MON_PlayAnim);	// OK
		SR1.WriteXString(next+=0x04, "83 C4 0C 56 E8");		// OK
		SR1.WriteDiff(next+=0x05, MON_TurnOffAllSpheres);	// OK
		SR1.WriteXString(next+=0x04, "83 C4 04 3B DD 74 21 8B 17 8D 86 94 01 00 00 83 E2 EF 50 89 17 E8");	// OK
		SR1.WriteDiff(next+=0x16, G2Anim_SetNoLooping);		// OK
		SR1.WriteXString(next+=0x04, "83 C4 04 6A 20 56 E8");	// OK
		SR1.WriteDiff(next+=0x07, MON_SwitchState);		// OK
		SR1.WriteXString(next+=0x04, "83 C4 08 E9");	// OK
		SR1.WriteDiff(next+=0x04, MON_GeneralDeath+0x8C);	// OK
		next+=0x04;

		// Adds the MON_PermanantlyDead state
		MON_PermanantlyDead = next;
		SR1.WriteXString(next, "83 EC 40 53 55 56 8B 74 24 50 33 DB 33 C9 8B AE 4C 01 00 00 C6 85 44 01 00 00 02 E9");	// OK
		SR1.WriteDiff(next+=0x1C, MON_Dead+0x14);	// OK
		next+=0x04;

		// Extend MONTABLE_GetStateFuncs to include the MON_PermanantlyDead state
		SR1.Write(MONTABLE_GetStateFuncs+0x2B, (BYTE) 0xE8);
		SR1.WriteDiff(MONTABLE_GetStateFuncs+0x2C, next);
		SR1.Write(MONTABLE_GetStateFuncs+0x30, (WORD) 0x9090);
		SR1.Write(MONTABLE_GetStateFuncs+0x37, (BYTE) 0xE8);
		SR1.WriteDiff(MONTABLE_GetStateFuncs+0x38, next);
		SR1.Write(MONTABLE_GetStateFuncs+0x3C, (WORD)0x9090);		// All OK

		// Add the MONTABLE_GetAdditionalStateFuncs function
		SR1.WriteXString(next, "83 FA 1E 7F 08 8D 04 D5");
		SR1.Write(next+=0x08, DefaultStateTable);
		SR1.WriteXString(next+=0x04, "C3 83 EA 1F 3E 8D 04 D5");
		SR1.Write(next+=0x08, next+0x05);
		SR1.Write(next+=0x04, (BYTE) 0xC3);
		SR1.Write(next+=0x01, MON_GeneralDeathEntry);
		SR1.Write(next+=0x04, MON_PermanantDeath);
		SR1.Write(next+=0x04, MON_DeadEntry);
		SR1.Write(next+=0x04, MON_PermanantlyDead);

		// Extends the glyph response List
		SR1.WriteDiff(MON_DefaultMessageHandler+0x023E, next+=0x04);	// May need to check this in SR1 version 1.0
		SR1.WriteXString(next, "83 FB 05 0F 87");	// OK
		SR1.WriteDiff(next+=0x05, MON_DefaultMessageHandler+0x0C0E);	// And this
		SR1.WriteXString(next+=0x04, "6A 1F 56 E8");	// OK
		SR1.WriteDiff(next+=0x04, MON_SwitchState);
		SR1.WriteXString(next+=0x04, "83 C4 08 6A 14 6A 0A 6A 46 68 44 44 88 00 56 E8");	// OK
		SR1.WriteDiff(next+=0x10, MON_MonsterGlow);
		SR1.WriteXString(next+=0x04, "83 C4 14 66 C7 87 40 01 00 00 10 00 5F 5E 5D 5B 83 C4 18 C3");	// OK
		// And it would be easy to add more!

		// Add function to choose the reavers finishing move
		DWORD newFinishOff = next+0x15;
		SR1.WriteXString(next+=0x14, "90 83 3D");
		SR1.Write(next+=0x03, currentReaverID);
		SR1.WriteXString(next+=0x04, "04 74 12 83 3D"); 
		SR1.Write(next+=0x05, currentReaverID);
		SR1.WriteXString(next+=0x04, "06 75 19 6A 20 68 0C 00 00 01 EB 07 6A 00 68 07 00 08 00 50");
		SR1.Write(next+=0x14, (BYTE) 0xE8);
		SR1.WriteDiff(++next, INSTANCE_Post);
		SR1.WriteXString(next+=0x04, "83 C4 0C C3 90 90 90");
		next+=0x07;

		// Modify StateHandlerAttack2 to use reaver finishing move function
		SR1.Write(StateHandlerAttack2+0x136A, (BYTE) 0xE8);
		SR1.WriteDiff(StateHandlerAttack2+0x136B, newFinishOff);
		SR1.WriteXString(StateHandlerAttack2+0x136F, "83 3D");
		SR1.Write(StateHandlerAttack2+0x1371, currentReaverID);
		SR1.WriteXString(StateHandlerAttack2+0x1375, "04 74 23 83 3D");
		SR1.Write(StateHandlerAttack2+0x137A, currentReaverID);
		SR1.WriteXString(StateHandlerAttack2+0x137E, "06 74 1A EB 05 90 90 90 90 90");
	}
	// [/UNCONDITIONAL DEATH]
	return next;
}

menuItem possession(DWORD location) {

	DWORD possessFunc = 0;
	if (version==1) {

		SR1.Write(location, 0x90909090);
		location+=0x04;

		char* hexStr = "A1 68 47 4E 00 56 57 8B 3D D4 37 C4 00 85 C0 75 0A A1 CC 37 C4 00 A3 68 47 4E 00 8B 70 08 85 F6 75 03 8B 77 04 3B F0 0F 84 BC 00 00 00 6A 01 56 E8 AB 57 0A 00 83 C4 08 A8 0B 74 15 0F BF 05 AA 37 C4 00 50 56 E8 36 59 0A 00 83 C4 08 85 C0 75 15 8B 76 08 85 F6 75 03 8B 77 04 A1 68 47 4E 00 3B F0 75 C9 EB 05 A1 68 47 4E 00 3B F0 74 7A 56 68 E0 10 C4 00 E8 B6 6A 07 00 A1 CC 37 C4 00 83 C4 08 3B F0 74 39 8B 0D 68 47 4E 00 3B C8 74 10 6A 00 68 10 00 00 01 51 E8 63 57 0A 00 83 C4 0C 6A 00 68 0F 00 00 01 56 E8 53 57 0A 00 8B 0D CC 37 C4 00 83 C4 0C 56 68 0F 00 00 01 51 EB 08 6A 00 68 10 00 00 01 50 E8 34 57 0A 00 89 35 68 47 4E 00 8B 56 1C 83 C4 0C 8B 42 24 50 68 00 00 00 00 E8 2A DE 02 00 83 C4 08 5F 5E C3";
		DWORD dataSize = 0;													// No of bytes to be written
		BYTE* array = new BYTE[1+(strlen(hexStr)/2)];	
		while (*hexStr) {													// For every character
			if (isxdigit(*hexStr)) {
				if (isxdigit(hexStr[1])) {
					sscanf(hexStr++, "%02x", (DWORD*) &array[dataSize++]);	// If it's hex scan for a byte
				}
			}
			hexStr++;
		}
		// DWORD V=0x00c410e0, W=0x00C437AA, X=0x00C437CC, Y=0x00C437D4, Z=0x004E4768;
		DWORD INSTANCE_Query = 0x0049dca0;
		DWORD INSTANCE_InPlane = 0x0049de40;
		DWORD INSTANCE_Post = 0x0049dcc0;
		DWORD CAMERA_SetInstanceFocus = 0x00474920;
		DWORD printf_SR1 = 0x0042fe30;

		DWORD*
		addr = (DWORD*) (array+0x31);
		*addr = INSTANCE_Query-(location+0x35);
		addr = (DWORD*) (array+0x46);
		*addr = INSTANCE_InPlane-(location+0x4A);
		addr = (DWORD*) (array+0x76);
		*addr = CAMERA_SetInstanceFocus-(location+0x7A);
		addr = (DWORD*) (array+0x99);
		*addr = INSTANCE_Post-(location+0x9D);
		addr = (DWORD*) (array+0xA9);
		*addr = INSTANCE_Post-(location+0xAD);
		addr = (DWORD*) (array+0xC8);
		*addr = INSTANCE_Post-(location+0xCC);
		addr = (DWORD*) (array+0xE2);
		*addr = printf_SR1-(location+0xE6);
		addr = (DWORD*) (array+0xDD);
		*addr = location+dataSize;

		possessFunc = location;
		SR1.Write(location, array, dataSize);
		SR1.WriteString(location=*addr, "Switching possession to %s\n");
		location+=0x1C;
	}
	else if (version==2) {
		possessFunc = MONAPI_PossessNext;
	}
	char* possessStr = "Camera Possess Next";
	menuItem possess = {HLAction,	NULL,	NULL,	SR1.WriteString(location, possessStr),	possessFunc,	NULL};
    location += strlen(possessStr);
	location++;
	SR1.Write(location, 0x90909090);
	location+=0x04;
	return possess;
}

void debugOn() {

	setup();
	if (!SR1.IsRunning()) return;

	if (!allocated)		allocated	= SR1.Alloc(1024);
	if (!allocated2)	allocated2	= SR1.Alloc(1024);
	
	if (allocated && !(initialised & dbgInit))
	{{
		initialised |= dbgInit;

		DWORD next = allocated2;
		next = betaSpiral(next);
		next = infoFix(next);
		next = monsterDeath(next);
		BYTE* possessionFunc = (BYTE*) &possession(next);
		
		// ADD POSSESSION CAMERA
		menuItem* moveMenu = (menuItem*) SR1.Read((BYTE*)cameraMenu, sizeof(menuItem[3]));	// Copy the Camera Menu
		cameraMenu -= sizeof(menuItem);														// Update Camera Menu Location
		SR1.Write(cameraMenu, (BYTE*) moveMenu, sizeof(menuItem[3]));						// Paste the Camera Menu
		SR1.Write(cameraMenu+sizeof(menuItem[3]), possessionFunc, sizeof(menuItem));

		// BUILD EXTENDED IMBUE MENU

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
		SR1.Write(mainMenu+sizeof(menuItem[70])+0x10, menuOption);
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

		moveMenu = (menuItem*) SR1.Read((BYTE*)saveMenu, sizeof(menuItem[4]));	// Copy the save menu
		SR1.Write(debugImbueSoulReaverMenu, (BYTE*) moveMenu, sizeof(menuItem[4]));			// Paste the save menu
		SR1.Write(mainMenu+sizeof(menuItem[4]), (BYTE*) mMenu, sizeof(menuItem[12]));		// Update main menu

		for (BYTE i=0; i<6; i++)
			SR1.Write(debugSpecialAbilitiesMenu+0x14+
				sizeof(menuItem[3])+(i*sizeof(menuItem)), (DWORD) 1<<i);

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

void debugOff() {
	setup();
	if (!SR1.IsRunning()) return;

	if (SR1.Read((BYTE*)menuState)==0x04)
	{
		SR1.Write(menuState, (BYTE) 0x00);  // Deactivates the menu.  Used to cause shadowlessness.  Fixed now.
		SR1.Write(lockRaziel, (BYTE) 0x06); // Lets Raz move again
	}
}

void swimOn() {

	setup();
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
	SR1.WriteDiff(RazielAdditionalCollide+0x278, allocated);
}

void swimOff() {
	setup();
	if (!SR1.IsRunning()) return;

	if (allocated && (initialised & swmInit))
	{           
		SR1.WriteDiff(RazielAdditionalCollide+0x278, PhysicsCheckSwim);
		SR1.Write(FX_MakeWaterBubble, (BYTE) 0x53);
		SR1.Write(TrailWaterFX, (BYTE) 0x83);
	}
}

void blackCore() {
	setup();
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
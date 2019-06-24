/*
    ###################################################################################
    ELTE-FAIL.cpp
    Main program
    Made by PR00F88
    EMAIL : PR0o0o0o0o0o0o0o0o0o0oF88@gmail.com
    ###################################################################################
*/

/*
    #################################################################################
    #    _______  _    _________ _______      _______  _______ _________ _          #
    #   (  ____ \( \   \__   __/(  ____ \    (  ____ \(  ___  )\__   __/( \         #
    #   | (    \/| (      ) (   | (    \/    | (    \/| (   ) |   ) (   | (         #
    #   | (__    | |      | |   | (__  _____ | (__    | (___) |   | |   | |         #
    #   |  __)   | |      | |   |  __)(_____)|  __)   |  ___  |   | |   | |         #
    #   | (      | |      | |   | (          | (      | (   ) |   | |   | |         #
    #   | (____/\| (____/\| |   | (____/\    | )      | )   ( |___) (___| (____/\   #
    #   (_______/(_______/)_(   (_______/    |/       |/     \|\_______/(_______/   #
    #                                                                               #
    #    Elcseszett Labor Teszt Eredménye - Funkcionális Analfabéta Idegen Légió    #
    #                                (ELTE-FAIL)                                    #
    #    Effect of a Totalled Lab Experiment - Functional Illiterate Alien Legion   #
    #                                (ETLE-FIAL)                                    #
    #    C++ version by PR00F88                                                     #
    #                                                                               #
    #################################################################################
*/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "CustomPGE.h"    // Using our customized PGE.

/**
    Entry point of our application.

    This doesn't use any API directly.

    @param hInstance     Application's current instance handle.
    @param hPrevInstance Always NULL.
    @param lpCmdLine     Command line.
    @param nCmdShow      How to display the window, ignored.
    @return 0 if the function terminates before entering runGame().
            Otherwise PostQuitMessage()'s nExitCode parameter (WM_QUIT message's wParam parameter).
*/

#pragma warning(disable:4100)  /* unreferenced formal parameter */

int WINAPI WinMain(const HINSTANCE hInstance, const HINSTANCE hPrevInstance, const LPSTR lpCmdLine, const int nCmdShow)
{
    CustomPGE* const gameEngine = CustomPGE::createAndGetCustomPGEinstance();
    int nResult = 0;

    // Initializing the game engine.
    switch ( gameEngine->initializeGame() )
    {
    case 0: // Success, running the game.
        nResult = gameEngine->runGame();  // <<< << << <<  <  -  <  -  <  -  -  -  HERE THE GAME RUNS! O.o 

        // Destroying the game engine.
        switch ( gameEngine->destroyGame() )
        {
        case 0: // Successful stop.
            break;
        case 1: // Error during GFX engine stop.
            CustomPGE::showErrorDialog( PGE_MSG_ERR_EXIT_GFX ); break;
        case 2: // Error during SFX engine stop.
            CustomPGE::showErrorDialog( PGE_MSG_ERR_EXIT_SFX ); break;
        case 3: // Error during NET engine stop.
            CustomPGE::showErrorDialog( PGE_MSG_ERR_EXIT_NET ); break;
        } // switch destroy

        break;
    
    // An error occured during initialization.
    case 1: // The GFX engine failed.
        CustomPGE::showErrorDialog( PGE_MSG_ERR_INIT_GFX ); break;
    case 2: // The SFX engine failed.
        CustomPGE::showErrorDialog( PGE_MSG_ERR_INIT_SFX ); break;
    case 3: // The NET engine failed.
        CustomPGE::showErrorDialog( PGE_MSG_ERR_INIT_NET ); break;
    } // switch initialize

    return nResult;
} // WinMain()


__declspec(dllexport) int APIENTRY g3tVersion()
{
    return 1;
}

/*
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
} */

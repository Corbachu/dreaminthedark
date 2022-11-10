//----------------------------------------------------------------------------
//  EDGE2 Main
//----------------------------------------------------------------------------
//
//  Copyright (c) 1999-2009  The EDGE2 Team.
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//----------------------------------------------------------------------------

#include "i_defs.h"
//#include "i_sdlinc.h"  // needed for proper SDL main linkage

#include "../../epi/exe_path.h"

#include "../dm_defs.h"
#include "../m_argv.h"
#include "../e_main.h"
#include "../version.h"

const char *win32_exe_path = ".";

extern "C" 
{

#ifdef GDB_DEBUG
extern void gdb_init(void);
#endif

#ifdef DREAMCAST
static int		dcargc;
static char*	dcargv[64];
#endif

int main(int argc, char *argv[])
{
	// FIXME: setup argument handler NOW
	bool allow_coredump = false;
	for (int i = 1; i < argc; i++)
		if (strcmp(argv[i], "-core") == 0)
			bool allow_coredump = true;

    I_SetupSignalHandlers(allow_coredump);

#ifdef WIN32
	// -AJA- give us a proper name in the Task Manager
	SDL_RegisterApp(TITLE, 0, 0);
#endif

    I_CheckAlreadyRunning();

#ifdef WIN32
    // -AJA- change current dir to match executable
	win32_exe_path = epi::GetExecutablePath(argv[0]);

    ::SetCurrentDirectory(win32_exe_path);
#else
    // -ACB- 2005/11/26 We don't do on LINUX since we assume the
    //                  executable is globally installed
#endif

#ifdef DREAMCAST
	const char *a[]={"dreamedge","-width","640","-height","480","-bpp","16","-fullscreen","-smoothing","-nomipmap","-v135","-rumble","-nodlights",
#ifdef NO_SOUND
		"-nosound",
#endif
		"-iwad","hyper.wad",
		"-file","slave.wad",
		"-logo","logo0.vq"
	};

#ifdef GDB_DEBUG
	gdb_init();
#endif

	printf("DREAMCAST STARTUP\n");
	fflush(stdout);

	FILE *dcph;
	int ix;

	printf("Attempting to load settings from edge.preset\n");

	//"/pc",          /* name */
	//KOS has changed from /cd/ -> /pc/, which makes a big difference. Let's try that...
	dcph = fopen("/pc/edge.preset", "r");
	if (dcph == NULL)
	{
		printf("Error! Couldn't open preset. Using default args.\n\n");
		fflush(stdout);
		E_Main(19,a);
		return 0;
	}

	printf("Preset:");
	for (ix=0; ix<64; ix++)
	{
		char temp[256];
		temp[0] = 0;
		fgets(temp, 255, dcph);
		if ((temp[0] == 0) || feof(dcph))
			break;
		temp[strlen(temp) - 1] = 0;
		dcargv[ix] = strdup(temp);
		printf(" %s", dcargv[ix]);
	}
	fclose(dcph);
	dcargc = ix;
	printf("\n");
	fflush(stdout);

	E_Main(dcargc, (const char **)dcargv);
#else
	// Run EDGE2. it never returns
	E_Main(argc, (const char **)argv);
#endif
	return 0;
}

} // extern "C"


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab

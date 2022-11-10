//----------------------------------------------------------------------------
//  EDGE2 Dreamcast Video Code
//----------------------------------------------------------------------------
//
//  Copyright (c) 2015 Josh Pearson.
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

#include <kos.h>

#include "i_defs.h"
#include "i_defs_gl.h"

#include <signal.h>

#include "../m_argv.h"
#include "../m_misc.h"
#include "../r_modes.h"

bool is_lowres;

int graphics_shutdown = 0;

cvar_c in_grab;

static int display_W, display_H;

void I_GrabCursor(bool enable)
{
}

void I_StartupGraphics(void)
{
	int dc_region, video_cable;
	bool is_NTSC;

    dc_region = flashrom_get_region();
    video_cable = vid_check_cable();
    is_NTSC = (dc_region != FLASHROM_REGION_EUROPE && video_cable != CT_VGA) ? true : false;
	I_Printf("I_StartupGraphics:\n  Dreamcast region: %s\n", dc_region == FLASHROM_REGION_JAPAN ? "Japan" :
			dc_region == FLASHROM_REGION_US ? "US" : dc_region == FLASHROM_REGION_EUROPE ? "Europe" : "unknown");
	I_Printf("  Video cable: %s\n", video_cable == CT_VGA ? "VGA" : video_cable == CT_COMPOSITE ? "Composite" :
			video_cable == CT_RGB ? "SCART" : "unknown");

    glKosInit();

	if (M_CheckParm("-nograb"))
		in_grab = 0;

	// default kos startup is 640x480
	display_W = 640;
	display_H = 480;

	is_lowres = false;

	I_Printf("Desktop resolution: %dx%d\n", display_W, display_H);

	// only two modes for PAL and VGA, four for NTSC to combat overscan
	scrmode_c scr_mode;
	if (is_NTSC)
	{
		scr_mode.width = 320;
		scr_mode.height = 200;
		scr_mode.depth = 16;
		scr_mode.full = true;
		R_AddResolution(&scr_mode);
	}
	scr_mode.width = 320;
	scr_mode.height = 240;
	scr_mode.depth = 16;
	scr_mode.full = true;
	R_AddResolution(&scr_mode);
	if (is_NTSC)
	{
		scr_mode.width = 640;
		scr_mode.height = 400;
		scr_mode.depth = 16;
		scr_mode.full = true;
		R_AddResolution(&scr_mode);
	}
	scr_mode.width = 640;
	scr_mode.height = 480;
	scr_mode.depth = 16;
	scr_mode.full = true;
	R_AddResolution(&scr_mode);

	I_Printf("I_StartupGraphics: initialisation OK\n");
}

#define GL_PVR_VERTEX_BUF_SIZE 2560 * 256 /* PVR Vertex buffer size */

bool I_SetScreenSize(scrmode_c *mode)
{
#if 0
	I_Printf("I_SetScreenSize: trying %dx%d %dbpp (%s)\n",
			 mode->width, mode->height, mode->depth,
			 mode->full ? "FS" : "WD");

	if (mode->width == 320)
		vid_init(DM_320x240, PM_RGB565);
	else
		vid_init(DM_640x480, PM_RGB565);

	is_lowres = (mode->width == 320) ? true : false;


    pvr_init_params_t params = {
        /* Enable opaque and translucent polygons with size 32 and 32 */
        { PVR_BINSIZE_32, PVR_BINSIZE_0, PVR_BINSIZE_32, PVR_BINSIZE_0, PVR_BINSIZE_0 },
        GL_PVR_VERTEX_BUF_SIZE, /* Vertex buffer size */
        0, /* No DMA */
        0 /* No FSAA */
    };


	//pvr_shutdown();
	//thd_sleep(100);
    //pvr_init();

	//glEnable(GL_KOS_NEARZ_CLIPPING);

#endif
		glEnable(GL_NEARZ_CLIPPING_KOS);


	return true;
}


/*
=================
GL_BeginRendering

=================
*/
void I_BeginRendering(int *x, int *y, int *width, int *height) 
{
  *x = 0;
  *y = 0;
  *width = SCREENWIDTH;
  *height = SCREENHEIGHT;
  glEnable(GL_NEARZ_CLIPPING_KOS);
}


void I_StartFrame(void)
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void I_FinishFrame(void)
{
#if 0
    pvr_stats_t stats;

    pvr_get_stats(&stats);
	I_Printf("%d/%f/%d/%d/%d/%d/%d\n", stats.vbl_count, stats.frame_rate,
		stats.reg_last_time, stats.rnd_last_time, stats.vtx_buffer_used,
		stats.buf_last_time, stats.frame_count);

	uint32 vmav = pvr_mem_available();
	I_Printf("VRAM available: %d\n", vmav);
#endif
	//glutSwapBuffers();
	glKosSwapBuffers();
}

void I_PutTitle(const char *title)
{
}

void I_SetGamma(float gamma)
{
}

void I_ShutdownGraphics(void)
{
	if (graphics_shutdown)
		return;

	vid_shutdown();
	graphics_shutdown = 1;
}

void I_GetDesktopSize(int *width, int *height)
{
	*width  = display_W;
	*height = display_H;
}

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab

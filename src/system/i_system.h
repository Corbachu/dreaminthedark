//----------------------------------------------------------------------------
//  EDGE2 Platform Interface (EPI) Header
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
//
//  Based on the DOOM source code, released by Id Software under the
//  following copyright:
//
//    Copyright (C) 1993-1996 by id Software, Inc.
//
//----------------------------------------------------------------------------

#ifndef __I_SYSTEM_H__
#define __I_SYSTEM_H__

//#include "../../epi/timestamp.h"

//--------------------------------------------------------
//  SYSTEM functions.
//--------------------------------------------------------

// -ACB- 1999/09/20 Removed system specific attribs.

void I_SetupSignalHandlers(bool allow_coredump);
void I_CheckAlreadyRunning(void);

void I_SystemStartup(void);
// This routine is responsible for getting things off the ground, in
// particular calling all the other platform initialisers (i.e.
// I_StartupControl, I_StartupGraphics, I_StartupMusic and
// I_StartupSound).  Does whatever else the platform code needs.

//--- void I_Loop(void);
// This is called by EDGE2 to begin the main engine loop, and is not
// expected to return.  It must call engine::Tick() to perform a
// single loop in the system, which processes events, updates the play
// simulation, keeps sound and music playing, and most importantly
// renders a single frame of graphics.

void I_Printf(const char *message,...) GCCATTR ((format(printf, 1, 2)));
// The generic print function.  If in text mode, the message should be
// displayed on the text mode screen.  This function should also call
// I_Debugf() and CON_Printf().

void I_Debugf(const char *message,...) GCCATTR ((format(printf, 1, 2)));
// The generic debugging function.

void I_PutTitle(const char *title);
// This function should clear the text mode screen (or window), and
// write the given title in a banner at the top (in a stand-out way,
// e.g. bright white on a red background).

void I_Error(const char *error,...) GCCATTR((format(printf, 1, 2)));
// The error function.  All fatal errors call I_Error().  This
// I_CloseProgram).

void I_DisplayExitScreen(void);
// Displays the exit screen on the text mode screen (or window).  The
// text typically comes from the "ENDOOM" lump in the IWAD.

void I_SystemShutdown(void);
// The opposite of the I_SystemStartup routine.  This will shutdown
// everything running in the platform code, by calling the other
// termination functions (I_ShutdownSound, I_ShutdownMusic,
// I_ShutdownGraphics and I_ShutdownControl), and doing anything else
// the platform code needs to (e.g. freeing all other resources).

void I_CloseProgram(int exitnum) GCCATTR((noreturn));
// Exit the program immediately, using the given `exitnum' as the
// program's exit status.  This is the very last thing done, and
// I_SystemShutdown() is guaranteed to have already been called.

void I_TraceBack(void);
// Aborts the program, and displays a traceback if possible (which is
// useful for debugging).  The system code should check for the
// "-traceback" option, and when present call this routine instead of
// I_CloseProgram() whenever a fatal error occurs.

void I_Warning(const char *warning,...) GCCATTR((format(printf, 1, 2)));
// Writes a warning to the console and the debug file (if any).  This
// function should call CON_Printf().

void I_Sleep(int millisecs);
// -AJA- 2005/01/21: sleep for the given number of milliseconds.

// Force Feedback
void I_Tactile (int freq, int intensity, int select);

void I_MessageBox(const char *message, const char *title);
// -AJA- 2007/04/13: display a system message box with the
// given message (typically a serious error message).

//extern const char *win32_exe_path;


//--------------------------------------------------------
//  INPUT functions.
//--------------------------------------------------------
//
// -ACB- 1999/09/19 moved from I_Input.H

void I_StartupControl(void);
// Initialises all control devices (i.e. input devices), such as the
// keyboard, mouse and joysticks.  Should be called from
// I_SystemStartup() -- the main code never calls this function.

void I_ControlGetEvents(void);
// Causes all control devices to send their events to the engine via
// the E_PostEvent() function.

void I_CalibrateJoystick(int ch);
// Something for calibrating a joystick -- not currently used.

void I_ShutdownControl(void);
// Shuts down all control devices.  This is the opposite of
// I_StartupControl().  Should be called from I_SystemShutdown(), the
// main code never calls this function.

int I_PureRandom(void);
// Returns a fairly random value, used as seed for EDGE2's internal
// random engine.  If this function would return a constant value,
// everything would still work great, except that random events before
// the first tic of a level (like random RTS spawn) would be
// predictable.

int I_GetTime(void);
// Returns a value that increases monotonically over time.  The value
// should increase by TICRATE every second (TICRATE is currently 35).
// The starting value should be close to zero.

int I_GetMillies(void);

u32_t I_ReadMicroSeconds(void);
// Like I_GetTime(), this function returns a value that increases
// monotonically over time, but in this case the value increases by
// 1000000 every second (i.e. each unit is 1 microsecond).  Since this
// value will overflow regularly (assuming `long' is 32 bits), the
// caller should check for this situation.


//--------------------------------------------------------
//  MUSIC functions.
//--------------------------------------------------------
//
// -ACB- 1999/09/19 moved from I_Music.H

//--------------------------------------------------------
//  VIDEO functions.
//--------------------------------------------------------
//
// -ACB- 1999/09/20 Moved from I_Video.H

//class scrmode_c;

void I_StartupGraphics(void);
// Initialises the graphics system.  This should be called by
// I_SystemStartup(), the main code never calls this function
// directly.  This function should determine what video modes are
// available, and call V_AddAvailableResolution() for them.

void I_ShutdownGraphics(void);
// Shuts down the graphics system.  This is the companion function to
// I_StartupGraphics.  Note that this should be called by
// I_SystemStartup(), the main code never calls this function.

void I_StartFrame(void);
// Called to prepare the screen for rendering (if necessary).

void I_FinishFrame(void);
// Called when the current frame has finished being rendered.  This
// routine typically copies the screen buffer to the video memory.  It
// may also handle double/triple buffering here.

void I_GetDesktopSize(int *width, int *height);
// returns the desktop resolution.

//bool I_SetScreenSize(scrmode_c *mode);
// Tries to set the video card to the given mode (or open a window).
// If there already was a valid mode (or open window), this call
// should replace it.  The previous contents (including the palette)
// is assumed to be lost.
//
// Returns true if successful, otherwise false.  The platform is free
// to select a working mode if the given mode was not possible, in
// which case the values of the global variables SCREENWIDTH,
// SCREENHEIGHT and SCREENBITS must be updated.

//void I_GrabCursor(bool enable);

//void I_SetGamma(float gamma);

//--------------------------------------------------------
//  Movie functions.
//--------------------------------------------------------
//
// -CW- 2017

void I_StartupMovie(void);
void I_ShutdownMovie(void);
int I_PlayMovie(char *name);


void I_Logf(const char *message,...) GCCATTR((format(printf, 1, 2)));

#define L_WriteDebug  I_Debugf
#define L_WriteLog    I_Logf


#ifdef DREAMCAST

// special support functions for Dreamcast specific hardware
int DC_LoadFromVMU(char *dst);
int DC_SaveToVMU(char *src, int ftype);

#endif

#endif /*__I_SYSTEM_H__*/

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab

//----------------------------------------------------------------------------
//  EDGE2 SDL Controller Stuff
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

#include <kos.h>

#include "i_defs.h"

#include "../dm_defs.h"
#include "../dm_state.h"
#include "../e_event.h"
#include "../e_input.h"
#include "../e_main.h"
#include "../m_argv.h"
#include "../r_modes.h"

#include <dc/maple.h>
#include <dc/maple/controller.h>
#include <dc/maple/purupuru.h>
#include <dc/maple/mouse.h>
#include <dc/maple/keyboard.h>

#undef DEBUG_KB

cvar_c in_keypad;
cvar_c in_warpmouse;


static bool nojoy;  // what a wowser, joysticks completely disabled
//static bool stick_disabled; // don't use analogue stick
static bool ff_enable;

bool has_mouse;
bool has_keyboard;

int num_joys;
int joystick_device;   // 0 for none
int joystick_device2;  // 0 for none
int twinstick_device;  // 0 for none
int twinstick_device2; // 0 for none

extern bool menuactive;

static int ff_timeout[2]   = { 0, 0 };
static int ff_frequency[2] = { 0, 0 };
static int ff_intensity[2] = { 0, 0 };
static int ff_select[2]    = { 0, 0 };

static maple_device_t *cont, *kbd, *mky;
static cont_state_t *cstate;
static kbd_state_t *kstate;
static mouse_state_t *mstate;

int key_table[128] = {
 0, 0, 0, 0, 'a', 'b', 'c', 'd',                // 00-07
 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',        // 08-0F
 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',        // 10-17
 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',        // 18-1F
 '3', '4', '5', '6', '7', '8', '9', '0',        // 20-27
 KEYD_ENTER, KEYD_ESCAPE, KEYD_BACKSPACE, KEYD_TAB, ' ', '-', '=', '[',        // 28-2F
 ']', '\\', 0, ';', '\'', '`', ',', '.',        // 30-37
 '/', 0, KEYD_F1, KEYD_F2, KEYD_F3, KEYD_F4, KEYD_F5, KEYD_F6,        // 38-3F
 KEYD_F7, KEYD_F8, KEYD_F9, KEYD_F10, KEYD_F11, KEYD_F12, KEYD_PRTSCR, KEYD_SCRLOCK,        // 40-47
 KEYD_PAUSE, KEYD_INSERT, KEYD_HOME, KEYD_PGUP, KEYD_DELETE, KEYD_END, KEYD_PGDN, KEYD_RIGHTARROW,        // 48-4F
 KEYD_LEFTARROW, KEYD_DOWNARROW, KEYD_UPARROW, KEYD_NUMLOCK, KEYD_KP_SLASH, KEYD_KP_STAR, KEYD_KP_MINUS, KEYD_KP_PLUS,        // 50-57
 KEYD_KP_ENTER, KEYD_KP1, KEYD_KP2, KEYD_KP3, KEYD_KP4, KEYD_KP5, KEYD_KP6, KEYD_KP7,        // 58-5F
 KEYD_KP8, KEYD_KP9, KEYD_KP0, KEYD_KP_DOT, 0, 0, 0, 0,        // 60-67
 0, 0, 0, 0, 0, 0, 0, 0,        // 68-6F
 0, 0, 0, 0, 0, 0, 0, 0,        // 70-77
 0, 0, 0, 0, 0, 0, 0, 0         // 78-7F
};

static unsigned int previous[2];
static int prev_mkyb, prev_mkyw, prev_shift;
static int tshift[2];

void DC_Tactile (int freq, int intensity, int select)
{
    ff_frequency[select] = freq;
    ff_intensity[select] = intensity;
    ff_select[select] = 0;
    ff_timeout[select] = I_GetTime() + 35;

#ifdef DEBUG_DREAMEDGE
    I_Printf("I_Tactile: %d, %d, %d\n", freq, intensity, select);
#endif
    // FF will be handled at the same time as the events
}

static void dc_handle_ff(int joy)
{
    maple_device_t *jump;
    int ix = ((joy + 1) == joystick_device) ? 0 : 1;

    jump = maple_enum_type(joy, MAPLE_FUNC_PURUPURU);

    if (ff_timeout[ix] && jump)
    {
        if (I_GetTime() >= ff_timeout[ix])
        {
            static purupuru_effect_t effect[2];
            // stop force feedback if still on
            effect[ix].duration = 0x00;
            effect[ix].effect2 = 0x00;
            effect[ix].effect1 = 0x00;
            effect[ix].special = PURUPURU_SPECIAL_MOTOR1;
            purupuru_rumble(jump, &effect[ix]);
            ff_timeout[ix] = 0;
            ff_intensity[ix] = 0;
        }
    }

    if (ff_intensity[ix] && jump)
    {
        static purupuru_effect_t effect[2];
        // start force-feedback
        effect[ix].duration = 30;
        effect[ix].effect2 = ff_frequency[ix];
        effect[ix].effect1 = PURUPURU_EFFECT1_INTENSITY(ff_intensity[ix]);
        effect[ix].special = PURUPURU_SPECIAL_MOTOR1 | ff_select[ix];
        purupuru_rumble(jump, &effect[ix]);
        ff_intensity[ix] = 0;
    }
}

static void dc_handle_ts(int joy)
{
    event_t event;
    int buttons, i, j;
    int ix = ((joy + 1) == twinstick_device) ? 0 : 1;

    cont = maple_enum_type(joy, MAPLE_FUNC_CONTROLLER);

    if (cont)
        cstate = (cont_state_t *)maple_dev_status(cont);
    else
        cstate = NULL;

    if (!cstate)
    {
        buttons = previous[ix] = 0;
		return;
    }

	buttons = cstate->buttons;

    // now handle controller buttons
    if (buttons == previous[ix])
        return; // no changes

    for (i=0, j=1; i<18; i++, j<<=1)
    {
        if ((buttons ^ previous[ix]) & j)
        {
            // button changed
            event.type = (buttons & j) ? ev_keydown : ev_keyup;
			if (menuactive)
			{
				switch (i)
				{
					case PAD_B:
						event.value.key.sym = KEYD_ESCAPE;
						break;
					case PAD_A:
					case PAD_X:
						event.value.key.sym = KEYD_ENTER;
						break;
					case PAD_START:
						event.value.key.sym = KEYD_ENTER;
						break;
					case PAD_UP:
					case PAD_UP2:
						event.value.key.sym = KEYD_UPARROW;
						break;
					case PAD_DOWN:
					case PAD_DOWN2:
						event.value.key.sym = KEYD_DOWNARROW;
						break;
					case PAD_LEFT:
					case PAD_LEFT2:
						event.value.key.sym = KEYD_LEFTARROW;
						break;
					case PAD_RIGHT:
					case PAD_RIGHT2:
						event.value.key.sym = KEYD_RIGHTARROW;
						break;
					case PAD_Y:
					case PAD_D:
						event.value.key.sym = 'y';
						break;
					default:
						event.value.key.sym = KEYD_JOY_1_1 + i + (ix * 18);
						break;
				}
			}
			else
			{
				if (i == PAD_START)
					event.value.key.sym = KEYD_ESCAPE;
				else if (i == PAD_Y)
					tshift[ix] = event.type;
				else
				{
					if (tshift[ix] == ev_keydown)
					{
						// Y down, fake some keys if left stick used
						switch (i)
						{
							case PAD_UP:
								event.value.key.sym = !ix ? KEYD_PGUP : KEYD_HOME;
								break;
							case PAD_DOWN:
								event.value.key.sym = !ix ? KEYD_PGDN : KEYD_END;
								break;
							case PAD_LEFT:
								event.value.key.sym = !ix ? 'k' : 'o';
								break;
							case PAD_RIGHT:
								event.value.key.sym = !ix ? 'l' : 'p';
								break;
							default:
								event.value.key.sym = KEYD_JOY_1_1 + i + (ix * 18);
								break;
						}
					}
					else
						event.value.key.sym = KEYD_JOY_1_1 + i + (ix * 18);
				}
			}
			E_PostEvent(&event);
        }
    }
    previous[ix] = buttons;
}

static void dc_handle_pad(int joy)
{
    event_t event;
    unsigned int buttons;
    int i, j;
    int ix = ((joy + 1) == joystick_device) ? 0 : 1;

	if ((joy + 1) == twinstick_device || (joy + 1) == twinstick_device2)
	{
		dc_handle_ts(joy);
		return;
	}

    cont = maple_enum_type(joy, MAPLE_FUNC_CONTROLLER);

    if (cont)
        cstate = (cont_state_t *)maple_dev_status(cont);
    else
        cstate = NULL;

    if (!cstate)
        return;
    else
        buttons = cstate->buttons;

    // add fake buttons from analogue inputs
	buttons |= cstate->ltrig > 16 ? (1 << PAD_LTRIGGER) : 0;
	buttons |= cstate->rtrig > 16 ? (1 << PAD_RTRIGGER) : 0;
    buttons |= cstate->joyx > 16 ? (1 << PAD_RIGHTS) : 0;
    buttons |= cstate->joyx < -16 ? (1 << PAD_LEFTS) : 0;
    buttons |= cstate->joyy > 16 ? (1 << PAD_DOWNS) : 0;
    buttons |= cstate->joyy < -16 ? (1 << PAD_UPS) : 0;
    // add fake buttons from alternate combo (Y + button)
    if (buttons & (1 << PAD_Y))
    {
        buttons |= (buttons & 0x01FF) << 22;
        buttons &= ~(buttons & 0x01FF);
        if (buttons & (1 << PAD_X))
        {
            buttons |= 1 << PAD_ALT_X;
            buttons &= ~(1 << PAD_X);
        }
    }

    // now handle controller buttons
    if (buttons == previous[ix])
        return; // no changes

    for (i=0, j=1; i<32; i++, j<<=1)
    {
        if ((buttons ^ previous[ix]) & j)
        {
            // button changed
            event.type = (buttons & j) ? ev_keydown : ev_keyup;
			if (menuactive)
			{
				switch (i)
				{
					case PAD_B:
						event.value.key.sym = KEYD_ESCAPE;
						break;
					case PAD_A:
						event.value.key.sym = KEYD_ENTER;
						break;
					case PAD_START:
						event.value.key.sym = KEYD_ENTER;
						break;
					case PAD_UP:
                    case PAD_UPS:
						event.value.key.sym = KEYD_UPARROW;
						break;
					case PAD_DOWN:
                    case PAD_DOWNS:
						event.value.key.sym = KEYD_DOWNARROW;
						break;
					case PAD_LEFT:
                    case PAD_LEFTS:
						event.value.key.sym = KEYD_LEFTARROW;
						break;
					case PAD_RIGHT:
                    case PAD_RIGHTS:
						event.value.key.sym = KEYD_RIGHTARROW;
						break;
					case PAD_Y:
						event.value.key.sym = 'y';
						break;
					default:
						event.value.key.sym = KEYD_JOY_1_1 + i + (ix * 32);
						break;
				}
			}
			else
			{
				if (i == PAD_START)
					event.value.key.sym = KEYD_ESCAPE;
				else
					event.value.key.sym = KEYD_JOY_1_1 + i + (ix * 32);
			}
			E_PostEvent(&event);
        }
    }
    previous[ix] = buttons;
}

static void dc_handle_mky(void)
{
    event_t event;
    short mousex, mousey;
    int mouseb, mousew;

    mky = maple_enum_type(0, MAPLE_FUNC_MOUSE);

    if (mky)
        mstate = (mouse_state_t *)maple_dev_status(mky);
    else
        mstate = NULL;

    // use mouse as mouse if found
    if (mstate)
    {
        mousex = mstate->dx;
        mousey = -mstate->dy;
        // handle mouse move
        if (mousex || mousey)
        {
            event.type = ev_mouse;
            event.value.mouse.dx = mousex;
            event.value.mouse.dy = mousey;
            E_PostEvent(&event);
        }

		// find wheel direction
        mousew = (mstate->dz < 0) ? 1 : (mstate->dz > 0) ? 2 : 0;
        if (prev_mkyw == -1)
            prev_mkyw = mousew;

        // handle mouse wheel
        if (mousew != prev_mkyw)
        {
			//I_Printf("Mouse wheel = %d\n", mstate->dz);
            if ((mousew ^ prev_mkyw) & 1)
            {
                // mouse wheel up
                if (mousew & 1)
                {
                    event.type = ev_keydown;
                    event.value.key.sym = KEYD_WHEEL_UP;
                    E_PostEvent(&event);
                }
                else
                {
                    event.type = ev_keyup;
                    event.value.key.sym = KEYD_WHEEL_UP;
                    E_PostEvent(&event);
                }
            }
            if ((mousew ^ prev_mkyw) & 2)
            {
                // mouse wheel down
                if (mousew & 2)
                {
                    event.type = ev_keydown;
                    event.value.key.sym = KEYD_WHEEL_DN;
                    E_PostEvent(&event);
                }
                else
                {
                    event.type = ev_keyup;
                    event.value.key.sym = KEYD_WHEEL_DN;
                    E_PostEvent(&event);
                }
            }
            prev_mkyw = mousew;
        }

        mouseb = (mstate->buttons & MOUSE_LEFTBUTTON ? 1 : 0) | (mstate->buttons & MOUSE_RIGHTBUTTON ? 2 : 0) | (mstate->buttons & MOUSE_SIDEBUTTON ? 4 : 0);
        if (prev_mkyb == -1)
            prev_mkyb = mouseb;

        // handle mouse buttons
        if (mouseb != prev_mkyb)
        {
            if ((mouseb ^ prev_mkyb) & 1)
            {
                // left mouse button changed
                if (mouseb & 1)
                {
                    event.type = ev_keydown;
                    event.value.key.sym = KEYD_MOUSE1;
                    E_PostEvent(&event);
                }
                else
                {
                    event.type = ev_keyup;
                    event.value.key.sym = KEYD_MOUSE1;
                    E_PostEvent(&event);
                }
            }
            if ((mouseb ^ prev_mkyb) & 2)
            {
                // right mouse button changed
                if (mouseb & 2)
                {
                    event.type = ev_keydown;
                    event.value.key.sym = KEYD_MOUSE2;
                    E_PostEvent(&event);
                }
                else
                {
                    event.type = ev_keyup;
                    event.value.key.sym = KEYD_MOUSE2;
                    E_PostEvent(&event);
                }
            }
            if ((mouseb ^ prev_mkyb) & 4)
            {
                // side mouse button changed
                if (mouseb & 4)
                {
                    event.type = ev_keydown;
                    event.value.key.sym = KEYD_MOUSE3;
                    E_PostEvent(&event);
                }
                else
                {
                    event.type = ev_keyup;
                    event.value.key.sym = KEYD_MOUSE3;
                    E_PostEvent(&event);
                }
            }
            prev_mkyb = mouseb;
        }
    }
}

static void dc_handle_kbd(void)
{
    event_t event;

    kbd = maple_enum_type(0, MAPLE_FUNC_KEYBOARD);

    if (kbd)
        kstate = (kbd_state_t *)maple_dev_status(kbd);
    else
        kstate = NULL;

    // use keyboard if found
    if (kstate)
    {
        static uint8 prev_matrix[128];
        int i;

        if (prev_shift == -1)
        {
            prev_shift = kstate->shift_keys;
            memset(prev_matrix, 0, 128);
        }

        switch(kstate->shift_keys ^ prev_shift)
        {
            case KBD_MOD_LALT:
            case KBD_MOD_RALT:
                if (kstate->shift_keys & (KBD_MOD_LALT | KBD_MOD_RALT))
                {
                    event.type = ev_keydown;
                    event.value.key.sym = KEYD_RALT;
                    E_PostEvent (&event);
                }
                else
                {
                    event.type = ev_keyup;
                    event.value.key.sym = KEYD_RALT;
                    E_PostEvent (&event);
                }
                break;

            case KBD_MOD_LCTRL:
            case KBD_MOD_RCTRL:
                if (kstate->shift_keys & (KBD_MOD_LCTRL | KBD_MOD_RCTRL))
                {
                    event.type = ev_keydown;
                    event.value.key.sym = KEYD_RCTRL;
                    E_PostEvent (&event);
                }
                else
                {
                    event.type = ev_keyup;
                    event.value.key.sym = KEYD_RCTRL;
                    E_PostEvent (&event);
                }
                break;

            case KBD_MOD_LSHIFT:
            case KBD_MOD_RSHIFT:
                if (kstate->shift_keys & (KBD_MOD_LSHIFT | KBD_MOD_RSHIFT))
                {
                    event.type = ev_keydown;
                    event.value.key.sym = KEYD_RSHIFT;
                    E_PostEvent (&event);
                }
                else
                {
                    event.type = ev_keyup;
                    event.value.key.sym = KEYD_RSHIFT;
                    E_PostEvent (&event);
                }
                break;
        }
        prev_shift = kstate->shift_keys;

        // handle regular keys
        for (i=4; i<0x65; i++)
        {
            if (kstate->matrix[i] != 0 && prev_matrix[i] == 0 && key_table[i] != 0)
            {
                event.type = ev_keydown;
                event.value.key.unicode = event.value.key.sym = key_table[i];
                E_PostEvent (&event);
            }
            else if (kstate->matrix[i] == 0 && prev_matrix[i] != 0 && key_table[i] != 0)
            {
                event.type = ev_keyup;
                event.value.key.unicode = event.value.key.sym = key_table[i];
                E_PostEvent (&event);
            }
        }
        memcpy(prev_matrix, kstate->matrix, 128);
    }
}

void dc_get_controllers()
{
    int i;

    // init DC controller stuff
    twinstick_device = twinstick_device2 = 0;
    for (i=num_joys=0; i<4; i++)
    {
        cont = maple_enum_type(i, MAPLE_FUNC_CONTROLLER);
        if (!cont)
            break;

        num_joys++;

        // look for Twin Sticks
        if (memcmp(cont->info.product_name, "Twin Stick", 10))
            continue; // not Twin Stick, skip
        if (!twinstick_device)
            twinstick_device = i + 1;
        else if (!twinstick_device2)
            twinstick_device2 = i + 1;
    }
	//default to using first two controllers
    joystick_device = num_joys ? 1 : 0;
    joystick_device2 = num_joys >= 2 ? 2 : 0;

	// if have Twin Stick controller, default to using it
    if (twinstick_device)
		joystick_device = twinstick_device;
    if (twinstick_device2)
		joystick_device2 = twinstick_device2;

	// look for conflict between sticks
	if (joystick_device == joystick_device2)
		joystick_device2 = joystick_device == 2 ? 1 : 2;

    kbd = maple_enum_type(0, MAPLE_FUNC_KEYBOARD);
    has_keyboard = kbd ? true : false;
    mky = maple_enum_type(0, MAPLE_FUNC_MOUSE);
    has_mouse = mky ? true : false;
}

int I_JoyGetAxis(int n)  // n begins at 0
{
    return 0;
}

void I_CentreMouse(void)
{
}

void I_ShowJoysticks(void)
{
    if (nojoy)
    {
        I_Printf("Joystick system is disabled.\n");
        return;
    }

    if (num_joys == 0)
    {
        I_Printf("No joysticks found.\n");
        return;
    }

    I_Printf("Joysticks:\n");

    for (int i = 0; i < num_joys; i++)
    {
        static const char *name[4] = {
            "JOY1", "JOY2", "JOY3", "JOY4"
        };
        I_Printf("  %2d : %s\n", i+1, name[i]);
    }
}

/****** Input Event Generation ******/

void I_StartupControl(void)
{
    ff_enable = M_CheckParm ("-rumble") ? true : false;
//  stick_disabled = M_CheckParm ("-noanalog") ? true : false;
    nojoy = M_CheckParm ("-nojoy") ? true : false;

    I_Printf("Num Joys: %d, Joy1: %d, Joy2: %d\n %s, %s, %s, %s\n",
        num_joys, joystick_device, joystick_device2, has_keyboard ? "KEYB" : "No KEYB",
        has_mouse ? "MKY" : "No MKY", ff_enable ? "FF" : "No FF",
        nojoy ? "Pads Disabled" : "Pads Enabled");

    I_Printf("Twin Stick 1: %d, Twin Stick 2: %d\n", twinstick_device, twinstick_device2);

    previous[0] = previous[1] = prev_mkyb = prev_mkyw = prev_shift = 0;
    tshift[0] = tshift[1] = ev_keyup;
}

void I_ControlGetEvents(void)
{
    if (!nojoy && joystick_device)
        dc_handle_pad(joystick_device - 1);
    if (!nojoy && joystick_device2)
        dc_handle_pad(joystick_device2 - 1);
    if (ff_enable && joystick_device)
        dc_handle_ff(joystick_device - 1);
    if (ff_enable && joystick_device2)
        dc_handle_ff(joystick_device2 - 1);
    if (has_keyboard)
        dc_handle_kbd();
    if (has_mouse)
        dc_handle_mky();
}

void I_ShutdownControl(void)
{
}


int I_GetTime(void)
{
    uint32 t = timer_ms_gettime64();

    // more complex than "t*35/1000" to give more accuracy
    return (t / 1000) * 35 + (t % 1000) * 35 / 1000;
}


//
// Same as I_GetTime, but returns time in milliseconds
//
int I_GetMillies(void)
{
    return (int)timer_ms_gettime64();
}


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab

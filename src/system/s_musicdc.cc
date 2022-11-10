//----------------------------------------------------------------------------
//  EDGE2 Music handling Code
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
// -ACB- 1999/11/13 Written
//

#include <kos.h>

#include "i_defs.h"

#include <stdlib.h>

#include "../../epi/file.h"
#include "../../epi/filesystem.h"

#include "../../ddf/main.h"

#include "../dm_state.h"
#include "../s_sound.h"
#include "../s_music.h"
#include "../m_misc.h"
#include "../w_wad.h"

extern "C" {
#include "libWildMidi/include/wildmidi_lib.h"
}

#include <dc/sound/sound.h>
#include <dc/sound/sfxmgr.h>
#include <dc/sound/stream.h>

#include <sndoggvorbis.h>
#include <opusplay.h>
#define DUMB_OFF_T_CUSTOM int64_t
#include "dumb/include/dumb.h"

#define AICA_VOL_LOW 128.0f
#define AICA_VOL_HIGH 255.0f

// music slider value
int mus_volume;

bool nomusic = false;
bool nocdmusic = false;

static int  entry_playing = -1;
static bool entry_looped;
static bool playing_cd = false;
static bool playing_ogg = false;
static bool playing_opus = false;
static bool playing_midi = false;
static bool playing_mod = false;
static bool music_paused = false;

static bool is_opus = false;

static midi *midi_ptr = NULL;
static struct _WM_Info *wm_info = NULL;
static byte *mus_midi_data = NULL;
static bool midi_pause = false;
static bool midi_done = false;
extern bool nomidi;

static DUH *mod_file;
static DUH_SIGRENDERER *mod_renderer;
static bool mod_pause = false;
static bool mod_done = false;
static sample_t **mod_smps = NULL;
static long mod_smps_sz;
static bool dumbfs_init = false;

#define MOD_SAMPLE_RATE 22050

snd_stream_hnd_t stream_hnd = SND_STREAM_INVALID;

// buffer size, must be <= SND_STREAM_BUFFER_MAX
#define MIDI_BUF_SIZE 65536

uint8 pcm_buffer[MIDI_BUF_SIZE];

// EPI filesystem functions for libdumb
static int dumb_epi_skip(void *f, dumb_off_t n)
{
    epi::file_c *F = (epi::file_c *)f;

    //I_Printf("dumb_epi_skip\n");

    F->Seek((int)n, epi::file_c::SEEKPOINT_CURRENT);
    return 0;
}

static int dumb_epi_getc(void *f)
{
    epi::file_c *F = (epi::file_c *)f;
    uint8_t ret;

    //I_Printf("dumb_epi_getc\n");

    if (F->Read(&ret, 1) == 1)
    {
        //I_Printf("%x\n", ret);
        return (int)ret;
    }

    return -1;
}

static dumb_ssize_t dumb_epi_getnc(char *ptr, size_t n, void *f)
{
    epi::file_c *F = (epi::file_c *)f;

    //I_Printf("dumb_epi_getnc\n");

    dumb_ssize_t ret = (dumb_ssize_t)F->Read(ptr, (int)n);

    //I_Printf("%d / %d\n", n, ret);
    return ret;
}

static void *dumb_epi_open(const char *filename)
{
    void *ret;

    sscanf(filename, "%p", &ret);
    //I_Printf("%p / %s\n", ret, filename);
    return ret;
}


static void dumb_epi_close(void *f)
{
    epi::file_c *F = (epi::file_c *)f;

    //I_Printf("dumb_epi_close\n");

    delete F;
}

static int dumb_epi_seek(void *f, dumb_off_t n)
{
    epi::file_c *F = (epi::file_c *)f;

    //I_Printf("dumb_epi_seek %d\n", (int)n);

    F->Seek((int)n, epi::file_c::SEEKPOINT_START);
    return 0;
}

static dumb_off_t dumb_epi_get_size(void *f)
{
    epi::file_c *F = (epi::file_c *)f;

    //I_Printf("dumb_epi_get_size\n");

    return (dumb_off_t)F->GetLength();
}

static DUMBFILE_SYSTEM epi_dfs = {
    &dumb_epi_open,
    &dumb_epi_skip,
    &dumb_epi_getc,
    &dumb_epi_getnc,
    &dumb_epi_close,
    &dumb_epi_seek,
    &dumb_epi_get_size
};

static void *stream_callback(snd_stream_hnd_t hnd, int size, int *size_out)
{
    (void)hnd;
    int num_samples;

	if (size > MIDI_BUF_SIZE)
		num_samples = MIDI_BUF_SIZE / 4;
	else
		num_samples = size / 4;

	// fill buffer with either music or silence
	if (playing_midi && !midi_pause && !midi_done)
	{
		int output_result;
		int samples_left = wm_info->approx_total_samples - wm_info->current_sample;
		if (samples_left <= num_samples)
		{
			// end of score, return last samples and (possibly) loop
			output_result = WildMidi_GetOutput(midi_ptr, (int8_t *)pcm_buffer, samples_left * 4);

			midi_done = !entry_looped; // stop playing if not looping
			if (entry_looped)
			{
				uint32 start_pos = 0;
				// start music from beginning
				WildMidi_FastSeek(midi_ptr, &start_pos);
				wm_info = WildMidi_GetInfo(midi_ptr);
				output_result = WildMidi_GetOutput(midi_ptr, (int8_t *)&pcm_buffer[samples_left * 4], (num_samples - samples_left) * 4);
			}
			else
			{
				// clear rest of buffer
				memset(&pcm_buffer[samples_left * 4], 0, (num_samples - samples_left) * 4);
			}
		}
		else
		{
			output_result = WildMidi_GetOutput(midi_ptr, (int8_t *)pcm_buffer, (num_samples * 4));
		}
		wm_info = WildMidi_GetInfo(midi_ptr);
	}
	else if (playing_mod && !mod_pause && !mod_done)
	{
        int got = duh_render_int(mod_renderer, &mod_smps, &mod_smps_sz, 16, 0, 1.0f,
                                 65536.0f / MOD_SAMPLE_RATE, num_samples, pcm_buffer);
        if (got < num_samples)
        {
            if (entry_looped)
            {
                // restart module
                duh_end_sigrenderer(mod_renderer);
                mod_renderer = duh_start_sigrenderer(mod_file, 0, 2, 0);
                DUMB_IT_SIGRENDERER *itsr = duh_get_it_sigrenderer(mod_renderer);
                dumb_it_set_loop_callback(itsr, &dumb_it_callback_terminate, NULL);
                dumb_it_set_xm_speed_zero_callback(itsr, &dumb_it_callback_terminate, NULL);
                dumb_it_set_resampling_quality(itsr, DUMB_RQ_LINEAR);
                // fill rest of buffer
                got = duh_render_int(mod_renderer, &mod_smps, &mod_smps_sz, 16, 0, 1.0f,
                                     65536.0f / MOD_SAMPLE_RATE, num_samples - got, &pcm_buffer[got*4]);
            }
            else
            {
                mod_done = true;
                // clear rest of buffer
                memset(&pcm_buffer[got * 4], 0, (num_samples - got) * 4);
            }
        }
    }
	else
	{
		// clear buffer
		memset(pcm_buffer, 0, num_samples * 4);
	}

	*size_out = num_samples * 4;
	return pcm_buffer;
}


void S_ChangeMusic(int entrynum, bool loop)
{
	if (nomusic)
		return;

	// -AJA- playlist number 0 reserved to mean "no music"
	if (entrynum <= 0)
	{
		S_StopMusic();
		return;
	}

	// -AJA- don't restart the current song (DOOM compatibility)
	if (entrynum == entry_playing && entry_looped)
		return;

	S_StopMusic();

	entry_playing = entrynum;
	entry_looped  = loop;

	// when we cannot find the music entry, no music will play
	const pl_entry_c *play = playlist.Find(entrynum);
	if (!play)
	{
		I_Warning("Could not find music entry [%d]\n", entrynum);
		return;
	}

	int vol = (int)((float)mus_volume * (AICA_VOL_HIGH - AICA_VOL_LOW) / (float)(SND_SLIDER_NUM - 1) + AICA_VOL_LOW);

	if (play->type == MUS_MP3)
	{
		I_Warning("S_ChangeMusic: MP3 music no longer supported.\n");
		return;
	}
	else if (play->type == MUS_CD)
	{
		if (nocdmusic)
		{
			I_Warning("S_ChangeMusic: CDDA disabled.\n");
			return;
		}
		entry_playing = atoi(play->info);
		entry_looped = loop;
		cdrom_cdda_play(entry_playing, entry_playing, loop ? 15 : 0, CDDA_TRACKS);
		spu_cdda_volume(vol >> 4, vol >> 4);
		spu_cdda_pan(0, 31);
		playing_cd = true;
		return;
	}
	else if ((play->type == MUS_MUS) || (play->type == MUS_MIDI))
	{
		if (nomidi)
		{
			I_Warning("S_ChangeMusic: MIDI not available.\n");
			return;
		}
	}
#ifdef NO_OGG
	else if (play->type == MUS_OGG)
	{
		I_Warning("S_ChangeMusic: OGG support disabled.\n");
		return;
	}
#endif

	// open the file or lump, and read it into memory
	epi::file_c *F;

	switch (play->infotype)
	{
		case MUSINF_FILE:
		{
			std::string fn = M_ComposeFileName(game_dir.c_str(), play->info.c_str());

			F = epi::FS_Open(fn.c_str(), epi::file_c::ACCESS_READ | epi::file_c::ACCESS_BINARY);

			if (! F)
			{
				I_Warning("S_ChangeMusic: Can't find file '%s'\n", fn.c_str());
				return;
			}
			I_Printf("S_ChangeMusic: Reading file '%s'\n", fn.c_str());

			break;
		}

		case MUSINF_LUMP:
		{
			int lump = W_CheckNumForName(play->info);
			if (lump < 0)
			{
				I_Warning("S_ChangeMusic: Can't find LUMP '%s'\n", play->info.c_str());
				return;
			}
			I_Printf("S_ChangeMusic: Reading LUMP '%s'\n", play->info.c_str());

			F = W_OpenLump(lump);
			break;
		}

		default:
			I_Printf("S_ChangeMusic: invalid method %d for music\n", play->infotype);
			return;
	}

	int length = F->GetLength();
//	byte *data = F->LoadIntoMemory();
	byte *data = new byte[256];

	if (!data)
	{
		delete F;
		I_Warning("S_ChangeMusic: Error loading data.\n");
		return;
	}

	if (length < 32)
	{
		delete F;
		delete[] data;
		I_Printf("S_ChangeMusic: ignored short data (%d bytes)\n", length);
		return;
	}

	F->Read(data, 32);
	if (memcmp(data, "Ogg", 3) == 0)
	{
		file_t fp;

		if (memcmp(&data[0x1c], "Opus", 4) == 0)
		{
			is_opus = true;
			fp = fs_open("/ram/temp.opus", O_WRONLY);
		}
		else
		{
			is_opus = false;
			fp = fs_open("/ram/temp.ogg", O_WRONLY);
		}
		if (fp < 0)
		{
			I_Printf("%s\n", "S_ChangeMusic: couldn't open OGG temp file\n");
			delete F;
			delete[] data;
			return;
		}
		F->Seek(0, epi::file_c::SEEKPOINT_START);
		for (int ix=0; ix<length; ix+=256)
		{
			int left = (length - ix) >= 256 ? 256 : (length - ix);
			F->Read(data, left);
			fs_write(fp, data, left);
		}
		fs_close(fp);
		delete F;
		delete[] data;
	}
	else if (memcmp(data, "MUS", 3) == 0)
	{
		delete[] data;
		F->Seek(0, epi::file_c::SEEKPOINT_START);
		mus_midi_data = F->LoadIntoMemory();
		delete F;
	}
	else if (memcmp(data, "MTh", 3) == 0)
	{
		delete[] data;
		F->Seek(0, epi::file_c::SEEKPOINT_START);
		mus_midi_data = F->LoadIntoMemory();
		delete F;
	}
	else if (play->type == MUS_MOD)
	{
		delete[] data;
		F->Seek(0, epi::file_c::SEEKPOINT_START);
//		data = F->LoadIntoMemory();
//		delete F;
        if (!dumbfs_init)
        {
            register_dumbfile_system(&epi_dfs);
            dumbfs_init = true;
            I_Printf("S_ChangeMusic: registered dumbfile system for epi\n");
        }
	}
	else
	{
		delete F;
		delete[] data;
		I_Printf("%s\n", "S_ChangeMusic: ignored unknown music");
		return;
	}

	if (play->type == MUS_OGG)
	{
		if (is_opus)
		{
			opusplay_init();
			opusplay_play_file("/ram/temp.opus", loop);
			opusplay_set_volume(vol);
			playing_opus = true;
		}
		else
		{
			sndoggvorbis_init();
			sndoggvorbis_start("/ram/temp.ogg", loop);
			sndoggvorbis_volume(vol);
			playing_ogg = true;
		}
	}
	else if ((play->type == MUS_MUS) || (play->type == MUS_MIDI))
	{
		midi_pause = midi_done = false;
		// play mus/midi using WildMidi
		midi_ptr = WildMidi_OpenBuffer(mus_midi_data, length);
		if (midi_ptr)
		{
			wm_info = WildMidi_GetInfo(midi_ptr);
			//WildMidi_LoadSamples(midi_ptr);
			WildMidi_MasterVolume(127);

			// setup stream
			stream_hnd = snd_stream_alloc(NULL, MIDI_BUF_SIZE);
			if(stream_hnd == SND_STREAM_INVALID)
			{
				I_Printf("%s\n", " snd_stream_alloc() failed");
				remove("/ram/temp.mid");
				return;
			}
			snd_stream_reinit(stream_hnd, &stream_callback);
			snd_stream_start(stream_hnd, 44100, 1);
            snd_stream_volume(stream_hnd, vol);
		}
		else
		{
			I_Printf("%s\n", "WildMidi_Open failed");
			remove("/ram/temp.mid");
			return;
		}
		playing_midi = true;
	}
	else if (play->type == MUS_MOD)
	{
		mod_pause = mod_done = false;

        // setup stream
        stream_hnd = snd_stream_alloc(NULL, MIDI_BUF_SIZE);
        if(stream_hnd == SND_STREAM_INVALID)
        {
            I_Printf("%s\n", " snd_stream_alloc() failed");
            delete[] data;
            return;
        }
        snd_stream_reinit(stream_hnd, &stream_callback);
        snd_stream_start(stream_hnd, MOD_SAMPLE_RATE, 1);
        snd_stream_volume(stream_hnd, vol);

        I_Printf("S_ChangeMusic: loading module\n");
//        DUMBFILE *mod_df = dumbfile_open_memory((const char*)data, length);
//        mod_file = dumb_read_any(mod_df, 0, 0);
//        dumbfile_close(mod_df);
//        delete[] data;
        char temp[16];
        sprintf(temp, "%p", F);
        mod_file = dumb_load_any((const char*)temp, 0, 0);
        if (!mod_file)
        {
            I_Printf("S_ChangeMusic: module failed to load\n");
            return;
        }
        I_Printf("S_ChangeMusic: module loaded\n");
        mod_renderer = duh_start_sigrenderer(mod_file, 0, 2, 0);
        DUMB_IT_SIGRENDERER *itsr = duh_get_it_sigrenderer(mod_renderer);
        dumb_it_set_loop_callback(itsr, &dumb_it_callback_terminate, NULL); // no auto-looping
        dumb_it_set_xm_speed_zero_callback(itsr, &dumb_it_callback_terminate, NULL);
        dumb_it_set_resampling_quality(itsr, DUMB_RQ_LINEAR);

		playing_mod = true;
	}

    thd_sleep(50);
}


void S_ResumeMusic(void)
{
	if (!music_paused)
		return;

	music_paused = false;

	if (playing_ogg)
		sndoggvorbis_start("/ram/temp.ogg", entry_looped);
	else if (playing_opus)
		opusplay_play_file("/ram/temp.opus", entry_looped);
	else if (playing_midi)
		midi_pause = false;
    else if (playing_mod)
        mod_pause = false;
	else if (playing_cd)
		cdrom_cdda_resume();

    thd_sleep(50);
}


void S_PauseMusic(void)
{
	if (music_paused)
		return;

	if (playing_ogg)
		sndoggvorbis_stop();
	else if (playing_opus)
		opusplay_stop();
	else if (playing_midi)
		midi_pause = true;
    else if (playing_mod)
        mod_pause = true;
	else if (playing_cd)
		cdrom_cdda_pause();

	music_paused = true;
    thd_sleep(50);
}


void S_StopMusic(void)
{
	if (playing_ogg)
	{
		playing_ogg = false;
		sndoggvorbis_stop();
		sndoggvorbis_shutdown();
		remove("/ram/temp.ogg");
	}
	else if (playing_opus)
	{
		playing_opus = false;
		opusplay_stop();
		opusplay_shutdown();
		remove("/ram/temp.opus");
	}
	else if (playing_midi)
	{
		midi_done = true;
		memset(pcm_buffer, 0, MIDI_BUF_SIZE);
		playing_midi = false;

	    WildMidi_Close(midi_ptr);
		delete mus_midi_data;

		// cleanup stream
		snd_stream_stop(stream_hnd);
		snd_stream_destroy(stream_hnd);
		stream_hnd = SND_STREAM_INVALID;
	}
    else if (playing_mod)
    {
        mod_done = true;
		memset(pcm_buffer, 0, MIDI_BUF_SIZE);
		playing_mod = false;

        if (mod_smps)
        {
            destroy_sample_buffer(mod_smps);
            mod_smps = NULL;
        }
		duh_end_sigrenderer(mod_renderer);
        unload_duh(mod_file);

		// cleanup stream
		snd_stream_stop(stream_hnd);
		snd_stream_destroy(stream_hnd);
		stream_hnd = SND_STREAM_INVALID;
    }
	else if (playing_cd)
	{
		playing_cd = false;
		cdrom_cdda_pause();
		cdrom_spin_down();
	}

	entry_playing = -1;
	entry_looped  = false;
    thd_sleep(50);
}


void S_MusicTicker(void)
{
	if (playing_ogg || playing_opus || playing_midi || playing_mod)
	{
            thd_pass();
	}
}


void S_ChangeMusicVolume(void)
{
	int vol = (int)((float)mus_volume * (AICA_VOL_HIGH - AICA_VOL_LOW) / (float)(SND_SLIDER_NUM - 1) + AICA_VOL_LOW);
	I_Printf("S_ChangeMusicVolume: %d\n", vol);

	if (playing_ogg)
		sndoggvorbis_volume(vol);
	else if (playing_opus)
		opusplay_set_volume(vol);
	else if (playing_midi)
        snd_stream_volume(stream_hnd, vol);
	else if (playing_mod)
        snd_stream_volume(stream_hnd, vol);
    else if (playing_cd)
		spu_cdda_volume(vol >> 4, vol >> 4);
}


//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab

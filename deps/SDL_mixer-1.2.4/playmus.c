/*
    PLAYMUS:  A test application for the SDL mixer library.
    Copyright (C) 1997, 1998, 1999, 2000, 2001  Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/

/* $Id: playmus.c,v 1.15 2001/12/20 00:01:30 slouken Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#ifdef unix
#include <unistd.h>
#endif

#include "SDL.h"
#include "SDL_mixer.h"


static int audio_open = 0;
static Mix_Music *music = NULL;
static int next_track = 0;

void CleanUp(void)
{
	if( Mix_PlayingMusic() ) {
		Mix_FadeOutMusic(1500);
		SDL_Delay(1500);
	}
	if ( music ) {
		Mix_FreeMusic(music);
		music = NULL;
	}
	if ( audio_open ) {
		Mix_CloseAudio();
		audio_open = 0;
	}
	SDL_Quit();
}

void Usage(char *argv0)
{
	fprintf(stderr, "Usage: %s [-i] |-l] [-8] [-r rate] [-b buffers] [-s] <musicfile>\n", argv0);
}

void Menu(void)
{
	char buf[10];

	printf("Available commands: (p)ause (r)esume (h)alt > ");
	fflush(stdin);
	scanf("%s",buf);
	switch(buf[0]){
	case 'p': case 'P':
		Mix_PauseMusic();
		break;
	case 'r': case 'R':
		Mix_ResumeMusic();
		break;
	case 'h': case 'H':
		Mix_HaltMusic();
		break;
	}
	printf("Music playing: %s Paused: %s\n", Mix_PlayingMusic() ? "yes" : "no", 
		   Mix_PausedMusic() ? "yes" : "no");
}

void IntHandler(int sig)
{
	switch (sig) {
	        case SIGINT:
			next_track++;
			break;
	}
}

int main(int argc, char *argv[])
{
	int audio_rate;
	Uint16 audio_format;
	int audio_channels;
	int audio_buffers;
	int looping = 0;
	int interactive = 0;
	int i;

	/* Initialize variables */
	audio_rate = 22050;
	audio_format = AUDIO_S16;
	audio_channels = 2;
	audio_buffers = 4096;

	/* Check command line usage */
	for ( i=1; argv[i] && (*argv[i] == '-'); ++i ) {
		if ( (strcmp(argv[i], "-r") == 0) && argv[i+1] ) {
			++i;
			audio_rate = atoi(argv[i]);
		} else
		if ( (strcmp(argv[i], "-b") == 0) && argv[i+1] ) {
			++i;
			audio_buffers = atoi(argv[i]);
		} else
		if ( strcmp(argv[i], "-m") == 0 ) {
			audio_channels = 1;
		} else
		if ( strcmp(argv[i], "-l") == 0 ) {
			looping = -1;
		} else
		if ( strcmp(argv[i], "-i") == 0 ) {
			interactive = 1;
		} else
		if ( strcmp(argv[i], "-8") == 0 ) {
			audio_format = AUDIO_U8;
		} else {
			Usage(argv[0]);
			return(1);
		}
	}
	if ( ! argv[i] ) {
		Usage(argv[0]);
		return(1);
	}

	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_AUDIO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		return(255);
	}
	atexit(CleanUp);
	signal(SIGINT, IntHandler);
	signal(SIGTERM, exit);

	/* Open the audio device */
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		return(2);
	} else {
		Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
		printf("Opened audio at %d Hz %d bit %s, %d bytes audio buffer\n", audio_rate,
			(audio_format&0xFF),
			(audio_channels > 1) ? "stereo" : "mono", 
			audio_buffers );
	}
	audio_open = 1;

	/* Set the external music player, if any */
	Mix_SetMusicCMD(getenv("MUSIC_CMD"));

	while (argv[i]) {
		next_track = 0;
		
		/* Load the requested music file */
		music = Mix_LoadMUS(argv[i]);
		if ( music == NULL ) {
			fprintf(stderr, "Couldn't load %s: %s\n",
				argv[i], SDL_GetError());
			return(2);
		}
		
		/* Play and then exit */
		printf("Playing %s\n", argv[i]);
		Mix_FadeInMusic(music,looping,2000);
		while ( !next_track && (Mix_PlayingMusic() || Mix_PausedMusic()) ) {
			if(interactive)
				Menu();
			else
				SDL_Delay(100);
		}
		Mix_FreeMusic(music);
		music = NULL;

		/* If the user presses Ctrl-C more than once, exit. */
		SDL_Delay(500);
		if ( next_track > 1 ) break;
		
		i++;
	}
	return(0);
}

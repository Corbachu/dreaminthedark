################################################################################
## EDGE for KallistiOS (arch: DREAMCAST)
## Copyright (c) 1997-2020 The EDGE Team (see AUTHORS.md)
##
## This software is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This software is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor,
## Boston, MA  02110-1301  USA
##
################################################################################

################################################################################
## KOS requires patching to work with SLaVE/DreamEDGE currently (major TODO)
##
## For new KOS repo via Git, use /KOS_Patches/kos-gcc9
## If on old repo that does not use GCC-9, use /KOS_Patches/kos-gcc9
##
################################################################################

################################################################################
## LIBRARIES REQUiRED for SLaVE
## Find these in /kos-ports/:
## SOUND: libogg, libvorbis, liboggvorbisplay, opus, opusfile, libopusplay, libtremor
## COMPRESSION: libbz2, zlib
##
## OPENGL RENDERER LIBRARY: GLdc (https://gitlab.com/simulant/GLdc)
## This works best when put into /kos/addons/GLdc! 
## DO NOT USE IN-REPO VERSION (libgl__)! 
################################################################################
## DREAMINTHEDARK REQUIRED LIBRARIES:
## SDL, SDL_Mixer (custom port, will contribute to kos-ports shortly), ZLIB, GLdc
##
## todo: Remove SDL completely, port handlers from /system folder (all "dc*.cc")
################################################################################
##
##
## export KOS_CFLAGS="-O2 -DFRAME_POINTERS -fno-omit-frame-pointer"
## CFLAGS is set to -O2 because that is how KOS is defaulted to when building ///!!
##
################################################################################


PROGRAM=inthedark.elf

OBJDIR=obj_dc
LIBDIR=lib_dc

OPTIMISE=-O0


################################################################################
## The compiler and compiler flags
## SLaVE compile with debugging for now.
################################################################################

CC=kos-cc
CXX=kos-c++

KOS_CFLAGS=-O0 -fno-omit-frame-pointer -fno-common -fno-builtin -fexceptions -ffast-math -fno-strict-aliasing -Wall -pipe -ml -m4-single-only -D_GNU_SOURCE=1 -D_REENTRANT -DINLINE_G=__inline__
#-DDATA_PREFIX="/cd/" 
LDFLAGS=-lm -Wl,--warn-common
#Map=output.map

CFLAGS += -I. -Dnullptr=NULL

################################################################################
## Operating System
##
################################################################################

CFLAGS += -DDREAMCAST -DDREAMCAST_DEBUG -DFRAME_POINTERS -D_REENTRANT -DKOS_DEBUG=1 -DUSE_GL=1 -DINTERNAL_DEBUGGER=1

################################################################################
# Line below enables more stuff for Debugging!
#CFLAGS += -DDREAMCAST_DEBUG, -DFRAME_POINTERS
# KOS_DEBUG FLAGS ARE SET IN /include/kos/opts.h (or similar).
################################################################################

################################################################################
# Enable below to debug with GDB!
# don't forget -g, then in gdb do file edge.elf, then target remote:2159 to step thru. I_Main contains gdb_init().
#KOS_CFLAGS+=-DGDB_DEBUG
################################################################################

################################################################################
## Maybe Useful:
## #-ml -m4-single-only -I/opt/toolchains/dc/kos/include -I/opt/toolchains/dc/kos/kernel/arch/dreamcast/include -I/opt/toolchains/dc/kos/addons/include -I/opt/toolchains/dc/kos/../kos-ports/include -D_arch_dreamcast -D_arch_sub_pristine
################################################################################

#KOS_CFLAGS+= -I$(KOS_BASE)/addons/GLdc/include/
KOS_CFLAGS+= -I$(KOS_BASE)/addons/GLdc/include/
KOS_CFLAGS+= -I$(KOS_BASE)/../kos-ports/include/SDL
KOS_CFLAGS+= -I$(KOS_BASE)/../kos-ports/include/zlib
KOS_CFLAGS+= -I$(KOS_BASE)/addons/include/

################################################################################
## EDGE Flags
##
################################################################################

KOS_CFLAGS+=-DTICKER_PRINT


#KOS_CFLAGS+=-DIMMEDIATE_MODE=1
#KOS_CFLAGS+=-DNO_OUTPUT_LOGGING
#KOS_CFLAGS+=-DAICA_SOUND_STUB
#KOS_CFLAGS+=-DPLAY_SND_ON_LOAD
#KOS_CFLAGS+=-DNO_SOUND
#KOS_CFLAGS+=-DNDEBUG
#KOS_CFLAGS+=-DNO_OGG
KOS_CFLAGS+=-DNO_PLAYROQ
KOS_CFLAGS+=-DNO_MIDI
KOS_CFLAGS+=-DNO_LOG
KOS_CFLAGS+=-DNO_FNMATCH
KOS_CFLAGS+=-DNO_NETWORK
#KOS_CFLAGS+=-DNO_LINE_SMOOTH
KOS_CFLAGS+=-DNO_MIPMAPPING
#KOS_CFLAGS+=-DNO_NORMALS
KOS_CFLAGS+=-DNO_EDGEFLAG
#KOS_CFLAGS+=-DNO_COLORMATERIAL
#KOS_CFLAGS+=-DNO_LIGHTING
KOS_CFLAGS+=-DNO_CLIP_PLANES
KOS_CFLAGS+=-DNO_DITHERING
KOS_CFLAGS+=-DFORCE_VERTEX_ARRAY
KOS_CFLAGS+=-DALLOC_GRANULARITY=16
KOS_CFLAGS+=-DNO_MEMORY_FUNCS
KOS_CFLAGS+=-DMEM_CACHE
KOS_CFLAGS+=-DUSE_MATH_INLINE
KOS_CFLAGS+=-DLOW_DETAIL
KOS_CFLAGS+=-DAUTO_FLUSH_VRAM
KOS_CFLAGS+=-DAUTO_FLUSH_LUMP
KOS_CFLAGS+=-DUSE_NATIVE_TEXTURES
KOS_CFLAGS+=-DNO_ALPHA_TEST
KOS_CFLAGS+=-DFRAME_POINTERS

################################################################################
## DEBUG FLAGS
##
################################################################################
#KOS_CFLAGS+=-DSHOW_SCREEN
#KOS_CFLAGS+=-DDEVELOPERS
#KOS_CFLAGS+=-DDEBUG
#KOS_CFLAGS+=-DWAD_CHECK
#KOS_CFLAGS+=-DDEBUGDDFREAD
#KOS_CFLAGS+=-DDEBUG_MOBJ

#KOS_CFLAGS+=-DMAKE_SLAVE_RELEASE_ISO
#KOS_CFLAGS+=-DTRIM_SLAVE_RELEASE_DBG
#KOS_CFLAGS+=-DGOAT_SLAVE_RELEASE_DEMO

# We need Intolerant Math for Z_Malloc stuff
# // WHY?!?! X87 Signaling Error?? Does that happen on SH-4?
KOS_CFLAGS+=-DINTOLERANT_MATH


KOS_CFLAGS+=-DDEBUG_DREAMEDGE




################################################################################
## PROFILE FLAGS
##
#KOS_CFLAGS+=-DSWAPBUFFER_THREATED
#KOS_CFLAGS+=-DPROFILER_DREAMEDGE
#KOS_CFLAGS+=-DRESET_PROFILER=150
#KOS_CFLAGS+=-DMEM_STATS
#KOS_CFLAGS+=-DMEM_STATS_VERBOSE

#KOS_CFLAGS+=-DAUTO_LOADGAME
#KOS_CFLAGS+=-DAUTO_EVENT_END=350
################################################################################

CFLAGS += $(KOS_CFLAGS)

################################################################################
## LIBRARY FLAGS
## ditd: CONVERT SYSTEM FROM SDL_mixer, REMOVE GLU TESSELATOR.
################################################################################

# SDL
LDFLAGS += -lSDL
#LDFLAGS += -lSDL_mixer_1212
#LDFLAGS += ../SDL_mixer-1.2.4/
#/libSDL_mixer.a

# OpenGL rendering

#LDFLAGS += -lGL
LDFLAGS += -lGLdc
#LDFLAGS += libgl/libGLdc.a

# PNG, JPEG, ZLIB, and KMG (for Dreamcast KMG textures)

#LDFLAGS += -lkmg -lpng -ljpeg -lkmg

# PHYSFS

#LDFLAGS += -lphysfs

# OGG/Vorbis
# we ommited -ltremor sometime ago
#LDFLAGS += -lopusplay -lopusfile -lopus
#LDFLAGS += -loggvorbisplay -lvorbis -logg

# WildMidi

#LDFLAGS += -lWildMidi -L libWildMidi

# libdumb

#LDFLAGS += -ldumb -Ldumb

# ZLIB

LDFLAGS += -lz

# ------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------

# Targets

all: makedirs $(PROGRAM)

stripped: $(PROGRAM)
	kos-strip --strip-unneeded $(PROGRAM)

clean:
	rm -f $(PROGRAM) $(OBJDIR)/*/*.*
	rm -f $(PROGRAM) $(OBJDIR)/*/*/*.*
#	rm -f libgl/libGLdc.a libgl/GL/*.o

halfclean:
	rm -f $(PROGRAM) $(OBJDIR)/epi/*.* $(OBJDIR)/ditd/*.*

makedirs:
	mkdir -p $(OBJDIR)/ditd
	mkdir -p $(OBJDIR)/ditd/system
	mkdir -p $(OBJDIR)/epi

#updaterev:
#	gcc updaterevision/updaterevision.c -o ./updaterev
#	./updaterev ./src/gitinfo.h

.PHONY: all stripped clean halfclean makedirs

#----------------------------------------
#----------------------------------------
#----------------------------------------

TARGET = dreamdark.bin
FIRSTREAD = 1st_read.bin

#//!!!  Disable the KOS_STRIP if debugging!! #
$(KOS_STRIP): --strip-unneeded $(PROGRAM)
$(TARGET): $(PROGRAM) 
	$(KOS_OBJCOPY) -R .stack -O binary $(PROGRAM) $(TARGET)

# Create scrambled binary
$(FIRSTREAD): $(TARGET)
	$(KOS_BASE)/utils/scramble/scramble $(TARGET) $(FIRSTREAD)

# set this to your CDVD writer
CDRECORD_ID = /dev/cdrom

# Burn a bootable CD of EDGE/DOOM
selfboot: $(FIRSTREAD)
	rm -f disc/session?.iso
	genisoimage -G disc/ip.bin -l -J -R -o disc/session1.iso disc/data $(FIRSTREAD)
	dd if=disc/session1.iso bs=2048 count=17 of=disc/session2.iso
	dd if=/dev/zero bs=2048 count=283 >> disc/session2.iso
	cdrecord dev=$(CDRECORD_ID) -multi -xa -tao disc/session1.iso
	cdrecord dev=$(CDRECORD_ID) -eject -xa -tao disc/session2.iso
	rm -f $(TARGET) $(FIRSTREAD)

# Burn a bootable CD of SLAVE
slaveboot: $(FIRSTREAD)
	rm -f disc/session?.iso
	genisoimage -G slave/ip.bin -l -J -R -o slave/session1.iso slave/data $(FIRSTREAD)
	dd if=slave/session1.iso bs=2048 count=17 of=slave/session2.iso
	dd if=/dev/zero bs=2048 count=283 >> slave/session2.iso
	cdrecord dev=$(CDRECORD_ID) -multi -xa -tao slave/session1.iso
	cdrecord dev=$(CDRECORD_ID) -eject -xa -tao slave/session2.iso
	rm -f $(TARGET) $(FIRSTREAD)

# Win32 Reminder: make -f makefile.dc selfbootWin32' to generate bootable image - burn with IMGBURN
# Burn a bootable CD
selfbootWin32: $(FIRSTREAD)
	rm -f slave/session?.iso
	genisoimage -G slave/ip.bin -l -J -R -o slave/session1.iso slave/data $(FIRSTREAD)
	dd if=slave/session1.iso bs=2048 count=17 of=slave/session2.iso
	dd if=/dev/zero bs=2048 count=283 >> slave/session2.iso
	#cdrecord dev=$(CDRECORD_ID) -multi -xa -tao disc/session1.iso
	#cdrecord dev=$(CDRECORD_ID) -eject -xa -tao disc/session2.iso
	rm -f $(TARGET) $(FIRSTREAD)

# Make a DreamShell SDISO and/or burn as a regular data disc (for debugging or DreamShell)
sdiso: $(TARGET)
	rm -f slavedata.iso
	cp $(TARGET) $(FIRSTREAD)
	genisoimage -V DC_APP -G slave/SDIP.BIN -l -J -R -o slavedata.iso slave/data $(FIRSTREAD)
	rm $(TARGET) $(FIRSTREAD)

# Make a SLAVE ISO and burn as data disc (for debugging or for DreamShell)
DREAMINTHEDARK: $(TARGET)
	rm -f inthedark.iso
	cp $(TARGET) $(FIRSTREAD)
	mkisofs -V DC_APP -G ALONE/SDIP.BIN -l -J -R -o inthedark.iso ALONE/data $(FIRSTREAD)
	rm $(TARGET) $(FIRSTREAD)

# Make total REBUILD, this isn't currently working, so obvi I need to brush up on Makefile skills.
# It's supposed to incorporate 'clean', 'all' (updaterev and main), and 'slaveiso' all-in-one.
#rebuild: $(TARGET)
#    rm -f $(PROGRAM) $(OBJDIR)/*/*.*
#	rm -f $(PROGRAM) $(OBJDIR)/*/*/*.*
#	makedirs updaterev $(PROGRAM)
#	rm -f slave.iso
#	cp $(TARGET) $(FIRSTREAD)
#	genisoimage -V DC_APP -G slave/SDIP.BIN -l -J -R -o slave.iso slave/data $(FIRSTREAD)
#	rm $(TARGET) $(FIRSTREAD)


# ---------- EDGE ---------------

DITD_OBJS= \
	$(OBJDIR)/ditd/actorList.o \
    $(OBJDIR)/ditd/aitdBox.o \
    $(OBJDIR)/ditd/anim.o \
    $(OBJDIR)/ditd/animAction.o \
    $(OBJDIR)/ditd/bmp.o \
    $(OBJDIR)/ditd/cosTable.o \
    $(OBJDIR)/ditd/debugFont.o \
    $(OBJDIR)/ditd/debugger.o \
    $(OBJDIR)/ditd/evalVar.o \
    $(OBJDIR)/ditd/fileAccess.o \
    $(OBJDIR)/ditd/floor.o \
    $(OBJDIR)/ditd/fmopl.o \
    $(OBJDIR)/ditd/font.o \
    $(OBJDIR)/ditd/hqr.o \
    $(OBJDIR)/ditd/input.o \
    $(OBJDIR)/ditd/inventory.o \
    $(OBJDIR)/ditd/life.o \
    $(OBJDIR)/ditd/life_2.o \
    $(OBJDIR)/ditd/lifeMacroTable.o \
    $(OBJDIR)/ditd/lines.o \
    $(OBJDIR)/ditd/main.o \
    $(OBJDIR)/ditd/mainLoop.o \
    $(OBJDIR)/ditd/music.o \
    $(OBJDIR)/ditd/object.o \
    $(OBJDIR)/ditd/osystemSDL.o \
    $(OBJDIR)/ditd/osystemSDL_GL.o \
    $(OBJDIR)/ditd/pak.o \
    $(OBJDIR)/ditd/polys.o \
    $(OBJDIR)/ditd/renderer.o \
    $(OBJDIR)/ditd/room.o \
    $(OBJDIR)/ditd/save.o \
    $(OBJDIR)/ditd/screen.o \
    $(OBJDIR)/ditd/sequence.o \
    $(OBJDIR)/ditd/startupMenu.o \
    $(OBJDIR)/ditd/systemMenu.o \
    $(OBJDIR)/ditd/tatou.o \
    $(OBJDIR)/ditd/threadCode.o \
    $(OBJDIR)/ditd/freezetime.o \
    $(OBJDIR)/ditd/track.o \
    $(OBJDIR)/ditd/unpack.o \
    $(OBJDIR)/ditd/vars.o \
    $(OBJDIR)/ditd/version.o \
    $(OBJDIR)/ditd/videoMode.o \
	$(OBJDIR)/ditd/zv.o 

#$(OBJDIR)/indark/%.o: src/%.cc
#	$(CXX) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/ditd/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<


# ---------- EPI ---------------

# EPI_OBJS= \
	# $(OBJDIR)/epi/arrays.o         \
	# $(OBJDIR)/epi/bytearray.o      \
	# $(OBJDIR)/epi/errors.o         \
	# $(OBJDIR)/epi/exe_path.o       \
	# $(OBJDIR)/epi/file.o           \
	# $(OBJDIR)/epi/filesystem.o     \
	# $(OBJDIR)/epi/file_memory.o    \
	# $(OBJDIR)/epi/file_sub.o       \
	# $(OBJDIR)/epi/stb_image.o      \
	# $(OBJDIR)/epi/stb_image_write.o \
	# $(OBJDIR)/epi/image_data.o     \
	# $(OBJDIR)/epi/image_jpeg.o     \
	# $(OBJDIR)/epi/image_png.o      \
	# $(OBJDIR)/epi/image_tga.o      \
	# $(OBJDIR)/epi/math_angle.o     \
	# $(OBJDIR)/epi/math_bbox.o      \
	# $(OBJDIR)/epi/math_color.o     \
	# $(OBJDIR)/epi/math_crc.o       \
	# $(OBJDIR)/epi/math_md5.o       \
	# $(OBJDIR)/epi/math_oddity.o    \
	# $(OBJDIR)/epi/math_matrix.o    \
	# $(OBJDIR)/epi/math_vector.o    \
	# $(OBJDIR)/epi/math_random.o    \
	# $(OBJDIR)/epi/memmanager.o     \
	# $(OBJDIR)/epi/mersenne_twist.o \
	# $(OBJDIR)/epi/path.o           \
	# $(OBJDIR)/epi/str_format.o     \
	# $(OBJDIR)/epi/sound_data.o     \
	# $(OBJDIR)/epi/sound_gather.o   \
	# $(OBJDIR)/epi/sound_wav.o      \
	# $(OBJDIR)/epi/timestamp.o      \
	# $(OBJDIR)/epi/utility.o        \
	# $(OBJDIR)/epi/epi_dreamcast.o  \
	# $(OBJDIR)/epi/filesystem_linux.o

# $(OBJDIR)/epi/%.o: epi/%.cc
	# $(CXX) $(CFLAGS) -o $@ -c $<

# ---------- FINAL LINK STEP -----------

$(PROGRAM) :$(DITD_OBJS) 
			#$(EPI_OBJS)	    
	$(CXX) $(CFLAGS) -o $@  $^ $(KOS_START) $(LDFLAGS) -lm -lstdc++ $(KOS_LIBS)


#--- editor settings ------------
# vi:ts=8:sw=8:noexpandtab

# ---------- GLdc ---------------

#libgl/libGLdc.a :
#	@$(MAKE) -C libgl CFLAGS=-DBUILD_LIBGL build
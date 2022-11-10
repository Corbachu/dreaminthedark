//----------------------------------------------------------------------------
//  EDGE File Class for Linux
//----------------------------------------------------------------------------
//
//  Copyright (c) 2003-2008  The EDGE Team.
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

#include "epi.h"

#include "file.h"

namespace epi
{

ansi_file_c::ansi_file_c(FILE *_filep) : fp(_filep)
{ }

ansi_file_c::~ansi_file_c()
{
    if (fp)
	{
#ifndef DREAMCAST
		fclose(fp);
#else
		fs_close((file_t)fp-1);
#endif
		fp = NULL;
    }
}

int ansi_file_c::GetLength()
{
	SYS_ASSERT(fp);

#ifndef DREAMCAST
    long cur_pos = ftell(fp);      // Get existing position

    fseek(fp, 0, SEEK_END);        // Seek to the end of file
    long len = ftell(fp);          // Get the position - it our length

    fseek(fp, cur_pos, SEEK_SET);  // Reset existing position
#else
    long cur_pos = fs_tell((file_t)fp-1);      // Get existing position

    fs_seek((file_t)fp-1, 0, SEEK_END);        // Seek to the end of file
    long len = fs_tell((file_t)fp-1);          // Get the position - it our length

    fs_seek((file_t)fp-1, cur_pos, SEEK_SET);  // Reset existing position
#endif
    return (int)len;
}

int ansi_file_c::GetPosition()
{
	SYS_ASSERT(fp);

#ifndef DREAMCAST
    return (int)ftell(fp);
#else
	return (int)fs_tell((file_t)fp-1);
#endif
}

unsigned int ansi_file_c::Read(void *dest, unsigned int size)
{
	SYS_ASSERT(fp);
	SYS_ASSERT(dest);

#ifndef DREAMCAST
    int result = fread(dest, size, 1, fp) * size;
#else
	int result = fs_read((file_t)fp-1, dest, size);
#endif

//I_Debugf("Reading %d bytes  --->  %d  %s %s\n", size, result,
//		 feof(fp) ? "EOF" : "-", ferror(fp) ? "ERROR" : "-");

	return result;
}

unsigned int ansi_file_c::Write(const void *src, unsigned int size)
{
	SYS_ASSERT(fp);
	SYS_ASSERT(src);

#ifndef DREAMCAST
    return fwrite(src, 1, size, fp) * size;
#else
	return fs_write((file_t)fp-1, src, size);
#endif
}

bool ansi_file_c::Seek(int offset, int seekpoint)
{
    int whence;

    switch (seekpoint)
    {
        case SEEKPOINT_START:   { whence = SEEK_SET; break; }
        case SEEKPOINT_CURRENT: { whence = SEEK_CUR; break; }
        case SEEKPOINT_END:     { whence = SEEK_END; break; }

        default:
			I_Error("ansi_file_c::Seek : illegal seekpoint value.\n");
            return false; /* NOT REACHED */
    }

#ifndef DREAMCAST
    int result = fseek(fp, offset, whence);
#else
	int result = fs_seek((file_t)fp-1, offset, whence);
#endif

//    long curr_pos = ftell(fp);

//    I_Debugf("Seek to: 0x%08x whence:%d  --->  0x%08x %d\n", offset, whence, curr_pos, result);

	return (result == 0);
}

byte *file_c::LoadIntoMemory(int max_size)
{
	SYS_ASSERT(max_size >= 0);

	int cur_pos     = GetPosition();
	int actual_size = GetLength();

	actual_size -= cur_pos;

	if (actual_size < 0)
	{
		I_Warning("file_c::LoadIntoMemory : position > length.\n");
		actual_size = 0;
	}

	if (actual_size > max_size)
		actual_size = max_size;

	byte *buffer = new byte[actual_size + 1];
	buffer[actual_size] = 0;

	if ((int)Read(buffer, actual_size) != actual_size)
	{
		delete[] buffer;
		return NULL;
	}

	return buffer;  // success!
}


// utility functions...

bool FS_FlagsToAnsiMode(int flags, char *mode)
{
    // Must have some value in epiflags
    if (flags == 0)
        return false;

    // Check for any invalid combinations
    if ((flags & file_c::ACCESS_WRITE) && (flags & file_c::ACCESS_APPEND))
        return false;

    if (flags & file_c::ACCESS_READ)
    {
        if (flags & file_c::ACCESS_WRITE)
            strcpy(mode, "wb+");                        // Read/Write
        else if (flags & file_c::ACCESS_APPEND)
            strcpy(mode, "ab+");                        // Read/Append
        else
            strcpy(mode, "rb");                         // Read
    }
    else
    {
        if (flags & file_c::ACCESS_WRITE)
            strcpy(mode, "wb");                         // Write
        else if (flags & file_c::ACCESS_APPEND)
            strcpy(mode, "ab");                         // Append
        else
            return false;                              // Invalid
    }

    return true;
}

} // namespace epi

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab

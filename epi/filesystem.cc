//----------------------------------------------------------------------------
//  EDGE Filesystem Class
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
#include "filesystem.h"

#define MAX_MODE_CHARS  32

namespace epi
{

// A Filesystem Directory

filesystem_dir_c::filesystem_dir_c() : array_c(sizeof(filesys_direntry_c))
{ }

filesystem_dir_c::~filesystem_dir_c()
{ }

bool filesystem_dir_c::AddEntry(filesys_direntry_c *fs_entry)
{
	if (InsertObject(fs_entry) < 0)
        return false;

	return true;
}

void filesystem_dir_c::CleanupObject(void *obj)
{ }

filesys_direntry_c *filesystem_dir_c::operator[](int idx)
{
	return (filesys_direntry_c*)FetchObject(idx);
}


//----------------------------------------------------------------------------

// common functions

bool FS_Access(const char *name, unsigned int flags)
{
	SYS_ASSERT(name);

    char mode[MAX_MODE_CHARS];

    if (! FS_FlagsToAnsiMode(flags, mode))
        return false;

#ifndef DREAMCAST
    FILE *fp = fopen(name, mode);
    if (!fp)
        return false;
    fclose(fp);
    return true;
#else
    file_t fp = fs_open(name, O_RDONLY);
    if (fp < 0)
        return false;
    fs_close(fp);
    return true;
#endif
}

file_c* FS_Open(const char *name, unsigned int flags)
{
	SYS_ASSERT(name);

    char mode[MAX_MODE_CHARS];

    if (! FS_FlagsToAnsiMode(flags, mode))
        return NULL;

#ifndef DREAMCAST
    FILE *fp = fopen(name, mode);
    if (!fp)
        return NULL;
#else
    file_t fh = fs_open(name, flags & file_c::ACCESS_WRITE ? O_WRONLY : O_RDONLY);
    if (fh < 0)
        return NULL;
    FILE *fp = (FILE *)(fh + 1);
#endif

	return new ansi_file_c(fp);
}

} // namespace epi

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab

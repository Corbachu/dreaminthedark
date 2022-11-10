//----------------------------------------------------------------------------
//  Dreamcast EPI System Specifics
//----------------------------------------------------------------------------
//
//  Copyright (c) 2002-2008  The EDGE Team.
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
#include "memmanager.h"
#include "epi.h"

namespace epi
{
	//filesystem_c *the_filesystem = NULL;
	mem_manager_c *the_mem_manager = NULL;

	const unsigned int max_mem_size = 1048576; // 32Mb 
	
	static bool inited = false;

    //
    // Init
    //
    bool Init(void)
    {
		bool ok;
		mem_manager_c* mm;
		//all_filesystem_c* fs;

		if (inited)
			Shutdown();

		ok = true;
		
		if (ok)
		{
			mm = new mem_manager_c(max_mem_size);
			if (mm)
				the_mem_manager = mm;
			else
				ok = false;
		}

		inited = ok;
        return ok;
    }

    //
    // Shutdown
    //
    void Shutdown(void)
    {
		if (the_mem_manager)
		{
			delete the_mem_manager;
			the_mem_manager = NULL;
		}
		
		inited = false;
    }

} // namespace epi


/*void strupr(char *str)
{
	if (str)
		for (; *str; str++)
			*str = toupper(*str);
}*/

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab

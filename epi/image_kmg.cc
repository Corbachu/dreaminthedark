//------------------------------------------------------------------------
//  KMG Image Handling
//------------------------------------------------------------------------
//
//  Copyright (c) 2018  The EDGE Team.
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
//------------------------------------------------------------------------
//
// This is wrapped around the epi namespace, but uses the PVR api in KOS!
//


#include "epi.h"

// Technically, we don't need this since this is directly referencing the API
// from KOS, but we can bypass that if I write the header information directly
// into this file, and lace it through image_data_c so we would let EPI handle
// it, but right now we need to see if this even works as-is.
//#include "image_kmg.h"

#include <kos/img.h>
#include <kmg/kmg.h>
#include <dc/pvr.h>


namespace epi
{

// Should we lace it through image_data_c? 
//image_data_c *KMG_load(file_c *f, u32_t* w, u32_t* h, u32_t* format) 
pvr_ptr_t load_kmg(char const* filename, uint32* w, uint32* h, uint32* format)
{
       kos_img_t img;
       pvr_ptr_t rv;

       if(kmg_to_img(filename, &img)) 
       {
               I_Printf("Failed to load image file: %s\n", filename);
               return NULL;
       }

       if(!(rv = pvr_mem_malloc(img.byte_count))) 
       {
               I_Printf("Couldn't allocate memory for texture!\n");
               kos_img_free(&img, 0);
               return NULL;
       }

       pvr_txr_load_kimg(&img, rv, 0);

       kos_img_free(&img, 0);

       *w = img.w;

       *h = img.h;

       *format = img.fmt;

       return rv;
}

} // namespace epi

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab

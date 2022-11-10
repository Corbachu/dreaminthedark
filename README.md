## Dream In the Dark
---
[![DITD.png](https://i.postimg.cc/gkWNhsjS/DITD.png)](https://postimg.cc/K40PSnnB)

#### DITD ("Dream in the Dark") is an open source version of the engine used in the Alone in the Dark (AITD) and its sequels. 

### It is based off of fitd ("Free in the Dark") and uses the Enhanced DOOM Gaming Engine ("EDGE") as its backend (EITD, or ""EDGE In the Dark").


## (C) 1999-2022 The EDGE Team & [Contributors](https://github.com/3dfxdev/hyper3DGE/blob/master/AUTHORS.md)
## (c) FITD Authors/Team ("Free in the Dark") (see Contributors above)
## (C) JMIMU (original basis for this port): https://github.com/jmimu/FITD
##### Uses GPL-licensed technology from id Tech 1-4 (C) 1997-2011 id Software, LLC
##### DOSDoom originally developed by Chi Hoang and the DOSDoom Team, (C) 1997-1999
#### [Licensed under the GPLv2 or greater](https://github.com/Corbachu/EITD/blob/master/AUTHORS.md)
#### [See our EDGE Wiki](http://3dfxdev.net/edgewiki/index.php/Main_Page) - TODO MAKE EITD WIKI

### *special notice:
----------------------------

This is a massive hack-job/WIP until I get things more under control!
- Ported from QMAKE to MAKE (original file fitd.pro)
- Help Wanted for debugging, my time is VERY limited! 
- SEE ALL NOTES BELOW

## Build System for DITD
---
To build DITD, build with "Make -f Makefile.dc" to produce the ELF binary under KOS.
- At the command line, typing in "Make DREAMINTHEDARK" will produce a disc image and ELF that can be used for testing.
- You still need the .PAK files from the registered version of Alone in the Dark (which you should put into /root/ALONE/data)
#### Libraries
---
- SDL 1 (NEED to remove this)
- SDL1_mixer (Currently disabled, scheduled for removal)
- Zlib
- GLdc 

---
## FAQ

### What is Dream in the Dark?
------------

Years ago, this started as "Dream in the Dark" (or "DITD"), a port of fitd to SH-4/DC. This is a crummy patch up of the code that existed years ago (the YouTube video I had posted at that time), but most if not all of it was completely broken as KOS has evolved greatly since then. 

EDIT: Found the original 0.1 video of Dream in the Dark (https://www.youtube.com/watch?v=sE_7FkT0aDM)


### What Is Supported? (Notes from original fitd)
---
- AITD1:
    Should be completable. Saving and loading is supported,
    but only one slot is available (the screen to select the
    save position is not implemented). It's possible to load
    a savegame from the original game, provided it's file is
    named "save0.itd" (rename it if necessary). Like wise,
    it is possible to load a FITD save in the original engine.
    There is a big issue concerning the projectils. Basically,
    guns, won't work, and throwing objects may produce strange
    result. Save before throwing anything, as the object may
    appear in a wall...

- AITD2:
      Supported but uncompletable. The introduction is buggy
      due to a regression in the script engine. Hit 'esc' to skip
      the intro (both parts) to bypass the bug .Save/load is
      unsupported.
- JACK:
      Supported but uncompletable.
      Save/load is unsupported.

- AITD3:
      Supported but uncompletable.
      Save/load is unsupported.

# The EDGE Team
---
## Team and Contributors

EDGE has and will continue to exist thanks to all the people who contribute:
- Corbin "Corbachu" Annis
	  Programmer, SH4/CC ports

- hogz
	  Maintainer of his own fork "hfitd", which I've incorporated some parts of.

- somaen
	  Initial FITD-risidualVM stuff

- tigrouind
	  AITD Room Viewer

- Vincent Hamm "yaz0r" (yazoo@yaz0r.net)
      Project leader and main programmer

- Nicolas Noble "Pixel"
      Cross platform issues. Helped understanding some
      complex aspect of the 3d engine
	  
- Andrew Apted
	  Authored EDGE, EPI, and the coding style we use here. :P


## Special Thanks
---
- The FITD Team for Free in the Dark.
- The EDGE Team, and the EDGE Engine from which we use significant pieces of here.
- The ridisualvm team for initial backends/support

### Original "Greetings" portion from FITD:
- All the Alone in the Dark 1 team:
      Frederick Raynal
      Yael Barroz
      Didier Chanfray
      Philippe Vachey
      Franck De Girolami
      Jean-Marc Torroella
      Hubert Chardot
      Franck Manzetti

- Laurent Paret, you know what for ;)

- Sebastien Viannay, maybe this project wouldn't
      have existed without you...

- All those I forgot....




## Development History
---
Read the historical timeline of EDGE at [The Doom Wiki](https://doomwiki.org/wiki/EDGE), or check out the /docs/logs/ folder.

---

# Support
* Visit the [EDGE forums](http://tdgmods.net/smf) and get involved with the community and the various projects for the engine.
* The [EDGEWiki](http://3dfxdev.net/edgewiki) is also a great resource for editing documentation and other information related to the engine.

(C) 1999 - 2021 [Isotope SoftWorks](https://www.facebook.com/IsotopeSoftWorks/) and Contributors (The EDGE Team). All Rights Reserved.




Original Readme from yaz0r
==========================

What is FITD
------------

FITD ("Free in the Dark") is an open source vesion of the
engine used in the Alone in the Dark (AITD) and sequels.
AITD_PakEdit allows to modify PAK file for AITD and FITD.

License
-------

FITD is distributed under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version. A copy of this license 
an be found in the file COPYING included with the source code of this
program.


*NIX version special notice:
----------------------------

This version had music diseabled at the last second. The music is still
enabled in the win32 build. If someone is interested to look for the
exact issue under *nix, please drop me a mail.

What is supported
-----------------

AITD1:
      Should be completable. Saving and loading is supported,
      but only one slot is available (the screen to select the
      save position is not implemented). It's possible to load
      a savegame from the original game, provided it's file is
      named "save0.itd" (rename it if necessary). Like wise,
      it is possible to load a FITD save in the original engine.
      There is a big issue concerning the projectils. Basically,
      guns, won't work, and throwing objects may produce strange
      result. Save before throwing anything, as the object may
      apprear in a wall...

AITD2:
      Supported but uncompletable. The introduction is buggy
      due to a regression in the script engine. Hit 'esc' to skip
      the intro (both parts) to bypass the bug .Save/load is
      unsupported.

JACK:
      Supported but uncompletable.
      Save/load is unsupported.

AITD3:
      Supported but uncompletable.
      Save/load is unsupported.

How to use FITD
---------------

The depences are : qmake, sdl, sdl-mixer, glu, zlib
Build (with qmake and make) and put the binary in the game
data directory (where all the .pak files are) and start FITD.
The CD version of AITD1 wasn't tested and may not work.

Version 0.1 Authors
-------------------

- Vincent Hamm "yaz0r" (yazoo@yaz0r.net)
      Project leader and main programmer

- Nicolas Noble "Pixel"
      Cross platform issues. Helped understanding some
      complex aspect of the 3d engine


Greetings
---------

- All the Alone in the Dark 1 team:
      Frederick Raynal
      Yael Barroz
      Didier Chanfray
      Philippe Vachey
      Franck De Girolami
      Jean-Marc Torroella
      Hubert Chardot
      Franck Manzetti

- Laurent Paret, you know what for ;)

- Sebastien Viannay, maybe this project wouldn't
      have existed without you...

- All those I forgot....


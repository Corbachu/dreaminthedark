@echo off

set gamename=dreaminthedark

@echo:===================================================
@echo:= [ ++           OVERRiDE 09	      ++ ] =
@echo:= [ ++ 3DGE (C) Isotope SoftWorks, 2015 ++ ] =
@echo:= [ ++   Josh Pearson / Coraline Annis  ++ ] =
@echo:===================================================

mkisofs -R -C 0,11702 -V %gamename% -G ALONE/ip.bin -l -J -hide 0.0 -o inthedark.iso disc/data 1st_read.bin

cdi4dc inthedark.iso %gamename%.CDi

@echo:===================================================
@echo:= [ ++   3DGE CDI generation completed!   ++ ] =
@echo:===================================================
@echo:===================================================
@echo:= [ ++        Do Not Distrubute!          ++ ] =
@echo:===================================================

@echo off

set CXXFLAGS=/std:c++17 /utf-8 /EHsc /Zi 
set INCLUDES=/I .
set LIBS=

rc.exe src/ressource.rc
cl.exe /MP /LD %CXXFLAGS% %INCLUDES% src/dllComponentMain.cpp src/ConvertProgressDialog.cpp src/ImageConvert.cpp src/REGISTRY.CPP src/dllComponentMain.def /link src/ressource.res /out:RAWtoJPG.dll


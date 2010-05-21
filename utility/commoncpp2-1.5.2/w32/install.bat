@echo off
rem bootstrap framework when no pre-packaged setup.exe is available
rem we use a default configuration...

echo Boostrapping Framework from Released Libraries

if exist "C:\Program Files\GNU Telephony" goto common

echo Creating C:\Program Files\GNU Telephony...

mkdir "C:\Program Files\GNU Telephony"
mkdir "C:\Program Files\GNU Telephony\CAPE Framework"
mkdir "C:\Program Files\GNU Telephony\CAPE Framework\Include"
mkdir "C:\Program Files\GNU Telephony\CAPE Framework\Include\cc++"
mkdir "C:\Program Files\GNU Telephony\CAPE Framework\Import"
mkdir "C:\Program Files\GNU Telephony\CAPE Framework .NET"
mkdir "C:\Program Files\GNU Telephony\CAPE Framework .NET\Include"
mkdir "C:\Program Files\GNU Telephony\CAPE Framework .NET\Include\cc++"
mkdir "C:\Program Files\GNU Telephony\CAPE Framework .NET\Import"

:common
if exist "C:\Program Files\Common Files\CAPE Framework" goto include

echo Creating C:\Program Files\Common Files\CAPE Framework...

mkdir "C:\Program Files\Common Files\CAPE Framework"
mkdir "C:\Program Files\Common Files\CAPE Framework\Runtime"
mkdir "C:\Program Files\Common Files\CAPE Framework\Audio Codecs"
mkdir "C:\Program Files\Common Files\CAPE Framework .NET"
mkdir "C:\Program Files\Common Files\CAPE Framework .NET\Runtime"
mkdir "C:\Program Files\Common Files\CAPE Framework .NET\Audio Codecs"

:include

echo Copying include files...

copy /y ..\include\cc++\*.h "C:\Program Files\GNU Telephony\CAPE Framework\Include\cc++" >nul
copy /y cc++\config.h "C:\Program Files\GNU Telephony\CAPE Framework\Include\cc++" >nul
copy /y ..\src\getopt.h "C:\Program Files\GNU Telephony\CAPE Framework\Include" >nul

copy /y ..\include\cc++\*.h "C:\Program Files\GNU Telephony\CAPE Framework .NET\Include\cc++" >nul
copy /y cc++\config.h "C:\Program Files\GNU Telephony\CAPE Framework .NET\Include\cc++" >nul
copy /y ..\src\getopt.h "C:\Program Files\GNU Telephony\CAPE Framework .NET\Include" >nul

if not exist release\ccext2.lib goto  import

echo Copying compat import libs...
copy /y release\cc*.lib "C:\Program Files\GNU Telephony\CAPE Framework\Import" >nul
:import

if not exist "Release .NET\ccext2.lib" goto compat

echo Copying common import libs...

copy /y "Release .NET\cc*.lib" "C:\Program Files\GNU Telephony\CAPE Framework .NET\Import" >nul

:compat

if not exist release\ccext2.dll goto runtime

echo Copying compat runtime files...

copy /y release\cc*.dll "C:\Program Files\Common Files\CAPE Framework\Runtime" >nul

:runtime

if not exist "Release .NET\ccext2.dll" goto finish

echo Copying common runtime files...

copy /y "Release .NET\cc*.dll" "C:\Program Files\Common Files\CAPE Framework .NET\Runtime" >nul

:finish

echo Updating registry...

regedit /s common.reg


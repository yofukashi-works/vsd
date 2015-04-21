@echo off
: .tab=4

set path=%path%;c:\cygwin\bin

xcopy /s/i release_files zrelease
pushd zrelease

copy "D:\Program Files\AVIUTL\Plugins\vsd_filter.auf" vsd_filter_gps.auf

mkdir vsd_plugins
xcopy /i/s "D:\Program Files\AVIUTL\Plugins\vsd_plugins" vsd_plugins

rmdir /s/q vsd_plugins\gallery
del vsd_plugins\_log_reader\vsd_log.js

: API �L�[�폜
for %%a in ( vsd_plugins/google_maps.js vsd_plugins/streetview.js ) do (
	perl -pe 's@.*//#DEL#.*\n@@; s@//#REL#@\t@' ../%%a > %%a
)

: d:\dds\bin\lha a -d ..\vsd_filter_gps.lzh *
c:\cygwin\bin\zip -r ../vsd_filter_gps.zip *

popd

rmdir /s/q zrelease

perl -ne 'rename( "vsd_filter_gps.zip", "vsd_filter_gps_r$1.zip" ) if( /#define\s+PROG_REVISION\s+(\d+)/ );' rev_num.h

pause

:@echo off

pushd ..

set path=c:\cygwin64\bin;C:\strawberry\c\bin;C:\strawberry\perl\site\bin;C:\strawberry\perl\bin;%PATH%

if exist c:\cygwin64\bin\ (
	echo cygwin64 detected
	
	c:\cygwin64\bin\make.exe
) else (
	echo cygwin64 not detected
	
	perl make/make_js_func.pl ScriptIF.h
	perl make/set_git_version.pl rev_num.h
)

popd

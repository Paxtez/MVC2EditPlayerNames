@echo off
SET PATH=%PATH%;C:\MinGW\bin

set cppfile="EditNames.cpp"
set exefile="EditNames.exe"



:Compile
IF EXIST %exefile% (
	echo --- DETECTED %exefile%... DELETING FILE ---
	del %exefile% >nul 2>nul
)

IF EXIST %exefile% (
	echo --- UNABLE TO DELETE ---
	echo --- CLOSE CURRENT %exefile% WINDOWS ---
	pause
	exit
)

echo --- BUILDING ---
"g++.exe" %cppfile% -g -Wall -static-libgcc -static-libstdc++ -o %exefile%

IF EXIST %exefile% (
	"strip.exe" --strip-all %exefile%
	echo --- RUNNING ---
	pause
	%exefile%
	) else (
		echo --- FAILED TO COMPILE ---
		pause
	)

pause
exit
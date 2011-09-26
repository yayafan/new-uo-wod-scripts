:top 
@echo off 
rem This batch file is the starter for POL0xx it is supposed to restart 
rem POL after it is closed down. 
c: 
cd \wod\ 
pol.exe 
REM for /f "tokens=2-4 delims=/ " %%a in ('DATE /T') do (set Date=%%c-%%b-%%a)
REM for /f "tokens=1-2 delims=: " %%a in ('ECHO. ^| TIME /T') do (set Time=%%a_%%bm)
REM rename pol.log pol%date%-%Time%.bak
REM sleep 10
goto top 

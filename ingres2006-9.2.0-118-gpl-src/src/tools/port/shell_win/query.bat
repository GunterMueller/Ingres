@echo off
set ii_sql=%1
:loop
shift
if "%1"=="" goto end
set ii_sql=%ii_sql% %1
goto loop
:end
echo on
@qbf -mall -lp %ii_sql%
@set ii_sql=

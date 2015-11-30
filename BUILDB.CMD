@REM *** Build bound EXE with EMX compiler (EXE requires EMX DLL's to run) ***
gcc -O -W -m486 -Zmtd startd.c startd.def -o startd.exe -s

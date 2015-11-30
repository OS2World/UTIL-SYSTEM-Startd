@REM *** Build stand-alone EXE with EMX compiler (EXE doesn't require EMX DLL's to run) ***
gcc -O -W -m486 -Zomf -Zsys startd.c startd.def -o startd.exe -los2 -s

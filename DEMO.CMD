REM
REM Start 2 dos sessions
REM
REM start a windowed dos session in the foreground at 0,240 size 640,240
REM and use demo1.ini as a settings file
REM and run the mem command keeping the session around afterwards
startd /win /dos /fg /k /sf demo1.ini /pos=0,240,320,240 mem 

REM same thing except uses demo2.ini
startd /win /dos /bg /k /sf demo2.ini /pos=0,0,320,240 mem


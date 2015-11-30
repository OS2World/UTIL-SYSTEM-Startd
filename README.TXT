STARTD V.1.00 README

The big difference between this program and the OS/2 'start' command
is that you can specify a settings file. STARTD will read the settings
file specified on the command line after the /SF switch and then start 
a dos session with those settings, otherwise it performs mostly like 'start'.

The settings file is just a list of the settings you want to change. One
setting per line (no blank lines) ie.

EMS_MEMORY_LIMIT=0
IDLE_SENSITIVITY=50
DOS_BACKGROUND_EXECUTION=OFF
.
.
.

The setting names are the same as the ones in DOS SETTINGS options.

Also STARTD will not ignore the /BG option and will actually start a dos
session in the background (good for those startup.cmd dos sessions).

Type STARTD /? for a list of the options it supports. 

The sample cmd file demo.cmd pops up two VDM's with different settings
and shows you the memory free in each one...

This is a quick hack so if it breaks tell me and I'll fix it (or fix it
yourself and tell me what I did wrong). I wrote this program without any
OS/2 technical reference (I'm still waiting for my redbooks) so I could 
concievably done horrid things. One thing I still can't understand is how
DosStartSession knows how big the environment buffer is. But it seems to 
work anyway...

Shareware:
	Basically share and enjoy!
	If you like/dislike the program send me email my
	address is npross@undergrad.uwaterloo.ca

	Although if you use my program on a regular basis and have an
	OS/2 shareware app requiring payment for regular use then I would
	like you to send me a note waiving the fee for that program.

	And if you want to send me money please append your VISA/MC number
	to your email message. ;-) (that's a joke for those who don't
	understand smilies and have a habit of sending hate mail to people
	who were obviously intending to be sarcastic)

This program is provided AS IS. There are no warranties expressed or implied
(except where prohibited by law -- what happens in that case I have no idea)
The author assumes no liability for damages caused by this program. I run it
everyday and haven't experienced any related difficulties...Mileage may vary.

BUGS/LIMITATIONS:

STARTD does not check for invalid combinations of command line switches. 
So if you specify STARTD /FS /WIN /PM /DOS you'll get a windowed DOS session
but who cares. At least it doesn't print "SYS179827346: You are a big jerk and
should know what parameters to specify"

STARTD's VDM sessions always run the autoexec.bat The OS/2 START doesn't if you
specify a command to execute. I think this is a feature :-)

I'm sure there are many more, but hey:

Lubarsky's Law of Cybernetic Entomology:
	There's always one more bug.

V.1.00-SGM ADDENDUM:

I have fixed the /WAIT switch and made a couple of minor changes.  I found /WAIT
essential for running certain DOS compiles from and OS/2 makefile (compilers that
can't run in parallel because of fixed temp file names or other reasons).

The STARTD.EXE included is a stand-alone executable built with EMX (v.08G) 
GCC (v.2.4.5) using the builds.cmd.  A bound executable can be created with the
alternative buildb.cmd if desired (smaller EXE, dynamically link latest EMX DLL's).

Stuart G. Mentzer
CIS ID: 71231,3173

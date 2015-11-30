/*
 *  STARTD:  A simple program to start a DOS session under OS/2 2.x with settings.
 *
 *  Last Modfied: 11/06/93
 *
 *  Author: Norm Ross / Modified by Stuart G. Mentzer
 *
 *  Using the example code for BOOTA.C found in IBM DOCUMENT GBOF-2254
 *
 *  MODIFICATION HISTORY
 *
 *  07-Jul-1992  Norm Ross  1.00 : Initial version
 *  06-Nov-1993  Stuart G. Mentzer  1.00-SGM : Fixed /WAIT + misc. mod's
 */

#define INCL_DOSSESMGR
#define INCL_DOSMISC
#define INCL_DOSPROCESS
#define INCL_DOSQUEUES   /* Queue values */
#include <os2.h>
#include <stdio.h>
#include <string.h>

PBYTE readEnv(PSZ pFileName);

PSZ pBootFailure = "Session could not be started.\r\n";
char szPgmInputs[256];
char szDosPgmInputs[256];

STARTDATA startd;                  /* Session start information */
ULONG SessionID;                   /* Session and Process ID for new session*/
PID ProcessID;
HQUEUE hq;                         /* Termination queue */

void main(int argc, char *argv[])
{
  USHORT       rc;
  USHORT       flagWin = 0, flagFs = 0, flagKeep = 0, flagNoCmd = 0;

  /* ---- init startd struct defaults */
  startd.Length                   = sizeof(STARTDATA);
  startd.Related                  = SSF_RELATED_INDEPENDENT;
  startd.FgBg                     = SSF_FGBG_FORE;
  startd.TraceOpt                 = SSF_TRACEOPT_NONE;
  startd.PgmTitle                 = NULL;
  startd.PgmName                  = NULL;
  startd.PgmInputs                = NULL;
  startd.TermQ                    = NULL;
  startd.Environment              = NULL;
  startd.InheritOpt               = SSF_INHERTOPT_PARENT;
  startd.SessionType              = SSF_TYPE_DEFAULT;
  startd.PgmControl               = SSF_CONTROL_VISIBLE;


  /* ------ Process args */
  while(--argc > 0) {
    char *arg;

    switch(**++argv) {
    case '\"':
	startd.PgmTitle = strtok(*argv, "\"");
	break;
    case '-':
    case '/':
      arg = (*argv)+1;
      if (strlen(arg) == 1) {
	switch(*arg) {
	case 'n':
	case 'N': flagNoCmd = 1; break;
	case 'k':
	case 'K': flagKeep = 1; break;
	case 'c':
	case 'C': break;
	case 'f':
	case 'F': startd.FgBg = SSF_FGBG_FORE; break;
	case 'B':
	case 'b': startd.FgBg = SSF_FGBG_BACK; break;
	case 'i':
	case 'I': startd.InheritOpt = SSF_INHERTOPT_SHELL; break;
	case '?': usage(); break;
	default:
	  fprintf(stderr, "Unrecognized option: %s\n", arg);
	  usage();
	  break;
	}
      } else if (stricmp(arg, "DOS") == 0) {
	startd.SessionType = SSF_TYPE_VDM;
      } else if (stricmp(arg, "WIN") == 0) {
	flagWin = 1;
      } else if (stricmp(arg, "WAIT") == 0) {
	startd.Related = SSF_RELATED_CHILD;
      } else if (stricmp(arg, "FS") == 0) {
	flagFs = 1;
      } else if (stricmp(arg, "FG") == 0) {
	startd.FgBg = SSF_FGBG_FORE;
      } else if (stricmp(arg, "BG") == 0) {
	startd.FgBg = SSF_FGBG_BACK;
      } else if (stricmp(arg, "MAX") == 0) {
	startd.PgmControl |= SSF_CONTROL_MAXIMIZE;
      } else if (stricmp(arg, "MIN") == 0) {
	startd.PgmControl |= SSF_CONTROL_MINIMIZE;
      } else if (stricmp(arg, "INV") == 0) {
	startd.PgmControl |= SSF_CONTROL_INVISIBLE;
      } else if (stricmp(arg, "PM") == 0) {
	startd.SessionType = SSF_TYPE_PM;
      } else if (strnicmp(arg, "pos", 3) == 0) {
	char *s = strtok(arg, "=");
	/* ---- I really should check strtok's return... */
	startd.PgmControl |= SSF_CONTROL_SETPOS;
	startd.InitXPos = atoi(strtok(NULL, ","));
	startd.InitYPos = atoi(strtok(NULL, ","));
	startd.InitXSize = atoi(strtok(NULL, ","));
	startd.InitYSize = atoi(strtok(NULL, ""));
      } else if (stricmp(arg, "PGM") == 0) {
	char *p = szPgmInputs;
	/* ---- strip quotes from name if there are any */
	startd.PgmName = strtok(*argv, "\"");

	/* ---- cat the rest of the args together to pass to pgm */
	while (argc > 1) {
	  argc--;
	  argv++;
	  strcpy(p, *argv);
	  p += strlen(*argv);
	  *p++ = ' '; /* put spaces between the args */
	}
	*p = '\0';
	startd.PgmInputs = szPgmInputs;
	break;
      } else if (stricmp(arg, "ICON") == 0) {
	startd.IconFile = *++argv;
	argc--;
      } else if (stricmp(arg, "SF") == 0) {
	char *fname = *++argv;
	argc--;
	if (access(fname, 4)) {
	  fprintf(stderr, "Session File %s not found\n", fname);
	}
	startd.Environment = readEnv(fname);
      } else {
	printf("Unrecognized option: %s\n", arg);
	usage();
      }
      break;

    default:
	{
	  char *p = szPgmInputs;
	  startd.PgmName = *argv;

	  /* ---- cat the rest of the args together to pass to pgm */
	  while (argc > 1) {
	    argc--;
	    argv++;
	    strcpy(p, *argv);
	    p += strlen(*argv);
	    *p++ = ' '; /* put spaces between the args */
	  }
	  *p = '\0';
	  startd.PgmInputs = szPgmInputs;
	  break;
	}

    } /* switch */
  } /* while */

  /* ------ Start thru Command processor */
  if ((startd.PgmName!=NULL)&&(startd.SessionType!=SSF_TYPE_PM)&&(!flagNoCmd)){
    if (flagKeep)
      strcpy(szDosPgmInputs, "/k ");
    else
      strcpy(szDosPgmInputs, "/c ");
    strcat(szDosPgmInputs, startd.PgmName);
    strcat(szDosPgmInputs, " ");
    strcat(szDosPgmInputs, startd.PgmInputs);
    startd.PgmInputs = szDosPgmInputs;
    startd.PgmName = NULL;
  }

  /* ------ Set the correct session type */
  if (flagWin) {
    switch(startd.SessionType) {
    case SSF_TYPE_DEFAULT: startd.SessionType = SSF_TYPE_WINDOWABLEVIO; break;
    case SSF_TYPE_VDM: startd.SessionType = SSF_TYPE_WINDOWEDVDM; break;
    case SSF_TYPE_PM: break;
    }
  } else if (flagFs) {
    switch(startd.SessionType) {
    case SSF_TYPE_DEFAULT: startd.SessionType = SSF_TYPE_FULLSCREEN; break;
    case SSF_TYPE_VDM: break;
    case SSF_TYPE_PM: break;
    }
  }

  /* ------ Set up the termination queue */
  {
   APIRET rdcq;
   rdcq = DosCreateQueue( &hq, QUE_FIFO | QUE_CONVERT_ADDRESS, "\\QUEUES\\term.que" );
   if (rdcq) {
     /* ------ Print out failure message */
     printf("DosCreateQueue Error %ld : ", rdcq);
     return;
   }
   startd.TermQ = "\\QUEUES\\term.que";
  }


  /* ------ Start the Session */
  rc = DosStartSession( &startd, &SessionID, &ProcessID );

  if (rc) {

    /* ------ Print out failure message */
    printf("DosStartSession Error %ld : ", rc);
    DosPutMessage( 1, strlen(pBootFailure), pBootFailure );
    DosExit( EXIT_PROCESS, rc ); /* SGM */

  } else if ( startd.Related == SSF_RELATED_CHILD ) {

    /* ------ Wait for child to finish - SGM */
    REQUESTDATA   Request;       /* Request-identification data  */
    ULONG         DataLength;    /* Length of element received   */
    PULONG        DataAddress;   /* Address of element received  */
    ULONG         ElementCode;   /* Request a particular element */
    BOOL32        NoWait;        /* No wait if queue is empty    */
    BYTE          ElemPriority;  /* Priority of element received */
    HEV           SemHandle;     /* Semaphore handle             */
    APIRET        rcq;           /* Return code                  */

    Request.pid = 0;           /* Set request data for current process   */
    ElementCode = 0;           /* Read from front of the queue           */
    NoWait = 0;                /* Read should wait if queue empty        */
    SemHandle = 0;             /* Unused since this is a call that waits */

    rcq = DosReadQueue( hq, &Request, &DataLength,
                       (PVOID *) &DataAddress, ElementCode, NoWait,
                       &ElemPriority, SemHandle );
    if (rcq) {
      printf( "DosReadQueue error: return code = %ld", rcq );
      return;
    }
  }

return;
}

#define MAXENV 4096
PBYTE readEnv(PSZ fname) {
  FILE *fptr;
  PBYTE env = (PBYTE)malloc(MAXENV);
  PBYTE p = env;

  fptr = fopen(fname, "r");
  if (fptr == (FILE *)NULL) {
    fprintf(stderr, "\nFile %s cannot be found\n");
    exit(-1);
  }

  while (fgets(p, 80, fptr)) {
    p+=strlen(p);
    *(p-1)='\0';
    if (p>env + 4096) {
      fprintf(stderr, "ERROR: too many settings\n");
      fflush(stderr);
      exit(-1);
    }
  }

  fclose( fptr ); /* SGM */
  realloc(env, p-env);

return(env);
}

usage( void ) {
  fprintf(stderr, "STARTD version 1.00-SGM\n\n");
  fprintf(stderr, "startd [\"program title\"] [/BG /C /DOS /F /FS /I /ICON iconfile /INV /K /MAX\n\t /MIN /PGM POS=x,y,x1,y1 /SF settingsfile /WIN] [command ...]\n\n");
  fprintf(stderr, "\t/B[G]\t start session in background\n");
  fprintf(stderr, "\t/C\t close session upon completion\n");
  fprintf(stderr, "\t/DOS\t start a dos session\n");
  fprintf(stderr, "\t/F[G]\t start session in foreground\n");
  fprintf(stderr, "\t/FS\t start a full screen session\n");
  fprintf(stderr, "\t/I\t sets SSF_INHERTOPT_SHELL\n");
  fprintf(stderr, "\t/ICON\t uses the specified icon file\n");
  fprintf(stderr, "\t/INV\t start the application invisibly\n");
  fprintf(stderr, "\t/K\t keep the session around after it is finished\n");
  fprintf(stderr, "\t/MAX\t start maximized\n");
  fprintf(stderr, "\t/MIN\t start minimized\n");
  fprintf(stderr, "\t/N\t don't start indirectly through command processor\n");
  fprintf(stderr, "\t/PGM\t the next argument is the program name\n");
  fprintf(stderr, "\t/PM\t start a PM program\n");
  fprintf(stderr, "\t/POS=x,y,x1,y1\t specify window position and size\n");
  fprintf(stderr, "\t/SF\t read the specified dos settings file\n");
  fprintf(stderr, "\t/WIN\t start a windowed session\n");
  fprintf(stderr, "\t/WAIT\t wait for session to finish\n");
  exit(1);
}

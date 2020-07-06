# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
#include <compat.h>
#include <tm.h>
#include <st.h>
#include <lo.h>
#include <si.h>
#include <er.h>
#include <evset.h>
/**
**  Name: screen - Screen mode part of idbg
**
**  Description:
**     This module supports the screen mode based operation of idbg
**
**     It contains the code for the following frames:-
**
**     main_frame    - The top level screen
**     view_frame    - Viewing files
**     export_frame  - Exporting evidence sets
**     add_frame     - Adding files to evidence sets
**     show_frame    - Displaying files
**
**  History:
**	22-feb-1996 - Initial port to Open Ingres
**	01-Apr-1996 - (prida01)
**			Add sunOS defines for port.
**      19-May-1998 - (horda03)
**          VMS port. In VMS Evidence Set Dump Stacks are
**          stored as binary files which must be opened
**          using SIfopen (bug 89423).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**    25-Oct-2005 (hanje04)
**        Add prototype for view_frame(), export_frame(), add_frame(),
**	  show_frame(), delete_evset_frame(), delete_file_frame() &
**	  import_frame(). Also remove cast of arg 4 in ERlookup() call,
**	  both to prevent compiler errors with GCC 4.0.
*/
#define BUF_SIZE 80
static VOID error();
static view_frame();
static export_frame();
static add_frame();
static show_frame();
static i4 delete_evset_frame();
static i4 delete_file_frame();
static import_frame();
main_frame()
{
  extern i4 main_screen;
  char buffer[BUF_SIZE];
  i4 id;
  i4 rows;
    EVSET_DETAILS details;
    STATUS status;
    i4  evset= -1;
    i4  going=TRUE;
    char *temp;
/* # line 74 "screen.sc" */	/* forms */
  {
    IIforms(0);
  }
/* # line 76 "screen.sc" */	/* addform */
  {
    IIaddform(main_screen);
  }
/* # line 78 "screen.sc" */	/* inittable */
  {
    if (IItbinit((char *)"main_screen",(char *)"data",(char *)"r") != 0) {
      IIthidecol((char *)"id",(char *)"integer");
      IItfill();
    } /* IItbinit */
  }
/* # line 80 "screen.sc" */	/* host code */
    while(going)
    {
/* # line 82 "screen.sc" */	/* display */
  {
    if (IIdispfrm((char *)"main_screen",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 86 "screen.sc" */	/* host code */
             /* Load up table with details of evidence sets */
             while((status=EVSetList(&evset,&details))==0)
             {
                 char time[30];
                 SYSTIME temp;
                 temp.TM_secs=details.created;
                 TMstr(&temp,time);
                 id=details.id;
                 STprintf(buffer,"%04d %s %s\n",details.id,time,
                    details.description);
/* # line 99 "screen.sc" */	/* loadtable */
    {
      if (IItbact((char *)"main_screen",(char *)"data",1) != 0) {
        IItcoputio((char *)"id",(short *)0,1,30,4,&id);
        IItcoputio((char *)"data",(short *)0,1,32,0,buffer);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 100 "screen.sc" */	/* host code */
             }
             if(status!=E_EVSET_NOMORE)
             {
                error(status,0,NULL);
             }
/* # line 107 "screen.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(40,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 113 "screen.sc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,(char *)"main_screen",(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&rows,32,(char *)"data",0);
          } /* IIiqset */
        }
/* # line 114 "screen.sc" */	/* host code */
             if(rows!=0)
             {
/* # line 116 "screen.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"main_screen",(char *)"data",-2) != 0) {
            IItcogetio((short *)0,1,30,4,&id,(char *)"id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 117 "screen.sc" */	/* host code */
                view_frame(id);
             }
      } else if (IIretval() == 2) {
/* # line 123 "screen.sc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,(char *)"main_screen",(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&rows,32,(char *)"data",0);
          } /* IIiqset */
        }
/* # line 124 "screen.sc" */	/* host code */
             if(rows!=0)
             {
/* # line 126 "screen.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"main_screen",(char *)"data",-2) != 0) {
            IItcogetio((short *)0,1,30,4,&id,(char *)"id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 127 "screen.sc" */	/* host code */
                export_frame(id);
             }
      } else if (IIretval() == 3) {
/* # line 133 "screen.sc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,(char *)"main_screen",(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&rows,32,(char *)"data",0);
          } /* IIiqset */
        }
/* # line 134 "screen.sc" */	/* host code */
             if(rows!=0)
             {
/* # line 136 "screen.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"main_screen",(char *)"data",-2) != 0) {
            IItcogetio((short *)0,1,30,4,&id,(char *)"id");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 137 "screen.sc" */	/* host code */
                if(delete_evset_frame(id))
/* # line 138 "screen.sc" */	/* deleterow */
        {
          if (IItbsetio(4,(char *)"main_screen",(char *)"data",-2) != 0) {
            if (IItdelrow(0) != 0) {
            } /* IItdelrow */
          } /* IItbsetio */
        }
/* # line 139 "screen.sc" */	/* host code */
             }
      } else if (IIretval() == 4) {
             import_frame();
/* # line 145 "screen.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
      } else if (IIretval() == 5) {
/* # line 150 "screen.sc" */	/* host code */
              going=FALSE;
/* # line 151 "screen.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    goto IIfdE1;
IImuI1:
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact((char *)"View",(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact((char *)"Export",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Delete",(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"Import",(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"Quit",(char *)0,2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 154 "screen.sc" */	/* host code */
    }
/* # line 156 "screen.sc" */	/* endforms */
  {
    IIendforms();
  }
/* # line 157 "screen.sc" */	/* host code */
}
static
view_frame(evset)
i4  evset;
{
  extern i4 view_screen;
  char *type;
  char *comment;
  char version[80];
  i4 evidenceset;
  char created[30];
  char *title;
  i4 current_file;
  i4 flags;
# define FLAG_CAN_SHOW 1
# define FLAG_CAN_DELETE 2
  i4 rows;
   static i4  done_addform=0;
   STATUS status;
   EVSET_DETAILS details;
   EVSET_ENTRY fdetails;
   SYSTIME temp;
   i4  my_evset=evset;
   i4  file;
   bool going=TRUE;
   if(!done_addform)
   {
/* # line 188 "screen.sc" */	/* addform */
  {
    IIaddform(view_screen);
  }
/* # line 189 "screen.sc" */	/* host code */
      done_addform=1;
   }
   /* Get in details of evidence set */
   EVSetList(&my_evset,&details);
   evidenceset=details.id;
   temp.TM_secs=details.created;
   TMstr(&temp,created);
   title=details.description;
   /* Get version information */
   EVSetLookupVar(evidenceset,"VERSION",version,sizeof(version)-1);
   while(going)
   {
/* # line 207 "screen.sc" */	/* display */
  {
    if (IIdispfrm((char *)"view_screen",(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    IIputfldio((char *)"evidenceset",(short *)0,1,30,4,&evidenceset);
    IIputfldio((char *)"created",(short *)0,1,32,0,created);
    IIputfldio((char *)"title",(short *)0,1,32,0,title);
    IIputfldio((char *)"version",(short *)0,1,32,0,version);
/* # line 212 "screen.sc" */	/* inittable */
    {
      if (IItbinit((char *)"view_screen",(char *)"data",(char *)"r") != 0) {
        IIthidecol((char *)"file",(char *)"integer");
        IIthidecol((char *)"flags",(char *)"integer");
        IItfill();
      } /* IItbinit */
    }
/* # line 215 "screen.sc" */	/* host code */
             /* Load table with list of files */
             file=0;
             while((status=EVSetFileList(evset,&file,&fdetails))==0)
             {
                 flags=(fdetails.flags & EVSET_TEXT)?FLAG_CAN_SHOW:0;
                 if((fdetails.flags & EVSET_SYSTEM)==0 )
                    flags|=FLAG_CAN_DELETE;
                 type=(fdetails.flags & EVSET_TEXT)?"TEXT":"BINARY";
                 comment=fdetails.description;
                 current_file=file-1;
/* # line 226 "screen.sc" */	/* loadtable */
    {
      if (IItbact((char *)"view_screen",(char *)"data",1) != 0) {
        IItcoputio((char *)"file",(short *)0,1,30,4,&current_file);
        IItcoputio((char *)"flags",(short *)0,1,30,4,&flags);
        IItcoputio((char *)"type",(short *)0,1,32,0,type);
        IItcoputio((char *)"comment",(short *)0,1,32,0,comment);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 228 "screen.sc" */	/* host code */
             }
             if(status!=E_EVSET_NOMORE)
             {
                error(status,0,NULL);
             }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 239 "screen.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"view_screen",(char *)"data",-2) != 0) {
            IItcogetio((short *)0,1,30,4,&flags,(char *)"flags");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 241 "screen.sc" */	/* host code */
             if(flags & FLAG_CAN_SHOW)
/* # line 242 "screen.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(40,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(
            1));
          } /* IIiqset */
        }
/* # line 243 "screen.sc" */	/* host code */
             else
/* # line 244 "screen.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(40,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 245 "screen.sc" */	/* host code */
             if(flags & FLAG_CAN_DELETE)
/* # line 246 "screen.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(40,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(
            1));
          } /* IIiqset */
        }
/* # line 247 "screen.sc" */	/* host code */
             else
/* # line 248 "screen.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(40,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
      } else if (IIretval() == 2) {
/* # line 254 "screen.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"view_screen",(char *)"data",-2) != 0) {
            IItcogetio((short *)0,1,30,4,&current_file,(char *)"file");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 255 "screen.sc" */	/* host code */
             show_frame(evset,current_file);
      } else if (IIretval() == 3) {
             add_frame(evset);
             /* Break out and redisplay the screen */
/* # line 262 "screen.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
      } else if (IIretval() == 4) {
/* # line 267 "screen.sc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,(char *)"main_screen",(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&rows,32,(char *)"data",0);
          } /* IIiqset */
        }
/* # line 268 "screen.sc" */	/* host code */
             if(rows!=0)
             {
/* # line 270 "screen.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"view_screen",(char *)"data",-2) != 0) {
            IItcogetio((short *)0,1,30,4,&current_file,(char *)"file");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 271 "screen.sc" */	/* host code */
                if(delete_file_frame(evset,current_file))
/* # line 272 "screen.sc" */	/* deleterow */
        {
          if (IItbsetio(4,(char *)"view_screen",(char *)"data",-2) != 0) {
            if (IItdelrow(0) != 0) {
            } /* IItdelrow */
          } /* IItbsetio */
        }
/* # line 273 "screen.sc" */	/* host code */
             }
      } else if (IIretval() == 5) {
              going=FALSE;
/* # line 279 "screen.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
      } else {
        if (1) goto IIfdE2;
      } /* IIretval */
    } /* IIrunform */
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    goto IIfdE2;
IImuI2:
    if (IITBcaClmAct((char *)"data",(char *)"all",1,1) == 0) goto IIfdE2;
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact((char *)"Show",(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IInmuact((char *)"Add",(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IInmuact((char *)"Delete",(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IInmuact((char *)"Quit",(char *)0,2,2,5) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 281 "screen.sc" */	/* host code */
   }
}
static
export_frame(evset)
i4  evset;
{
  extern i4 export_screen;
  char filename[80];
  short xnull;
   static i4  done_addform=0;
   STATUS status;
   if(!done_addform)
   {
/* # line 298 "screen.sc" */	/* addform */
  {
    IIaddform(export_screen);
  }
/* # line 299 "screen.sc" */	/* host code */
      done_addform=1;
   }
/* # line 302 "screen.sc" */	/* display */
  {
    if (IIdispfrm((char *)"export_screen",(char *)"u") == 0) goto IIfdE3;
    goto IImuI3;
IIfdI3:;
IIfdB3:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 306 "screen.sc" */	/* getform */
        {
          if (IIfsetio((char *)"export_screen") != 0) {
            IIgetfldio(&xnull,1,32,79,filename,(char *)"data");
          } /* IIfsetio */
        }
/* # line 307 "screen.sc" */	/* message */
        {
          IImessage((char *)"Exporting data...");
        }
/* # line 308 "screen.sc" */	/* host code */
          if((status=EVSetExport(evset,filename))==0)
          {
/* # line 310 "screen.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE3;
        }
/* # line 311 "screen.sc" */	/* host code */
          }
          else
          {
             error(E_EXPORT_CREATE,STlength(filename),(PTR)filename);
          }
      } else if (IIretval() == 2) {
/* # line 320 "screen.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE3;
        }
      } else {
        if (1) goto IIfdE3;
      } /* IIretval */
    } /* IIrunform */
IIfdF3:
    if (IIchkfrm() == 0) goto IIfdB3;
    goto IIfdE3;
IImuI3:
    if (IIinitmu() == 0) goto IIfdE3;
    if (IInmuact((char *)"Accept",(char *)0,2,2,1) == 0) goto IIfdE3;
    if (IInmuact((char *)"Quit",(char *)0,2,2,2) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE3;
    if (IIendmu() == 0) goto IIfdE3;
    goto IIfdI3;
IIfdE3:;
    IIendfrm();
  }
/* # line 322 "screen.sc" */	/* host code */
}
static
add_frame(evset)
i4  evset;
{
  extern i4 add_screen;
  char description[40];
  char type[10];
  char filename[80];
  i4 current_file;
  short xnull;
   static i4  done_addform=0;
   char new_file[EVSET_MAX_PATH];
   STATUS status;
   if(!done_addform)
   {
/* # line 342 "screen.sc" */	/* addform */
  {
    IIaddform(add_screen);
  }
/* # line 343 "screen.sc" */	/* host code */
      done_addform=1;
   }
/* # line 346 "screen.sc" */	/* display */
  {
    if (IIdispfrm((char *)"add_screen",(char *)"u") == 0) goto IIfdE4;
    goto IImuI4;
IIfdI4:;
    if (IIfsetio((char *)0) == 0) goto IIfdE4;
    IIputfldio((char *)"type",(short *)0,1,32,0,(char *)"TEXT");
IIfdB4:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 352 "screen.sc" */	/* getform */
        {
          if (IIfsetio((char *)"add_screen") != 0) {
            IIgetfldio(&xnull,1,32,39,description,(char *)"description");
            IIgetfldio(&xnull,1,32,0,type,(char *)"type");
            IIgetfldio(&xnull,1,32,79,filename,(char *)"filename");
          } /* IIfsetio */
        }
/* # line 355 "screen.sc" */	/* host code */
          if((status=EVSetCreateFile(evset,
              type[0]=='T'?EVSET_TEXT:EVSET_BINARY,description,new_file,
              sizeof(new_file)))==OK)
          {
             LOCATION from_loc;
             LOCATION to_loc;
             FILE *fdes;
             if((status=LOfroms(PATH&FILENAME,filename,&from_loc))!=OK ||
                (status=LOfroms(PATH&FILENAME,new_file,&to_loc))!=OK ||
                (status=SIopen(&to_loc,"w",&fdes))!=OK ||
                (status=SIcat(&from_loc,fdes))!=OK)
             {
                error(E_COPY_FAIL,STlength(filename),(PTR)filename);
             }
             SIclose(fdes);
         }
         else
         {
             error(status,0,NULL);
         }
/* # line 378 "screen.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE4;
        }
      } else if (IIretval() == 2) {
/* # line 384 "screen.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE4;
        }
      } else {
        if (1) goto IIfdE4;
      } /* IIretval */
    } /* IIrunform */
IIfdF4:
    if (IIchkfrm() == 0) goto IIfdB4;
    goto IIfdE4;
IImuI4:
    if (IIinitmu() == 0) goto IIfdE4;
    if (IInmuact((char *)"Accept",(char *)0,2,2,1) == 0) goto IIfdE4;
    if (IInmuact((char *)"Quit",(char *)0,2,2,2) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE4;
    if (IIendmu() == 0) goto IIfdE4;
    goto IIfdI4;
IIfdE4:;
    IIendfrm();
  }
/* # line 386 "screen.sc" */	/* host code */
}
static
show_frame(evset,file)
i4  evset;
i4  file;
{
  extern i4 show_screen;
  char *title;
  char buffer[BUF_SIZE+1];
   EVSET_ENTRY fdetails;
   static i4  done_addform=0;
   LOCATION my_loc;
   FILE *fdes;
   STATUS status;
   if((status=EVSetFileList(evset,&file,&fdetails))!=OK)
   {
      error(status,0,NULL);
   }
#ifndef VMS 
  if (fdetails.flags & EVSET_BINARY)
   {
      error(E_VIEW_BINARY_FAIL,0,NULL);
   }
   else 
   {
#endif
      if(!done_addform)
      {
/* # line 418 "screen.sc" */	/* addform */
  {
    IIaddform(show_screen);
  }
/* # line 419 "screen.sc" */	/* host code */
         done_addform=1;
      }
      title=fdetails.description;
/* # line 424 "screen.sc" */	/* display */
  {
    if (IIdispfrm((char *)"show_screen",(char *)"r") == 0) goto IIfdE5;
    goto IImuI5;
IIfdI5:;
    if (IIfsetio((char *)0) == 0) goto IIfdE5;
    IIputfldio((char *)"title",(short *)0,1,32,0,title);
/* # line 429 "screen.sc" */	/* host code */
#ifdef VMS
        if (fdetails.flags & EVSET_TEXT)
        {
#endif
         if(LOfroms(PATH&FILENAME,fdetails.value,&my_loc)==OK &&
            SIopen(&my_loc,"r",&fdes)==OK)
         {
/* # line 436 "screen.sc" */	/* inittable */
    {
      if (IItbinit((char *)"show_screen",(char *)"data",(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 438 "screen.sc" */	/* message */
    {
      IImessage((char *)"Reading data...");
    }
/* # line 440 "screen.sc" */	/* host code */
            while(SIgetrec(buffer,BUF_SIZE,fdes)==OK)
            {
/* # line 442 "screen.sc" */	/* loadtable */
    {
      if (IItbact((char *)"show_screen",(char *)"data",1) != 0) {
        IItcoputio((char *)"data",(short *)0,1,32,0,buffer);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 443 "screen.sc" */	/* host code */
            }
            SIclose(fdes);
         }
#ifdef VMS
      }
      else
      {
      EV_STACK_ENTRY StackRec;
      char           *string;
      char           prev_session [BUF_SIZE+1];
      i4             first = 1;
      /* Binary File. */
      string = & StackRec.Vstring[2];
      if(LOfroms(PATH&FILENAME,fdetails.value,&my_loc)==OK &&
            SIfopen(&my_loc, "r", SI_RACC, (i4) sizeof(EV_STACK_ENTRY), &fdes)==OK)
         {
            i4 count;
/* # line 466 "screen.sc" */	/* inittable */
    {
      if (IItbinit((char *)"show_screen",(char *)"data",(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 468 "screen.sc" */	/* message */
    {
      IImessage((char *)"Reading data...");
    }
/* # line 470 "screen.sc" */	/* host code */
            while(SIread( fdes, sizeof(EV_STACK_ENTRY), &count, &StackRec)==OK)
            {
               STprintf( buffer, "%-80s", string);
               if (STscompare( buffer, BUF_SIZE * sizeof(*buffer), prev_session, BUF_SIZE * sizeof(*prev_session)) )
               {
                  STprintf( prev_session, "%-80s", string);
                  if (! first)
                  {
/* # line 480 "screen.sc" */	/* loadtable */
    {
      if (IItbact((char *)"show_screen",(char *)"data",1) != 0) {
        IItcoputio((char *)"data",(short *)0,1,32,0,(char *)" ");
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 481 "screen.sc" */	/* host code */
                  }
                  else first = 0;
/* # line 484 "screen.sc" */	/* loadtable */
    {
      if (IItbact((char *)"show_screen",(char *)"data",1) != 0) {
        IItcoputio((char *)"data",(short *)0,1,32,0,buffer);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 485 "screen.sc" */	/* host code */
               }
               STprintf( buffer, "[ pc: %08x      %8s          %8s      sp: %08x",
                         StackRec.pc,
                         " ",                  /* Would like to be PV */
                         " ",                  /* Would like to be FP */
                         StackRec.sp );
/* # line 493 "screen.sc" */	/* loadtable */
    {
      if (IItbact((char *)"show_screen",(char *)"data",1) != 0) {
        IItcoputio((char *)"data",(short *)0,1,32,0,buffer);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 495 "screen.sc" */	/* host code */
               STprintf( buffer, "  r16-r21: %08x %08x %08x %08x %08x %08x ]",
                         StackRec.args[0],
                         StackRec.args[1],
                         StackRec.args[2],
                         StackRec.args[3],
                         StackRec.args[4],
                         StackRec.args[5] );
/* # line 503 "screen.sc" */	/* loadtable */
    {
      if (IItbact((char *)"show_screen",(char *)"data",1) != 0) {
        IItcoputio((char *)"data",(short *)0,1,32,0,buffer);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 504 "screen.sc" */	/* host code */
            }
            SIclose(fdes);
         }
      }
#endif
IIfdB5:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 515 "screen.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE5;
        }
      } else {
        if (1) goto IIfdE5;
      } /* IIretval */
    } /* IIrunform */
IIfdF5:
    if (IIchkfrm() == 0) goto IIfdB5;
    goto IIfdE5;
IImuI5:
    if (IIinitmu() == 0) goto IIfdE5;
    if (IInmuact((char *)"Quit",(char *)0,2,2,1) == 0) goto IIfdE5;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE5;
    if (IIendmu() == 0) goto IIfdE5;
    goto IIfdI5;
IIfdE5:;
    IIendfrm();
  }
/* # line 518 "screen.sc" */	/* host code */
#ifndef VMS
   }
#endif
}
static i4
delete_evset_frame(evset)
i4  evset;
{
  char xprompt[50];
  char reply[2];
   STATUS status;
   STprintf(xprompt,"Delete evidence set %04d - Are you sure ?",evset);
/* # line 535 "screen.sc" */	/* prompt */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(4,(short *)0,1,30,sizeof(40),(void *)IILQint(40));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(10),(void *)IILQint(10));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(10),(void *)IILQint(10));
    IIFRgpcontrol(2,0);
    IIprmptio(0,xprompt,(short *)0,1,32,1,reply);
  }
/* # line 538 "screen.sc" */	/* host code */
   if(reply[0]=='y' || reply[0]=='Y')
   {
      if((status=EVSetDelete(evset))==0)
      {
         return(1);
      }
      else
      {
         error(status,0,NULL);
      }
   }
   return(0);
}
static i4
delete_file_frame(evset,file)
i4  evset;
i4  file;
{
  char xprompt[50];
  char reply[2];
   STATUS status;
   STprintf(xprompt,"Delete file - Are you sure ?");
/* # line 566 "screen.sc" */	/* prompt */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(4,(short *)0,1,30,sizeof(40),(void *)IILQint(40));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(10),(void *)IILQint(10));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(10),(void *)IILQint(10));
    IIFRgpcontrol(2,0);
    IIprmptio(0,xprompt,(short *)0,1,32,1,reply);
  }
/* # line 569 "screen.sc" */	/* host code */
   if(reply[0]=='y' || reply[0]=='Y')
   {
      if((status=EVSetDeleteFile(evset,file))==0)
      {
         return(1);
      }
      else
      {
         error(status,0,NULL);
      }
   }
   return(0);
}
static 
import_frame()
{
  char filename[EVSET_MAX_PATH];
   STATUS status;
   i4  dummy;
/* # line 593 "screen.sc" */	/* prompt */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpsetio(4,(short *)0,1,30,sizeof(78),(void *)IILQint(78));
    IIFRgpsetio(2,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    IIFRgpsetio(1,(short *)0,1,30,sizeof(10),(void *)IILQint(10));
    IIFRgpcontrol(2,0);
    IIprmptio(0,(char *)"Enter filename of import file",(short *)0,1,32,79,
    filename);
  }
/* # line 596 "screen.sc" */	/* message */
  {
    IImessage((char *)"Importing data...");
  }
/* # line 598 "screen.sc" */	/* host code */
   if((status=EVSetImport(filename,&dummy))!=OK)
      error(status,0,NULL);
}
static VOID
error(status,len,value)
STATUS status;
i4     len;
PTR    value;
{
  char xmessage[ER_MAX_LEN];
   i4  langcode;
   i4  mylen;
   CL_ERR_DESC clerr;
   ER_ARGUMENT er_arg;
   er_arg.er_size=len;
   er_arg.er_value=value;
   ERlangcode(NULL,&langcode);
   if(ERlookup(status,NULL,ER_TEXTONLY,NULL,xmessage,sizeof(xmessage),
      langcode,&mylen,&clerr,1,&er_arg)!=OK)
   {
      STprintf(xmessage,"Failure to lookup error message 0x%x",status);
   }
/* # line 627 "screen.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(xmessage);
  }
/* # line 628 "screen.sc" */	/* host code */
}

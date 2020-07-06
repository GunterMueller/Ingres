/*
**  Copyright (c) 1996, 2002 Ingres Corporation
*/

/*
**  Name: ckxcopy - wrapper utility for XCOPY, used by ckpdb, rollforwarddb
**
**  Description:
**      This utility executes the NT xcopy command, optionally
**      taking a list of files. It is used in the cktmpl.def file
**      as a replacement for ckcopyd.bat. Ckcopyd had the limitation
**      of not being able to communicate an xcopy error (like "out of
**      disk space") back to the originating program (dmfjsp).
**
**      The syntax as used in cktmpl.def is:
**
**          ckxcopy %D %A BACKUP
**          ckxcopy %D %A BACKUP PARTIAL %B
**          ckxcopy %D %A RESTORE
**          ckxcopy %D %A RESTORE PARTIAL %B
**
**          %D  Database directory
**          %A  Checkpoint directory
**          %B  List of individual table files
**
**  Examples of passed parameters:
**
**      ckxcopy d:\oping\ingres\data\default\mydb
**              d:\oping\ingres\ckp\default\mydb\c0001001.ckp
**              BACKUP
**
**      ckxcopy d:\oping\ingres\data\default\mydb
**              d:\oping\ingres\ckp\default\mydb\c0002002.ckp
**              RESTORE PARTIAL
**              aaaaaamp.t00 aaaaaalb.t00
**
**  History:
**      01-May-96 (sarjo01)
**          Created.
**      21-May-97 (musro02)
**          Made compilation conditional on NT_GENERIC
**      29-Jul-97 (merja01)
**          Added preproccessor stmt for NT_GENERIC to prevent compile 
**          errors on unix platforms.
**	26-sep-97 (mcgem01)
**	    Remove extraneous ifdef.
**	20-feb-1998 (somsa01)
**	    Since this executable is spawned from dmfjsp, the "Control C"
**	    handler which is setup by EX in dmfjsp will not pass through
**	    to here. We also do not want to allow the user to "Control C"
**	    out of this when it is running. Therefore, disable "Control C"
**	    processing from here as well as from children spawned from here.
**	15-nov-1997 (canor01)
**	    Must quote pathnames when passing them on command line to
**	    suppport embedded spaces.
**	27-apr-2000 (cucjo01)
**	    Added extra XCopy flag "/Y" to supress prompting if a file 
**	    exists in Windows 2000 since it will currently prompt you 
**	    if a file exists in the destination.  This behavior has
**	    changed since Windows NT 4.0.
**	15-may-2002 (somsa01)
**	    Added copy_files() function, which replaces the running of
**	    xcopy. This was done to properly take care of localization
**	    issues. Also, removed the docommand() function since it is
**	    not used anymore.
*/
#ifdef NT_GENERIC  /* only build for NT_GENERIC */

#include <compat.h>
#include <cm.h>
#include <si.h>
#include <st.h>
#include <stdio.h>
#include <string.h>

void syntax();
int  copy_files(char *source, char *destination);

int
main(int argc, char *argv[])
{
    int     partial = 0;
    int     stat = 0;
    int     mode = 0;  /* backup */
    int     tblcount, i, j;
    char    *src, *dest, src2[1024], dest2[1024];

    if (argc < 4)
        syntax();

    CMset_locale();

    SetConsoleCtrlHandler(NULL, TRUE);

    if (stricmp(argv[3], "restore") == 0)
    {
        mode = 1;
        src  = argv[2];
        dest = argv[1];
    }
    else if (stricmp(argv[3], "backup") != 0)
        syntax();
    else
    {
	/*
	** backup
	*/
        src  = argv[1];
        dest = argv[2];
    }

    if (argc > 4)
    {
        if (argc < 6 || stricmp(argv[4], "partial") != 0) 
            syntax();
        partial = 1;
        tblcount = argc - 5;
    }

    if (!partial)
	stat = copy_files(src, dest);
    else
    {
	/*
	** partial
	*/
        if (mode == 0)  /* backup */
	    CreateDirectory(dest, NULL);

        for (i = 0, j = 5; i < tblcount; i++, j++) 
        {
	    STprintf(src2, "%s\\%s", src, argv[j]);
	    STprintf(dest2, "%s\\%s", dest, argv[j]);
	    SIprintf("%s\n", src2);
	    SIflush(stdout);
	    if (CopyFile(src2, dest2, FALSE) == 0)
	    {
		SIprintf("Error = %d\n", GetLastError());
	    	SIflush(stdout);
		stat = 1;
		break;
	    }
	    else
	    {
		SIprintf("1 File(s) copied\n");
		SIflush(stdout);
	    }
        }
    }

    exit(stat);
}

void
syntax()
{
    printf("Usage: ckxcopy <db-path> <ckp-path>\n"
           "               { RESTORE | BACKUP } [ PARTIAL file1 file2 ... ]\n");
    exit(1);
}

int
copy_files(char *source, char *destination)
{
    WIN32_FIND_DATA	FindFileData;
    HANDLE		hFind;
    char		src[1024], FullFilePath[1024], FullDestPath[1024];
    bool		MoreData = TRUE;
    int			status = 0, NumFiles = 0;

    STprintf(src, "%s\\*", source);
    if ((hFind = FindFirstFile(src, &FindFileData)) == INVALID_HANDLE_VALUE)
    {
	SIprintf("Error = %d\n", GetLastError());
	SIflush(stdout);
	return(1);
    }

    if (GetFileAttributes(destination) == -1)
	CreateDirectory(destination, NULL);

    while (MoreData)
    {
	if (STcompare(FindFileData.cFileName, ".") != 0 &&
	    STcompare(FindFileData.cFileName, "..") != 0)
	{
	    STprintf(FullFilePath, "%s\\%s", source, FindFileData.cFileName);
	    STprintf(FullDestPath, "%s\\%s", destination,
		     FindFileData.cFileName);
	    SIprintf("%s\n", FullFilePath);
	    SIflush(stdout);
	    if (CopyFile(FullFilePath, FullDestPath, FALSE) == 0)
	    {
		SIprintf("Error = %d\n", GetLastError());
		SIflush(stdout);
		status = 1;
	    }
	    else
		NumFiles++;
	}

	MoreData = FindNextFile(hFind, &FindFileData);
    }

    SIprintf("%d File(s) copied\n", NumFiles);
    SIflush(stdout);
    return(status);
}
# else
int main();
#endif /* NT_GENERIC */

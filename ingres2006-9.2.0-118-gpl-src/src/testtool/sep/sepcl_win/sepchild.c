#include <compat.h>
#include <clconfig.h>
#include <systypes.h>
#include <clnfile.h>
#include <lo.h>
#include <pc.h>
#include <er.h>
#include <si.h>
#include <st.h>
#include <cm.h>
#include <cs.h>
#include <me.h>

#include <sepdefs.h>
#include <fndefs.h>

#include <errno.h>

#include <stdlib.h>

/*
** History:
**
**	##-sep-1995 (somsa01)
**		Created. Used only on NT_GENERIC platforms with sepspawn.
**	26-Jan-1996 (somsa01)
**		Changed "nullfile.tmp" to "nul".
**      14-Jan-97 (fanra01)
**              Modified to use CreateProcess and to close handles not needed
**              by this process.  Previously preventing file deletion.
**      03-Feb-97 (fanra01)
**              Modified error handling from failed creation of console
**              handles.  Instead of exiting  try and continue with default
**              handle.
**	26-jul-1999 (somsa01)
**		When spawning a process, standard error for the child gets
**		redirected to standard out on UNIX. Let's follow suite here
**		on NT.
**      22-sep-2000 (mcgem01)
**              More nat and longnat to i4 changes.
**	27-feb-2001 (somsa01)
**		Make sure we don't re-close handles.
*/
/*
DEST = TOOLS

NEEDLIBS = COMPATLIB
**
*/
# define MAX_STD_HANDLES    4
# define MAX_CMD_LENGTH     1024

bool batchMode;

struct shm_struct {
  bool batchMode;
  i4  stderr_action;
  char prog_name[64];
  char in_name[256];
  char out_name[256];
  bool child_spawn;
};

static struct shm_struct stParams;

main(argc,argv)
i4  argc;
char **argv;
{
register i4             i;
char                    shm_name[20];
struct shm_struct       *childenv_ptr;
int                     allocated_pages;
CL_ERR_DESC             err_code;
STATUS                  mem_status;
char                    full_prog_name[256];
int                     exit_code;
HANDLE                  hConIO[MAX_STD_HANDLES];
char                    buf[64];
STARTUPINFO             si;
PROCESS_INFORMATION     pi;
SECURITY_ATTRIBUTES     sa;
char                    *pArgLine;
BOOL                    blStatus = FALSE;
DWORD                   dwAttrib = 0;

    /*
    **  Initialise security attributes for use with handle creation and
    **  duplication.
    */
    iimksec(&sa);

    /*
    **  Remove the shared memory name from the list of arguments.
    */
    strcpy (shm_name, argv[argc-1]);
    argv[argc-1]=NULL;
    argc=argc-1;

    /*
    **  Get the pointer tp shared memory and copy the contents into a local
    **  area.
    */
    mem_status = IIMEget_pages(ME_SSHARED_MASK,1,shm_name,(PTR *)&childenv_ptr,
                               &allocated_pages,&err_code);

    if (mem_status == OK)
    {
        MEcopy(childenv_ptr, sizeof(struct shm_struct), &stParams);
        childenv_ptr->child_spawn = TRUE;
        mem_status = IIMEfree_pages((PTR)childenv_ptr,1,&err_code);
    }
    else
    {
        _exit(509);
    }

    /*
    **  Allocate an area to reconstruct the command line from the argv ptrs.
    */
    pArgLine = MEreqmem (0,MAX_CMD_LENGTH,TRUE,NULL);
    if (pArgLine != NULL)
    {
        for (i=0; (i < argc) && (argv[i] != NULL); i++)
        {
            STcat(pArgLine,argv[i]);
            if (argc > (i+1))
                STcat(pArgLine," ");
        }
    }
    else
    {
        _exit(509);
    }

    /*
    **  Initialise the handle array which will take the created or duplicated
    **  handles for standard I/O.
    */
    for (i = 0; i < MAX_STD_HANDLES; i++)
    {
        hConIO[i] = 0;
    }

    /*
    **  Setup standard input handle for the child process.
    **  If input is redirected from a file open the file. If we're not in batch
    **  get the current handle.
    */
    if ((stParams.batchMode) || (stParams.in_name[0] != 0))
    {
        if (stParams.in_name[0] == 0)
        {
            STcopy((PTR)ERx("c:\\nul"),stParams.in_name);
        }
        dwAttrib = OPEN_ALWAYS;
    }
    else
    {                                   /* Batch mode with no std input */
        STcopy((PTR)ERx("CONIN$"),stParams.in_name);
        dwAttrib = OPEN_EXISTING;
    }
    hConIO[1] = CreateFile(
                            stParams.in_name,
                            GENERIC_READ|GENERIC_WRITE,
                            FILE_SHARE_READ|FILE_SHARE_WRITE,
                            &sa,
                            dwAttrib,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL
                          );
    if (hConIO[1] == INVALID_HANDLE_VALUE)
    {
        hConIO[1] = NULL;
    }

    /*
    **  Setup standard output handle for the child process.
    **  If output is redirected from a file open the file. If we're not in
    **  batch mode get the current handle.
    */
    if ((stParams.batchMode) || (stParams.out_name[0] != 0))
    {
        if (stParams.out_name[0] == 0)
        {
            STcopy((PTR)ERx("c:\\nul"),stParams.out_name);
        }
        dwAttrib = OPEN_ALWAYS;
    }
    else
    {                                   /* Batch mode with no std input */
        STcopy((PTR)ERx("CONOUT$"),stParams.out_name);
        dwAttrib = OPEN_EXISTING;
    }
    hConIO[2] = CreateFile(
                            stParams.out_name,
                            GENERIC_READ|GENERIC_WRITE,
                            FILE_SHARE_READ|FILE_SHARE_WRITE,
                            &sa,
                            dwAttrib,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL
                          );
    if (hConIO[2] == INVALID_HANDLE_VALUE)
    {
        hConIO[2] = NULL;
    }
    /*
    **  Setup standard error handle for the child process.
    */
    switch (stParams.stderr_action)
    {
        case 1:
	    hConIO[3] = CreateFile(
                                    "c:\\nul",
                                    GENERIC_READ|GENERIC_WRITE,
                                    FILE_SHARE_READ|FILE_SHARE_WRITE,
                                    &sa,
                                    OPEN_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL
                                  );
            break;
        case 0:
        case 2:
	    /* Standard error goes to Standard out */
	    hConIO[3] = hConIO[2];
            break;
    }
    if (hConIO[3] == INVALID_HANDLE_VALUE)
    {
        hConIO[3] = NULL;
    }

    si.cb = sizeof (STARTUPINFO);
    si.lpReserved       = 0;
    si.lpDesktop        = NULL;
    si.lpTitle          = NULL;
    si.dwX              = 0;
    si.dwY              = 0;
    si.dwXSize          = 0;
    si.dwYSize          = 0;
    si.dwXCountChars    = 0;
    si.dwYCountChars    = 0;
    si.dwFillAttribute  = 0;
    si.dwFlags          = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
    si.wShowWindow      = SW_HIDE;
    si.hStdInput        = (hConIO[1])?hConIO[1]:GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput       = (hConIO[2])?hConIO[2]:GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError        = (hConIO[3])?hConIO[3]:GetStdHandle(STD_ERROR_HANDLE);
    si.cbReserved2      = 0;
    si.lpReserved2      = 0;

    if (SearchPath (NULL, stParams.prog_name, ".com",
                    sizeof(full_prog_name),full_prog_name, NULL)==0)
        if (SearchPath (NULL, stParams.prog_name, ".exe",
                        sizeof(full_prog_name),full_prog_name, NULL)==0)
            if (SearchPath (NULL, stParams.prog_name, ".bat",
                            sizeof(full_prog_name),full_prog_name, NULL)==0)
                if (SearchPath (NULL, stParams.prog_name, ".cmd",
                                sizeof(full_prog_name),full_prog_name, NULL)==0)
    {
        /* Handle error condition */
        STprintf(buf, "SearchPath failed %d", GetLastError());
        SIfprintf(stderr, "%s\n", buf);
        SIflush(stderr);
        _exit(509);
    }
    if (CreateProcess (
                        full_prog_name,
                        pArgLine,
                        NULL,
                        NULL,
                        TRUE,
                        NORMAL_PRIORITY_CLASS,
                        NULL,
                        NULL,
                        &si,
                        &pi) == 0)
    {
        STprintf(buf, "CreateProcess failed %d", GetLastError());
        SIfprintf(stderr, "%s\n", buf);
        SIflush(stderr);
        if (hConIO[1])
            CloseHandle(hConIO[1]);
        if (hConIO[2])
            CloseHandle(hConIO[2]);
        if (hConIO[3] && hConIO[3] != hConIO[2])
            CloseHandle(hConIO[3]);
        _exit(509);
    }
    else
    {
        if (hConIO[1])
            CloseHandle(hConIO[1]);
        if (hConIO[2])
            CloseHandle(hConIO[2]);
        if (hConIO[3] && hConIO[3] != hConIO[2])
            CloseHandle(hConIO[3]);
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess,&exit_code);
        _exit (exit_code);
    }
} /* sepchild */

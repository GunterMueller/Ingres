# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation 
*/
/*
** Name: fortunoff.sc
**
PROGRAM = fortunoff
NEEDLIBS = LIBINGRES
**
** Description:
**      Read configuration files to load fortunoff demo tables.
**
** History:
**      07-Apr-1999 (fanra01)
**          Created. CL'ized for marol01.
*/
# include <compat.h>
# include <cv.h>
# include <er.h>
# include <lo.h>
# include <me.h>
# include <pc.h>
# include <si.h>
# include <st.h>
/*
** Declare the SQLCA structure and the SQLDA typedef
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
#define BLOCK_SIZE 1000
#define LINE_SIZE 4096
#define COMMENT_SIZE 2048
#define NAME_SIZE 33
#define PATH_SIZE 257
#define URL_SIZE 257
# ifdef NT_GENERIC
# define CHR_PATH_DELIM '\\'
# define STR_PATH_DELIM "\\"
# else
# ifdef UNIX
# define CHR_PATH_DELIM '/'
# define STR_PATH_DELIM "/"
# endif
# endif
void    II_Err ();
void    putHandler( char *filename );
char*   GetWord( char **line );
STATUS  loadArticle ( char *conf_file );
STATUS  loadArticleComment ( char *conf_file );
STATUS  loadMenu ( char *conf_file );
STATUS  loadImage ( char *conf_file );
STATUS  loadLocation( char *conf_file );
STATUS  CreateCommand();
STATUS  Guestbook();
STATUS  loadArticleMenu ( char *conf_file );
char *ImagePath;
/*
** Name: main
**
** Description:
**      Connect to specified database and create and load tables as specified
**      in the cfg files.
**
** Input:
**      argv[1]     dbname      Database to load
**      argv[2]     CFGpath     Path to configuration files
**      argv[3]     ImagePath   Path to blobs to store in database
**
** Output:
**      None.
**
** Returns:
**      OK      completed successfully
**      FAIL    on error
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
*/
STATUS
main( i4 argc, char **argv )
{
    STATUS  status = OK;
  char *dbname;
    char *CfgPath;
    char *name;
    if (argc != 4)
    {
        SIfprintf( stderr,
            ERx("usage : %s <database name> <cfg directory> <image directory>\n"),
            argv[0] );
        PCexit( FAIL );
    }
    if ((dbname = STalloc( argv[1] )) == NULL)
    {
        SIfprintf( stderr,
            ERx("Not enought memory to load the fortunoff application.\n") );
        PCexit( FAIL );
    }
    if ((CfgPath = STalloc( argv[2] )) == NULL)
    {
        SIfprintf( stderr,
            ERx("Not enought memory to load the fortunoff application.\n") );
        PCexit( FAIL );
    }
    if ((ImagePath = STalloc( argv[3] )) == NULL)
    {
        SIfprintf( stderr,
            ERx("Not enought memory to load the fortunoff application.\n") );
        PCexit( FAIL );
    }
    SIfprintf( stderr, "connection to %s\n", dbname );
/* # line 127 "fortunoff.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser((char *)"icedbuser");
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
  }
/* # line 129 "fortunoff.sc" */	/* host code */
    if (sqlca.sqlcode != 0)
    {
        SIfprintf( stderr, ERx("Unable to open the database\n") );
        PCexit( FAIL );
    }
/* # line 137 "fortunoff.sc" */	/* host code */
    if ((name = MEreqmem( 0, strlen(CfgPath) + 20, TRUE, NULL )) == NULL)
    {
        SIfprintf( stderr,
            ERx("Not enought memory to load the fortunoff application.\n") );
        PCexit( FAIL );
    }
    STprintf( name, "%s%cmenu.cfg", CfgPath, CHR_PATH_DELIM );
    loadMenu( name );
    STprintf( name, "%s%cimages.cfg", CfgPath, CHR_PATH_DELIM );
    loadImage( name );
    STprintf( name, "%s%carticles.cfg", CfgPath, CHR_PATH_DELIM );
    loadArticle( name );
    STprintf( name, "%s%cartcomments.cfg", CfgPath, CHR_PATH_DELIM );
    loadArticleComment( name );
    STprintf( name, "%s%cartmenu.cfg", CfgPath, CHR_PATH_DELIM );
    loadArticleMenu( name );
    STprintf( name, "%s%clocation.cfg", CfgPath, CHR_PATH_DELIM );
    loadLocation( name );
    CreateCommand();
    Guestbook();
/* # line 159 "fortunoff.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 160 "fortunoff.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 162 "fortunoff.sc" */	/* host code */
    MEfree( name );
    MEfree( dbname );
    MEfree( CfgPath );
    MEfree( ImagePath );
    return(status);
}
/*
** Name: II_Err
**
** Description:
**      SQL Error handler.  Displays error message and rolls back transaction.
**
** Input:
**      None.
**
** Output:
**      None.
**
** Returns:
**      None.
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
*/
void
II_Err()
{
  char text[2000];
  i4 error;
/* # line 196 "fortunoff.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&error,2);
    IILQisInqSqlio((short *)0,1,32,1999,text,63);
  }
/* # line 198 "fortunoff.sc" */	/* host code */
    if (error != 0 && error != 30100)
    {
        SIfprintf( stderr, "error : %d '%s'\n", error, text );
/* # line 201 "fortunoff.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 202 "fortunoff.sc" */	/* host code */
        PCexit( FAIL );
    }
}
/*
** Name: File_Err
**
** Description:
**      File open error handler.  Rolls back transaction.
**
** Input:
**      filename    Path and filename being opened.
**
** Output:
**      None.
**
** Returns:
**      None.
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
*/
void
File_Err( char *name )
{
    SIfprintf( stderr, "error opening file : %s\n", name );
/* # line 229 "fortunoff.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 230 "fortunoff.sc" */	/* host code */
    PCexit( FAIL );
}
/*
** Name: putHandler
**
** Description:
**      blob put handler.
**
** Input:
**      filename    name of blob file
**
** Output:
**      None.
**
** Returns:
**      None.
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
*/
void
putHandler( char *filename )
{
  char segment[BLOCK_SIZE];
  i4 length = 0;
    FILE *fd = NULL;
    LOCATION loc;
    char locpath[MAX_LOC + 1];
    STATUS status = OK;
    if (*filename != '\0')
    {
        STcopy( filename, locpath);
        LOfroms (PATH & FILENAME, locpath, &loc);
        if ((status = SIfopen( &loc, "r", SI_BIN, 0, &fd )) == OK)
        {
            SIfprintf( stdout, "opened the %s file \n", filename );
            while (status == OK)
            {
                status = SIread( fd, BLOCK_SIZE, &length, segment );
                if (((status == OK) ||(status == ENDFILE)) && length != 0)
                {
/* # line 276 "fortunoff.sc" */	/* put data */
  {
    IILQlpd_LoPutData(3,32,BLOCK_SIZE,segment,length,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 278 "fortunoff.sc" */	/* host code */
                }
            }
            SIclose( fd );
        }
        else
        {
            File_Err(filename);
        }
    }
/* # line 287 "fortunoff.sc" */	/* put data */
  {
    IILQlpd_LoPutData(0,0,0,(char *)0,0,1);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 288 "fortunoff.sc" */	/* host code */
    return;
}
/*
** Name: GetWord
**
** Description:
**      Return the next word from the line.
**
** Input:
**      line    pointer to input line.
**
** Output:
**      line    updated line pointer.
**
** Returns:
**      pointer to word.
**      no error return.
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
*/
char *
GetWord( char **line )
{
    char *tmp;
    char *word;
    tmp = *line;
    while (*tmp == ' ' && *tmp == '\t') tmp++;
    word = tmp;
    while (*tmp != ';' && *tmp != '\r' && *tmp != '\n' && *tmp != '\0') tmp++;
    *line = tmp+1;
    while (*tmp == ' ' || *tmp == '\t') tmp--;
    tmp[0] = '\0';
    return(word);
}
/*
** Name: loadMenu
**
** Description:
**      Create menu table and load the menu images into it.
**
** Input:
**      conf_file   path and filename to menu config file.
**
** Output:
**      None.
**
** Returns:
**      OK  successful
**      !0  failure
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
*/
STATUS
loadMenu (char *conf_file)
{
  char name[NAME_SIZE];
  char ref[URL_SIZE];
  char alt[COMMENT_SIZE];
  char image[PATH_SIZE];
  char line[LINE_SIZE];
    FILE *fd = NULL;
    LOCATION loc;
    char locpath[MAX_LOC + 1];
    STATUS status = OK;
    char *tmp;
    SIfprintf( stdout, "deleting existing menu table\n" );
/* # line 367 "fortunoff.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table menu");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 369 "fortunoff.sc" */	/* host code */
    SIfprintf( stdout, "creating menu table\n" );
/* # line 370 "fortunoff.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table menu(name varchar(32), ref varchar(256), alt varchar(256\
), image long byte)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 372 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "loading data from the %s file \n", conf_file);
    STcopy( conf_file, locpath);
    LOfroms (PATH & FILENAME, locpath, &loc);
    if ((status = SIfopen( &loc, "r", SI_BIN, 0, &fd )) == OK)
    {
        MEfill(LINE_SIZE, 0, line);
        while ((status = SIgetrec( line, LINE_SIZE, fd)) == OK )
        {
            tmp = line;
            STcopy( GetWord( &tmp ), name);
            STcopy( GetWord( &tmp ), ref);
            STcopy( GetWord( &tmp ), alt);
            STprintf( image, "%s%c%s", ImagePath, CHR_PATH_DELIM,
                GetWord(&tmp) );
            SIfprintf( stdout,
                "loading : (name : %s, ref : %s, alt : %s, image : %s)\n",
                name, ref, alt, image);
/* # line 390 "fortunoff.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into menu(name, ref, alt, image)values(");
    IIputdomio((short *)0,1,32,0,name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,ref);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,alt);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IILQldh_LoDataHandler(2,(short *)0,putHandler,image);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 392 "fortunoff.sc" */	/* host code */
            MEfill(LINE_SIZE, 0, line);
        }
        SIclose(fd);
        if (status == ENDFILE)
            status = OK;
    }
    else
    {
        File_Err(conf_file);
    }
/* # line 402 "fortunoff.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 403 "fortunoff.sc" */	/* host code */
    return (status);
}
/*
** Name: loadImage
**
** Description:
**      Create image table and load the images into it.
**
** Input:
**      conf_file   path and filename to menu config file.
**
** Output:
**      None.
**
** Returns:
**      OK  successful
**      !0  failure
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
**      02-Oct-2000 (fanra01)
**          Add a commit in between insert and modify.
*/
STATUS
loadImage( char *conf_file )
{
  char name[NAME_SIZE];
  char image[PATH_SIZE];
  char line[LINE_SIZE];
    FILE *fd = NULL;
    LOCATION loc;
    char locpath[MAX_LOC + 1];
    STATUS status = OK;
    char *tmp;
    SIfprintf(stdout, "deleting existing images table\n");
/* # line 443 "fortunoff.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table images");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 445 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "creating image table\n");
/* # line 446 "fortunoff.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table images(name varchar(32), image long byte)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 447 "fortunoff.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 448 "fortunoff.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify images to btree unique on name");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 450 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "loading data from the %s file \n", conf_file);
    STcopy( conf_file, locpath);
    LOfroms (PATH & FILENAME, locpath, &loc);
    if ((status = SIfopen( &loc, "r", SI_BIN, 0, &fd )) == OK)
    {
        MEfill(LINE_SIZE, 0, line);
        while ((status = SIgetrec( line, LINE_SIZE, fd)) == OK )
        {
            tmp = line;
            STcopy( GetWord( &tmp ), name );
            STprintf(image, "%s%c%s", ImagePath, CHR_PATH_DELIM,
                GetWord(&tmp));
            SIfprintf(stdout,
                "loading : (name : %s, image : %s)\n", name, image);
/* # line 465 "fortunoff.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into images(name, image)values(");
    IIputdomio((short *)0,1,32,0,name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IILQldh_LoDataHandler(2,(short *)0,putHandler,image);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 467 "fortunoff.sc" */	/* host code */
            MEfill(LINE_SIZE, 0, line);
        }
        SIclose(fd);
        if (status == ENDFILE)
            status = OK;
    }
    else
    {
        File_Err(conf_file);
    }
/* # line 477 "fortunoff.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 478 "fortunoff.sc" */	/* host code */
    return (status);
}
/*
** Name: loadArticle
**
** Description:
**      Create the articles table and populate it.
**
** Input:
**      conf_file   path and filename to menu config file.
**
** Output:
**      None.
**
** Returns:
**      OK  successful
**      !0  failure
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
**      02-Oct-2000 (fanra01)
**          Add a commit in between insert and modify.
*/
STATUS
loadArticle (char *conf_file)
{
  i4 siteld;
  i4 category;
  char cdProd[NAME_SIZE];
  char item[COMMENT_SIZE];
  char image[PATH_SIZE];
  char line[LINE_SIZE];
  char description[COMMENT_SIZE];
  i4 price;
  i4 special;
  i4 newitem;
  char AA[NAME_SIZE];
  char A0[NAME_SIZE];
  char A1[NAME_SIZE];
  char A2[NAME_SIZE];
  char A3[NAME_SIZE];
  char A4[NAME_SIZE];
  char A5[NAME_SIZE];
  char A6[NAME_SIZE];
    FILE *fd = NULL;
    LOCATION loc;
    char locpath[MAX_LOC + 1];
    STATUS status = OK;
    char *tmp;
    SIfprintf(stdout, "deleting existing articles table\n");
/* # line 533 "fortunoff.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table articles");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 535 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "creating articles table\n");
/* # line 536 "fortunoff.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table articles(siteld integer, productCode varchar(32), item v\
archar(256), category integer, description varchar(1000), price intege\
r, special integer, newitem integer, AA varchar(32), A0 varchar(32), A\
1 varchar(32), A2 varchar(32), A3 varchar(32)");
    IIwritio(0,(short *)0,1,32,0,(char *)
", A4 varchar(32), A5 varchar(32), A6 varchar(32), image long byte)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 555 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "loading data from the %s file \n", conf_file);
    STcopy( conf_file, locpath);
    LOfroms (PATH & FILENAME, locpath, &loc);
    if ((status = SIfopen( &loc, "r", SI_BIN, 0, &fd )) == OK)
    {
        MEfill(LINE_SIZE, 0, line);
        while ((status = SIgetrec( line, LINE_SIZE, fd)) == OK )
        {
            tmp = line;
            status = CVan( GetWord(&tmp), &siteld );
            STcopy( GetWord( &tmp ), cdProd);
            STcopy( GetWord( &tmp ), item);
            STprintf(image, "%s%c%s", ImagePath, CHR_PATH_DELIM, GetWord(&tmp));
            status = CVan( GetWord(&tmp), &category );
            STcopy( GetWord( &tmp ), description);
            status = CVan( GetWord(&tmp), &price );
            status = CVan( GetWord(&tmp), &special );
            status = CVan( GetWord(&tmp), &newitem );
            STcopy( GetWord( &tmp ), AA);
            STcopy( GetWord( &tmp ), A0);
            STcopy( GetWord( &tmp ), A1);
            STcopy( GetWord( &tmp ), A2);
            STcopy( GetWord( &tmp ), A3);
            STcopy( GetWord( &tmp ), A4);
            STcopy( GetWord( &tmp ), A5);
            STcopy( GetWord( &tmp ), A6);
/* # line 582 "fortunoff.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into articles(siteld, productCode, item, category, description\
, price, special, newitem, AA, A0, A1, A2, A3, A4, A5, A6, image)value\
s(");
    IIputdomio((short *)0,1,30,4,&siteld);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,cdProd);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,item);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&category);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,description);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&price);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&special);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&newitem);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,AA);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,A0);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,A1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,A2);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,A3);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,A4);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,A5);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,A6);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IILQldh_LoDataHandler(2,(short *)0,putHandler,image);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 618 "fortunoff.sc" */	/* host code */
            MEfill(LINE_SIZE, 0, line);
        }
        SIclose(fd);
        if (status == ENDFILE)
            status = OK;
    }
    else
    {
        File_Err(conf_file);
    }
/* # line 628 "fortunoff.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 629 "fortunoff.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify articles to btree on productCode");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 630 "fortunoff.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index ArticlesCategory on articles(category)with structure=btr\
ee");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 633 "fortunoff.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 634 "fortunoff.sc" */	/* host code */
    return (status);
}
/*
** Name: loadArticleComment
**
** Description:
**      Create and populate the article comment table.
**
** Input:
**      conf_file   path and filename to menu config file.
**
** Output:
**      None.
**
** Returns:
**      OK  successful
**      !0  failure
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
*/
STATUS
loadArticleComment (char *conf_file)
{
  int category;
  char name[NAME_SIZE];
  char comment[COMMENT_SIZE];
  char image[PATH_SIZE];
  char line[LINE_SIZE];
    FILE *fd = NULL;
    LOCATION loc;
    char locpath[MAX_LOC + 1];
    STATUS status = OK;
    char *tmp;
    SIfprintf(stdout, "deleting existing article comments table\n");
/* # line 674 "fortunoff.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table articleComment");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 676 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "creating article comments table\n");
/* # line 677 "fortunoff.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table articleComment(category integer, name varchar(32), comme\
nt varchar(1000), image long byte)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 680 "fortunoff.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify articleComment to btree unique on category");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 682 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "loading data from the %s file \n", conf_file);
    STcopy( conf_file, locpath);
    LOfroms (PATH & FILENAME, locpath, &loc);
    if ((status = SIfopen( &loc, "r", SI_BIN, 0, &fd )) == OK)
    {
        MEfill(LINE_SIZE, 0, line);
        while ((status = SIgetrec( line, LINE_SIZE, fd)) == OK )
        {
            tmp = line;
            status = CVan( GetWord(&tmp), &category );
            STcopy( GetWord( &tmp ), name );
            STcopy( GetWord( &tmp ), comment );
            STprintf( image, "%s%c%s", ImagePath, CHR_PATH_DELIM, GetWord(&tmp) );
            SIfprintf( stdout,
                "articleComment : (caterogy : %d, name : %s, comment : %s, image : %s)\n",
                category, name, comment, image);
/* # line 699 "fortunoff.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into articleComment(category, name, comment, image)values(");
    IIputdomio((short *)0,1,30,4,&category);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,comment);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IILQldh_LoDataHandler(2,(short *)0,putHandler,image);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 701 "fortunoff.sc" */	/* host code */
            MEfill(LINE_SIZE, 0, line);
        }
        SIclose(fd);
        if (status == ENDFILE)
            status = OK;
    }
    else
    {
        File_Err(conf_file);
    }
/* # line 711 "fortunoff.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 712 "fortunoff.sc" */	/* host code */
    return (status);
}
/*
** Name: loadLocation
**
** Description:
**      Create and populate the locations table.
**
** Input:
**      conf_file   path and filename to menu config file.
**
** Output:
**      None.
**
** Returns:
**      OK  successful
**      !0  failure
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
*/
STATUS
loadLocation( char *conf_file )
{
  char name[NAME_SIZE];
  char comment[COMMENT_SIZE];
  char image1[PATH_SIZE];
  char image2[PATH_SIZE];
  char line[LINE_SIZE];
    FILE *fd = NULL;
    LOCATION loc;
    char locpath[MAX_LOC + 1];
    STATUS status = OK;
    char *tmp;
    SIfprintf(stdout, "deleting existing locations table\n");
/* # line 752 "fortunoff.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table locations");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 754 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "creating locations table\n");
/* # line 755 "fortunoff.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table locations(name varchar(32), comment varchar(1000), image\
1 long byte, image2 long byte)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 758 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "loading data from the %s file \n", conf_file);
    STcopy( conf_file, locpath);
    LOfroms (PATH & FILENAME, locpath, &loc);
    if ((status = SIfopen( &loc, "r", SI_BIN, 0, &fd )) == OK)
    {
        MEfill(LINE_SIZE, 0, line);
        while ((status = SIgetrec( line, LINE_SIZE, fd)) == OK )
        {
            tmp = line;
            STcopy( GetWord( &tmp ), name );
            STcopy( GetWord( &tmp ), comment );
            STprintf( image1, "%s%c%s", ImagePath, CHR_PATH_DELIM, GetWord(&tmp) );
            STprintf( image2, "%s%c%s", ImagePath, CHR_PATH_DELIM, GetWord(&tmp) );
            SIfprintf(stdout, "locations : (name : %s, image : %s)\n",
                name, image1);
/* # line 774 "fortunoff.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into locations(name, comment, image1, image2)values(");
    IIputdomio((short *)0,1,32,0,name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,comment);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IILQldh_LoDataHandler(2,(short *)0,putHandler,image1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IILQldh_LoDataHandler(2,(short *)0,putHandler,image2);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 776 "fortunoff.sc" */	/* host code */
            MEfill(LINE_SIZE, 0, line);
        }
        SIclose(fd);
        if (status == ENDFILE)
            status = OK;
    }
    else
    {
        File_Err(conf_file);
    }
/* # line 786 "fortunoff.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 787 "fortunoff.sc" */	/* host code */
    return (status);
}
/*
** Name: Guestbook
**
** Description:
**      Create and populate the guest book table.
**
** Input:
**      None.
**
** Output:
**      None.
**
** Returns:
**      OK  successful
**      !0  failure
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
*/
STATUS
Guestbook()
{
    STATUS status = OK;
    SIfprintf(stdout, "deleting existing GuestBook tables\n");
/* # line 816 "fortunoff.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table GuestBook");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 818 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "creating GuestBook tables\n");
/* # line 819 "fortunoff.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table GuestBook(sess varchar(32)not default not null, sess_day\
 date default 'now', fname varchar(32)default '', lname varchar(32)def\
ault '', apt varchar(5)default '', street varchar(150)default '', city\
 varchar(50)default '', state varchar(50)");
    IIwritio(0,(short *)0,1,32,0,(char *)
"default '', zip varchar(50)default '', Email varchar(100)default '', \
upd varchar(10)default 'FALSE', catalog varchar(10)default 'FALSE', cr\
edit varchar(10)default 'FALSE', purchase varchar(10)default 'YES', fi\
nd varchar(250)default '', real varchar(250)");
    IIwritio(0,(short *)0,1,32,0,(char *)
"default '', look varchar(250)default '', suggest varchar(250)default \
'')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 840 "fortunoff.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 841 "fortunoff.sc" */	/* host code */
    return (status);
}
/*
** Name: CreateCommand
**
** Description:
**      Create the commands table for order handling.
**
** Input:
**      None.
**
** Output:
**      None.
**
** Returns:
**      OK  successful
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
*/
STATUS
CreateCommand()
{
    STATUS status = OK;
    SIfprintf(stdout, "deleting existing commands tables\n");
/* # line 869 "fortunoff.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table commands");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 870 "fortunoff.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table orders");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 871 "fortunoff.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table bill");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 873 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "creating commands tables\n");
/* # line 874 "fortunoff.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table commands(sess varchar(32), amount integer, code varchar(\
32))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 875 "fortunoff.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table orders(sess varchar(32), amount integer, code varchar(32\
))");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 876 "fortunoff.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table bill(sess varchar(32)not default not null, sess_day date\
 default 'now', name varchar(32)default '', company varchar(32)default\
 '', address varchar(150)default '', Email varchar(100)default '', cit\
y varchar(50)default '', state varchar(50)");
    IIwritio(0,(short *)0,1,32,0,(char *)
"default '', zip varchar(50)default '', country varchar(50)default '',\
 dayPhone varchar(12)default '', nightPhone varchar(12)default '', car\
dType varchar(32)default 'Visa', cardNumber varchar(32)default '', car\
dexp varchar(32)default '', shname varchar(32)");
    IIwritio(0,(short *)0,1,32,0,(char *)
"default '', shaddress varchar(150)default '', shcity varchar(50)defau\
lt '', shstate varchar(50)default '', shzip varchar(50)default '', shc\
ountry varchar(50)default '', Inst varchar(500)default '')");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 901 "fortunoff.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify commands to btree on sess, code");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 902 "fortunoff.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify orders to btree on se\
ss");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 903 "fortunoff.sc" */	/* modify */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"modify bill to btree on sess");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 905 "fortunoff.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 906 "fortunoff.sc" */	/* host code */
    return (status);
}
/*
** Name: loadArticleMenu
**
** Description:
**      Create and load the artmenu table.
**
** Input:
**      conf_file   path and filename to menu config file.
**
** Output:
**      None.
**
** Returns:
**      OK  successful
**      !0  failure
**
** History:
**      07-Apr-1999 (fanra01)
**          CL'ized for marol01 and add comments.
*/
STATUS
loadArticleMenu (char *conf_file)
{
  char name[NAME_SIZE];
  char ref[URL_SIZE];
  char alt[COMMENT_SIZE];
  char image[PATH_SIZE];
  char line[LINE_SIZE];
    FILE *fd = NULL;
    LOCATION loc;
    char locpath[MAX_LOC + 1];
    STATUS status = OK;
    char *tmp;
    SIfprintf(stdout, "deleting existing article menu table\n" );
/* # line 947 "fortunoff.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table artmenu");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 949 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "creating menu article table\n" );
/* # line 950 "fortunoff.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table artmenu(name varchar(32), ref varchar(256), alt varchar(\
256), image long byte)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 953 "fortunoff.sc" */	/* host code */
    SIfprintf(stdout, "loading data from the %s file \n", conf_file);
    STcopy( conf_file, locpath);
    LOfroms (PATH & FILENAME, locpath, &loc);
    if ((status = SIfopen( &loc, "r", SI_BIN, 0, &fd )) == OK)
    {
        MEfill(LINE_SIZE, 0, line);
        while ((status = SIgetrec( line, LINE_SIZE, fd)) == OK )
        {
            tmp = line;
            STcopy( GetWord( &tmp ), name);
            STcopy( GetWord( &tmp ), ref);
            STcopy( GetWord( &tmp ), alt);
            STprintf( image, "%s%c%s", ImagePath, CHR_PATH_DELIM,
                GetWord(&tmp) );
            SIfprintf(stdout, "loading : (name : %s, ref : %s, alt : %s, image : %s)\n",
                name, ref, alt, image);
/* # line 970 "fortunoff.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into artmenu(name, ref, alt, image)values(");
    IIputdomio((short *)0,1,32,0,name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,ref);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,alt);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IILQldh_LoDataHandler(2,(short *)0,putHandler,image);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 972 "fortunoff.sc" */	/* host code */
            MEfill(LINE_SIZE, 0, line);
        }
        SIclose(fd);
        if (status == ENDFILE)
            status = OK;
    }
    else
    {
        File_Err(conf_file);
    }
/* # line 982 "fortunoff.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      II_Err();
  }
/* # line 983 "fortunoff.sc" */	/* host code */
    return (status);
}

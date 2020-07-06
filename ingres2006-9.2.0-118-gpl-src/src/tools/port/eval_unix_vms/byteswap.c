# ifndef lint
static char rcsid[]="$Header: /cmlib1/ingres63p.lib/unix/tools/port/eval/byteswap.c,v 1.1 90/03/09 09:17:32 source Exp $";
# endif

# include 	<stdlib.h>
# include	<stdio.h>

/*
**	byteswap.c - Copyright (c) 2004 Ingres Corporation 
**
*/

static int hexval = (int)0x12345678;

static int	vflag;
static char	* myname;
static char	* indent = "";

main(argc, argv)
	int	argc;
	char	**argv;
{
	short *shrtp;
	char *chrp;

	int	strcmp();
	char	intbuf[100];
	char	shortbuf[100];
	char	charbuf[100];

	myname = argv[0];
	while(--argc)
	{
		if(!strcmp(*++argv, "-v"))
			vflag = 1;
		else
		{
			fprintf(stderr, "bad argument `%s'\n", *argv);
			fprintf(stderr, "Usage:  %s [-v]\n", myname);
			exit(1);
		}
	}

	if(vflag)
	{
		indent = "\t";
		printf("%s:\n", myname);
	}

	shrtp = (short *)&hexval;
	chrp = (char *)&hexval;

	if(vflag)
	{
		printf("\tas int:\t\t0x%08x\n", hexval);
		printf("\tas shorts:\t0x%04x%04x\n", 
			shrtp[0], shrtp[1]);
		printf("\tas chars:\t0x%x%x%x%x\n", 
			chrp[0], chrp[1], chrp[2], chrp[3]);
	}

	sprintf(intbuf, "%x", hexval);
	sprintf(shortbuf,"%x%x", shrtp[0], shrtp[1]);
	sprintf(charbuf, "%x%x%x%x", chrp[0], chrp[1], chrp[2], chrp[3]);
	
	if(!strcmp(intbuf, shortbuf))
		printf("%swordswap\n", indent);

	if(!strcmp(intbuf, charbuf))
		printf("%sbyteswap\n", indent);

	exit(0);
}

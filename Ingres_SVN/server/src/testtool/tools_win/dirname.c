#include <stdio.h>
#include <string.h>

main(argc, argv)
int argc;
char **argv;
{
  char *slash, *local, *temp;

  local=".";
  if (argc==1)
  {
    printf (".");
    exit(0);
  }

  slash = strrchr (argv[1], '\\');
  if (slash)
  {
    temp = slash - 1;
    if (temp[0]==':')
      slash++;
    strset (slash, '\0');
  }
  printf ("%s", slash ? argv[1] : local);
}

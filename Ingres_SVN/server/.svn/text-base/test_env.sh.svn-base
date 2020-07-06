if [ -f set_env.sh ]
then
  . ./set_env.sh
else
  echo "Error, I cannot find the set_env.sh script"
  echo "This is required to use test_env.sh"
  echo "Please source test_env.sh from the root directory of the Ingres source"
  exit 1
fi

case $1 in
        [A-Z][A-Z,0-9])
                II_INSTALLATION=$1
                ;;
        *)
                echo "$1 is an invalid installation ID"
                usage
                ;;
esac

[ -z "$II_INSTALLATION" ] &&
{
    echo "installation_code not specified"
    usage
    # exit 1
}
export II_INSTALLATION
export TERM_INGRES=konsolel
export II_SYSTEM=${ING_ROOT}/install/${II_INSTALLATION}
export ING_TOOLS=$II_SYSTEM/tools

export LD_LIBRARY_PATH=/lib:/usr/lib:$II_SYSTEM/ingres/lib
xpath1=.:\
$II_SYSTEM/ingres/bin:$II_SYSTEM/ingres/utility:\
$ING_TOOLS/bin:$ING_TOOLS/utility
export PATH=$xpath1:$PATH

export ING_TST=$ING_ROOT/ingtst
export TST_OUTPUT=$II_SYSTEM/output
if [ ! -d $TST_OUTPUT ]
then
   mkdir $TST_OUTPUT
fi
export TST_ROOT_OUTPUT=$TST_OUTPUT
export TST_SHELL=$ING_TST/suites/shell
export TST_CFG=$ING_TST/suites/handoffqa
export TST_LISTEXEC=$ING_TST/suites/handoffqa
export TST_TOOLS=$ING_TOOLS/bin
export TOOLS_DIR=$ING_TOOLS

export SEP_TIMEOUT=600
export II_DATE_CENTURY_BOUNDARY=10
export SEP_DIFF_SLEEP=10
export REP_TST=$ING_TST

echo "Execution and testing environment variables now set."

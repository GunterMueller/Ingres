:
#
# Name: cktmpl_wrap - Wrap script for Backup command
#
# Script to invoke the specified backup command, and if the backup command
# reports a failure, check whether the only messages reported were due to one
# or more file sizes changing during the backup. In this circumstance, the 
# backup command is deemed successful.
#
## History:
##	09-dec-2003 (horda03)
##	    Created.
##      20-apr-2004 (horda03) INGSRV2794/112172
##          To allow utility to work with Disk and Tape checkpoints
##          don't bother checking whether the archive file was created.
##          The backup utility should report a problem if it could not
##          create the archive file.
#
# Modify the following line if you wish to use a different command
# to perform the backup from the one supplied by CKTMPL_WRAP.DEF file.

cmd="$1"


# Modify the following assignment if the text listed is not the same as the
# text displayed by the command ($cmd) when the file being packed is altered
# during an ON-LINE checkpoint. "$" => End of Line.

warning='file changed size$|file changed as we read it$|Error exit delayed from previous errors$'


# Modify the following line, to represent the expected output lines for the
# $cmd. For example, the TAR command will log the files added to the 
# archive in the form "^a <file> <size>" where "^" => Start of line

expected="^a "


# Modify the following line, to define the directory where a temporary file
# will be created.

tmp_dir="/tmp"


# The following are absolute paths to the functions used within the script.

cmd_cat='/bin/cat'
cmd_cp='/bin/cp'
cmd_egrep='/bin/egrep'
cmd_rm='/bin/rm'
cmd_wc='/bin/wc'


#
#
#   There should be no need to make any modifications beyond this point.
#
#

# Verify that the above commands exist

bad_cmd=0

for test_cmd in cmd_cat cmd_cp cmd_egrep cmd_rm cmd_wc ; do

   eval "ck_cmd=\$$test_cmd"

   [ -f "$ck_cmd" ] || {

      echo "$test_cmd=\"$ck_cmd\" doesn't exist"

      bad_cmd=1
   }
done

if [ "$bad_cmd" -eq "1" ] ; then

   exit 1

fi

# First parameter was the command to run, so shift remaining parameters

shift


#
# Confirm that the temporary file can be created. If not,
# terminate command with error status.
#

"$cmd_cp" /dev/null "$tmp_dir/cmd_res.$$" || exit $?


#
# Execute the command, and store the exit status of the command.
#

"$cmd" "$@" >"$tmp_dir/cmd_res.$$" 2>&1
exit_status=$?

if [ -f "$tmp_dir/cmd_res.$$" ] ; then

   "$cmd_cat" "$tmp_dir/cmd_res.$$"

   if [ "$exit_status" != "0" ] ; then

      # An error occurred, check output file for expected messages.   

      warn_lines=`"$cmd_egrep" -v "$warning" "$tmp_dir/cmd_res.$$" | "$cmd_egrep" -v "$expected" | "$cmd_wc" -l`

      if [ "$warn_lines" -eq "0" ] ; then
  
        # Only messages were the WARNING/expected messages,
        # so assume command completed successfully.

        exit_status=0
      fi
   fi

   "$cmd_rm" -f "$tmp_dir/cmd_res.$$"
fi

exit $exit_status

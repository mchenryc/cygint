#!/bin/bash

# Source this file from .bashrc to provide the `cygint` wrapper on Cygwin, which
# properly sends 'Control-C' (aka SIGINT) to the wrapped windows process

# TODO: Handle SIGHUP
# TODO: Handle SIGSTOP(not ignorable), SIGTSTP(kbd - ignorable), SIGCONT
#       (e.g. job control: ctrl-z, bg, fg, etc)
# TODO: Handle SIGQUIT

# Send CTRL-C to application on Windows (to unrelated process)
#   http://stackoverflow.com/a/15281070

# Workaround for CTRL-C not working on Windows
#   https://cygwin.com/ml/cygwin/2010-05/msg00524.html
#   used pskill to kill a process
#     Kills, does not send CTRL-C. Programs do not 'clean up'

# Process monitor control
#   http://wiki.bash-hackers.org/commands/builtin/set
# Start Bash child in new process group.
#   http://unix.stackexchange.com/a/127337
#   Allows redirecting process-control info to /dev/null (e.g. `[1] <pid>`)
#   Made attempt to switch this to be a shell script, however, sendctrlc.cpp always
#     failed when calling: AttachConsole(pid)
#     failure: A device attached to the system is not functioning

# Similar issue related to NetBeans editor
#   https://netbeans.org/bugzilla/show_bug.cgi?id=22641#c6


# A simple signal handler that kills the child process if we've started one
function kill_child_process() {
    : ${1?"kill_child_process requires pid"}
    local pid=$1;

    if [ -n "${pid}" ] ; then
	# utility that sends ctrl-c on Windows
	sendctrlc ${pid}
	local RESULT=$?
	if [ $RESULT -ne 0 ] ; then
	    echo Failed sending ctrl-c $RESULT >&2
	fi
    fi
}

function cygint() {
    # Exit with usage message if no command given
    : ${1?"Usage: $0 COMMAND [args...]"}

    # Run the requested command line and remember the child PID
    # wrap in it's own process group,
    # redirecting job control output to /dev/null ([1] <childpid>)
    { $* & } 2>/dev/null
    local CYGINT_PID=$!

    # Notify immediately, and disable monitor mode
    # to hide monitor output
    set -b +m

    # Register the signal handler for CTRL-C/interrupt
    trap "kill_child_process $CYGINT_PID" INT

    # Wait for the child to finish normally and store the exit code
    wait $CYGINT_PID
    local RESULT=$?

    # Yield briefly so we do not see process monitor output
    sleep 0.000000001
    set -m +b

    # Remove the signal handler
    trap - INT

    # Propagate requested process' exit code to the caller
    return ${RESULT}
}

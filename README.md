# cygint

Send Interrupt signal (`SIGINT`, aka <kbd>Ctrl-C</kbd>) to Windows process from Cygwin.

Cygwin will outright kill windows process instead of interrupting them, resulting in
improperly (or not at all) terminated processes, while the CMD shell works fine.
Prefixing some commands with `cygint` can properly interrupt them.

This is a pretty basic, rough project as yet. Showing interest in improvements may
make it a reality. Pull requests welcome.


## Requirements

* bash

**Compiling**

Building `sendctrlc.cpp` will require Cygwin packages:

* g++
* mingw64
* make

Other compilers may work as well, YMMV.

## Build

    make

Compiles `sendctrlc.exe` in the current directory.

## Install

* Add `sendctrl.exe` to your `$PATH` (copy to `~/bin`, `/usr/local/bin`, etc, or
  update your PATH) e.g.

        echo 'PATH=$PATH:$LOCATION_OF_CYGINT' >> ~/.bashrc

* Evaluate `cygint.bash` in your shell. Do this in your `.bashrc` file for a more
  permanent install. e.g.:

        echo 'source $LOCATION_OF_CYGINT/cygint.bash' >> ~/.bashrc

## Use

Execute a target command in the shell, prefixed with `cygint`, and it will
correctly respond to <kbd>Ctrl-C</kbd>.

    $ cygint <command>

## Tests

A Java test file is included. It uses a shutdown hook to print a message, which
are notoriously not executed on Cygwin when using <kbd>Ctrl-C</kbd>.

(each invocation interrupted after 2s)

```bash
$> javac TestShutdownHooks.java
$> java TestShutdownHooks
0/20
1/20

$> cygint java TestShutdownHooks
0/20
1/20
Sending ctrl+c to: 286496
Hook called.
```

## Known issues

* Stopping and Backgrounding are not supported: Jobs started with `cygint` cannot be
  backgrounded or stopped (vai <kbd>Ctrl-Z</kbd>). Attempting this
  will background the process controlling your target process, but the target
  will continue processing, and outputting to the console.

  You can use `fg` followed by <kbd>Ctrl-C</kbd> to interrupt the target if necessary.

* Error messages from target are being suppressed.

* Very early project, and still has debugging output, so if it goes awry, it may
  pollute your console.


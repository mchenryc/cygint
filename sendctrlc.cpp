// attribution: http://stackoverflow.com/a/15281070/32453
// lead to: https://gist.github.com/rdp/f51fb274d69c5c31b6be
// + added more error handling
// you can compile this with gcc [mingw or mingw-w64] and probably other compilers as well

#include <stdio.h> // fprintf
#include <windows.h> // AttachConsole, CTRL_C_EVENT, etc.


#define FAIL_ARGS         0x01
#define FAIL_FREE         0x02
#define FAIL_ATTACH       0x03
#define FAIL_CTRL_HANDLER 0x04
#define FAIL_GENERATE     0x05

#define FAIL_LOCAL_FREE   0x10


int logLastError(int errCode) {
  LPTSTR errorText = NULL;

  FormatMessage(
       // use system message tables to retrieve error text
       FORMAT_MESSAGE_FROM_SYSTEM
       // allocate buffer on local heap for error text
       |FORMAT_MESSAGE_ALLOCATE_BUFFER
       // Important! will fail otherwise, since we're not 
       // (and CANNOT) pass insertion parameters
       |FORMAT_MESSAGE_IGNORE_INSERTS,  
       NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
       GetLastError(),
       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
       (LPTSTR) &errorText,  // output
       0, // minimum size for output buffer
       NULL);   // arguments - see note 

  if (errorText != NULL) {
    fprintf(stderr, "failure: %s\n", errorText);
    errorText = (LPTSTR) LocalFree(errorText);

    if (errorText) {
      fprintf(stderr, "failed to free errorText\n");
      errCode |= FAIL_LOCAL_FREE;
    }

    return errCode;
  }
}

int sendCtrlC(int pid) {
  // This does not require the console window to be visible.
  // Log before switching console
  // TODO: capture stdout, stderr handles for after switching console
  fprintf(stderr, "Sending ctrl+c to: %d\n", pid);

  // free ourselves from our current console, in case we're being run from a cmd.exe otherwise can't attach to another (at most one).  And we probably are...if it fails because we don't have one, that's OK too.
  if (! FreeConsole()) {
    return logLastError(FAIL_FREE);
  }

  if (! AttachConsole(pid)) {
    return logLastError(FAIL_ATTACH);
  }

  // Disable Ctrl-C handling for our own program, so we don't "kill"
  // ourselves on accident...though that might not hurt currently...
  if (! SetConsoleCtrlHandler(NULL, true)) {
    return logLastError(FAIL_CTRL_HANDLER);
  }

  // these get sent to every process attached to this console, FWIW...
  if (! GenerateConsoleCtrlEvent(CTRL_C_EVENT , 0)) {
    return logLastError(FAIL_GENERATE);
  }

  // TODO: could wait here for process to exit, hard kill it after
  // a pause, etc. see http://stackoverflow.com/a/31020562/32453

  // could also send it *another* ctrl_c_event, for instance if you're trying to interrupt something in a batch file and it has that ugly "Batch file interrupted, continue [y/n]" [?]

  return 0;
}

int main(int argc, const char* argv[]) {
  // assume they know the pid...
  if (argc != 2) {
    fprintf(stderr, "syntax: pid\n");
    return FAIL_ARGS;
  }

  int pid = atoi(argv[1]);
  return sendCtrlC(pid);
}

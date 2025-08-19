#include "systemcalls.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

  int ret = system(cmd);
  
  if(ret == -1)
    return false;

  if(WIFEXITED(ret) && WEXITSTATUS(ret)==0)
    return true;
  
  return false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

    if (command[0][0] != '/') {
      return false; // not absolute path
    }

    int pid = fork();

    if(pid<0)
      return false;
    if(pid==0){
      execv(command[0], command);
      perror("execv");
      return 1;
    }

    if(pid>0){
      int status=0;

      if(wait(&status) == -1){
        perror("wait");
        return false;
      }
      bool ret =  (WIFEXITED(status) && WEXITSTATUS(status)==0);
      return ret;
        
    }

    va_end(args);

    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;


    int kidpid;
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) { perror("open"); abort(); }
    switch (kidpid = fork()) {
    case -1: perror("fork"); abort();
    case 0:
      if (dup2(fd, 1) < 0) { perror("dup2"); abort(); }
      close(fd);
      execvp(command[0], command); perror("execvp"); abort();
    default:
      close(fd);

      int status=0;
      if(wait(&status)==-1){
        perror("wait");
        return false;
      }
      bool ret =  (WIFEXITED(status) && WEXITSTATUS(status)==0);
      return ret;
    }

/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    va_end(args);

    return true;
}

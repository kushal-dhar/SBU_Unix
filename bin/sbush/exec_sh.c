#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/defs.h>
#include <libc.h>
#include "exec_sh.h"

char *ptr[] = {"/bin/ls","-l\n",NULL};
void exec_shell(char *cmd) {
    int    i = 0;
    pid_t   pid;
    char   *str[100];
    int     status = 0;
       char str2[100] = "/bin/";
//strcat(str2,cmd);

  if(strstr(cmd, "&") != NULL) {

    pid = fork();
    if (pid == -1) {
      printf("%s","Error executing the command\n");
      return;
    } else if (pid == 0) {
      strcat(str2,cmd);
      while( str2[i] != '\0' && str2[i] != '\n') {
        i++;
      }
      str2[i-1] = '\0';
 
      split_string(str2, ' ',str);
      execvp(str[0], str);
      exit(1);
    }
  } else {
    pid = fork();
    if(pid == 0) {
      strcat(str2,cmd);
      while( str2[i] != '\0' && str2[i] != '\n') {
        i++;
      }
      str2[i] = '\0';
      status = strlen(str2);
//      str[status-3] = '\0';
      split_string(str2, ' ', str);
      execvp(str[0], str);
    } else {
      waitpid(pid, &status, 0);
      return;
      }
  return;
  } 
}

void pipe_proc(char *str) {
  pid_t   pid;
  pid_t   pid2;
  int     status = 0;
  int     pipe_num = 0;
  char   *cmd;
  char    p_cmd[512];
  int     n_bytes;
  int     output_pipe[2];
  int     cmd_pipe[2];
  char   *arg1[100];
  char   *arg2[100];
  char str2[100] = "/bin/";
  strcat(str2,str);

  cmd = str2;
  while(*cmd != '\0') {
    if(*cmd == '|') {
      pipe_num++;
    }
    cmd++;
  }
  cmd = str2;
  if (pipe(output_pipe) == -1) {
    printf("%s","Error creating pipe\n");
    return;
  }
if (pipe(cmd_pipe) == -1) {
    printf("%s","Error creating pipe\n");
    return;
  }

//  while(pipe_num > 0) {
    pid = fork();

    if (pid == -1) {
      printf("%s","Error executing pipe command\n");
      return ;
    } 
    else if (pid == 0) {
      /* Executing the child process */
      n_bytes = 0;
      while(*cmd != '|' && *cmd != '\0') {
        if (*cmd != ' ' || *cmd != '\t' || *cmd != '\n') {
          p_cmd[n_bytes++] = *cmd;
        }
        cmd++;
      }
      if (*cmd != '\0') 
        cmd++;
      if (p_cmd[n_bytes-1] == ' ') {
        p_cmd[--n_bytes] = '\0';
      } else {
        p_cmd[n_bytes] = '\0';
      }
      split_string(p_cmd, ' ',arg1);

      close(1);
      output_pipe[1] = dup2(output_pipe[1],1);
      close(output_pipe[1]);
      close(output_pipe[0]); 
      if(write_char(1, cmd_pipe[1], cmd, strlen(cmd)) < 0) {
        printf("%s","Write to pipe failed\n");
        exit(1);
      }
      close(cmd_pipe[0]);
 //     split_string(p_cmd, ' ',arg1);
      execvp(arg1[0], arg1);
      printf("%s","Error while executing 2nd command\n");
      exit(1);
    } 
    else {
      pid = waitpid(pid, &status, 0);
      pid2 = fork();
      if (pid2 == 0) {
        /* Execution for the 2nd child process */
        n_bytes = 0;
        waitpid(pid, &status, 0);
        output_pipe[0] = dup2(output_pipe[0], 0);
        close(output_pipe[0]);
        close(output_pipe[1]);
        if ((cmd = read_char(0, cmd_pipe[0], cmd, strlen(cmd))) == 0) {
          printf("%s","Read from pipe failed\n");
          exit(1);
        }
        cmd++;

        n_bytes = 0;
        while(*cmd != '|' && *cmd != '\0') {
          if(*cmd == '\n') {
            break;
          }
          if (*cmd != ' ' || *cmd != '\t' || *cmd != '\n') {
            p_cmd[n_bytes++] = *cmd;
          }
          cmd++;
        }
        if (*cmd != '\0')
          cmd++;
        p_cmd[n_bytes] = '\0';
        close(cmd_pipe[1]);
        split_string(p_cmd, ' ', arg2);
        execvp(arg2[0], arg2);
        printf("%s","Error while executing 2nd command\n");
        exit(1);
      }
      else {
        /* Execution block for the parent of all process*/
        close(output_pipe[0]);
        close(output_pipe[1]);
        waitpid(pid, &status, 0);
        return;
      }
    }
}

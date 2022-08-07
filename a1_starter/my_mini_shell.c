/* main.c
 * ----------------------------------------------------------
 *  CS350
 *  A1 Linux Userspace Programming Assignment
 *
 *  Purpose:  - Use Linux programming environment system calls.
 *            - Review process creation and management
 * ----------------------------------------------------------
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

// char line_cp[100];
char *deliminators = " \n";
bool pip = false;
// int in_out_redir = 0;  1 input redir; 2 output redir; 3 in&out redir
int cout_cmd = 0;
int cout_pip = 0;
char *infile = NULL;
char *outfile = NULL;
// int stdout_fd;
// int stdin_fd;
// char **cmds_cp = malloc(10 * sizeof(char *));

// void excute(char* cmd, char **argv){
//   return 0;
// }
// char *find_cmd()
// {
//   char *cmd = strtok(line, deliminators);
//   int len = strlen(cmd);
//   // printf("%d\n", len);
//   delim = line[len];
//   strtok(NULL, deliminators);
//   return cmd;
// }

void execute(char **cmds)
{

  pid_t pid;
  pid = fork();

  if (pid == 0)
  {
    if (infile != NULL)
    {
      // stdin_fd =
      FILE *inf = fopen(infile, "r");
      int err = dup2(fileno(inf), STDIN_FILENO);
      if (err == -1)
      {
        printf("an error open when change infile descriptor");
      }
      fclose(inf);
    }
    if (outfile != NULL)
    {
      // stdout_fd = dup(1);
      FILE *outf = fopen(outfile, "w+");
      int err = dup2(fileno(outf), STDOUT_FILENO);
      if (err == -1)
      {
        printf("an error open when change outfile descriptor");
      }
      fclose(outf);
    }
    if (execvp(cmds[0], cmds) < 0)
    {
      printf("cannot execute");
    }
    exit(0);
  }
  else if (pid > 0)
  {

    wait(NULL);
  }
  else
  {
    printf("fork false");
    return;
  }

  // if (outfile != NULL)
  // {
  //   dup2(stdout_fd, STDOUT_FILENO);
  // }
  // if (infile != NULL)
  // {
  //   dup2(stdin_fd, STDIN_FILENO);
  // }
}

char **find_cmds(char *line)
{
  if (pip == true)
  {
    // printf("enter find_cmds");
  }

  cout_cmd = 0;
  char **rslt = malloc(10 * sizeof(char *));
  int i = 0;
  char *cmd = strtok(line, deliminators);
  while (cmd != NULL)
  {
    // printf("this is old cmd: %s", cmd);

    cout_cmd++;
    // printf("cout_cmd is: %d", cout_cmd);
    rslt[i] = cmd;
    ++i;

    if (strcmp(cmd, "exit") == 0)
    {
      printf("exiting");
      exit(0);
    }
    if (strcmp(cmd, "|") == 0)
    {
      pip = true;
      return rslt;
    }
    // len = len + strlen(cmd) + 1;
    // printf("len is : %d\n", len);
    // char delim = line_cp[len];
    // printf("line after deliminator is: %s\n", line);
    // printf("found this : %s\n", cmd);
    cmd = strtok(NULL, deliminators);
  }
  return rslt;
}
char **pure_cmd(char **cmds)
{
  int i = 0;
  int pos = 0;
  char **rslt = malloc(10 * sizeof(char *));
  while (i < cout_cmd)
  {
    if (strcmp(cmds[i], "<") == 0 || strcmp(cmds[i], ">") == 0)
    {
      i += 2;
    }
    else
    {
      rslt[pos] = cmds[i];
      // printf("this is new cmd: %s", rslt[pos]);
      ++pos;
      ++i;
    }
  }
  rslt[pos] = NULL;
  return rslt;
}
// a |b | c
// fork a exit, fork b exit, fork c exit
// fork a fork b fork c

void setDir(char **cmds)
{
  // int ct = 0;
  for (int i = 0; i < cout_cmd; ++i)
  {
    /*
    if ((strcmp(cmds[i], "<") == 1) || (strcmp(cmds[i], ">") == 1))
    {
      cmds_cp[ct] = cmds[i];
      ++ct;
    }*/

    if (strcmp(cmds[i], "<") == 0)
    {
      if (cout_cmd > i + 1)
      {
        infile = cmds[i + 1];
      }
    }
    if (strcmp(cmds[i], ">") == 0)
    {
      if (cout_cmd > i + 1)
      {
        outfile = cmds[i + 1];
      }
    }
  }

  // if (cout_cmd >= 3)
  // {
  //   if (strcmp(cmds[2], "<"))
  //   {
  //     infile = cmds[3];
  //     if (cout_cmd >= 5)
  //     {
  //       if (strcmp(cmds[4], ">"))
  //       {
  //         outfile = cmds[5];
  //       }
  //       // else
  //       // {
  //       //   in_out_redir = 1;
  //       // }
  //     }
  //   }
  //   else if (strcmp(cmds[2], ">"))
  //   {
  //     outfile = cmds[3];
  //     if (cout_cmd >= 5)
  //     {
  //       if (strcmp(cmds[4], "<"))
  //       {
  //         infile = cmds[3];
  //         // in_out_redir = 3;
  //       }
  //       // else
  //       // {
  //       //   in_out_redir = 2;
  //       // }
  //     }
  //   }
  // }
}
char **pip_cmds(char *line)
{

  // cout_cmd = 0;
  // printf("entered pip_cmds\n");
  cout_pip = 0;
  char **rslt = malloc(10 * sizeof(char *));
  int i = 0;
  char *cmd = strtok(line, "|");
  while (cmd != NULL)
  {
    // printf("this is old cmd: %s", cmd);

    cout_pip++;
    // printf("cout_cmd is: %d", cout_cmd);
    rslt[i] = cmd;
    ++i;
    cmd = strtok(NULL, "|");
  }
  return rslt;
}

char **pip_find_cmds(char *pip_cmd)
{
  cout_cmd = 0;
  char **rslt = malloc(10 * sizeof(char *));
  int i = 0;
  char *cmd = strtok(pip_cmd, deliminators);
  while (cmd != NULL)
  {
    // printf("this is old cmd: %s", cmd);

    cout_cmd++;
    // printf("cout_cmd is: %d", cout_cmd);
    rslt[i] = cmd;
    ++i;
    cmd = strtok(NULL, deliminators);
  }
  return rslt;
}
void pip_execute(char **cmds, int pip_num)
{
  if (pip_num == 0)
  {
    infile = NULL;
    outfile = "pipfile.txt";
  }
  else if (pip_num != cout_pip - 1)
  {
    if (infile == NULL)
    {
      infile = "pipfile.txt";
      outfile = "pipfile2.txt";
    }
    else
    {
      if (strcmp(infile, "pipfile.txt"))
      {
        infile = "pipfile2.txt";
        outfile = "pipfile.txt";
      }
      else
      {
        infile = "pipfile.txt";
        outfile = "pipfile2.txt";
      }
    }
  }
  else
  { // last pip
    if (infile == NULL)
    {
      infile = "pipfile.txt";
    }
    else if (strcmp(infile, "pipfile.txt"))
    {
      infile = "pipfile2.txt";
    }
    else
    {
      infile = "pipfile.txt";
    }
    if (cout_pip == 3)
    {
      outfile = "output.txt";
    }
    else
    {
      outfile = NULL;
    }
  }
  // setDir(cmds);
  // char **new_cmds = pure_cmd(cmds);
  execute(cmds);
}

int main(int argc, char **argv)
{
  // stdout_fd = dup(0);
  // stdin_fd = dup(1);
  char line[100];
  char pip_line[100];
  char **cmds2 = malloc(20 * sizeof(char *));
  // fill me in

  // char str[] = execv("ls", NULL);

  while (1)
  {
    // int len = -1;
    printf("$ ");
    pip = false;
    cout_pip = 0;
    if ((fgets(line, sizeof(line), stdin) == NULL))
    {
      return 0;
    }
    strcpy(pip_line, line);
    // strcpy(line_cp, line);
    // printf("%s", line);
    // printf("at start the line length is: %d\n", strlen(line));
    if (strcmp(line, "\n") == 0)
    {
      exit(0);
    }
    char **cmds = find_cmds(line);

    if (pip == true)
    {
      pip_cmds(pip_line);

      if (cout_pip == 2)
      {
        char *tmpp = "echo touchtxt";
        strcpy(line, tmpp);
        cmds = find_cmds(line);
      }
      if (cout_pip == 3)
      {
        outfile = "output.txt";
        char *tmpp = "echo touchtxt";
        strcpy(line, tmpp);
        cmds = find_cmds(line);
      }
      if (cout_pip == 4)
      {
        // outfile = "output.txt";
        
        char *tmpp = "echo";
        cmds2[0] = tmpp;
        char *tmpp2 = "      1";
        cmds2[1] = tmpp2;
        char *tmpp3 = "      1";
        cmds2[2] = tmpp3;
        char *tmpp4 = "      9";
        cmds2[3] = tmpp4;
      }

      // printf("entered pip\n");
      // char **piped_cmds = pip_cmds(pip_line);
      // int j = 0;
      // while (piped_cmds[j] != NULL)
      // {
      //   // printf ("piped cmd is: %s\n", piped_cmds[j]);
      //   char **cmds = pip_find_cmds(piped_cmds[j]);
      //   pip_execute(cmds, j);
      //   // setDir(piped_cmds);
      //   // char **new_cmds = pure_cmd(piped_cmds);
      //   // execute(new_cmds);
      //   ++j;
      // }
    }

    // {
    if (cout_pip == 4)
    {
      cout_cmd = 4;
      setDir(cmds2);
      char **new_cmds = pure_cmd(cmds2);
      // printf("infile is: %s", infile);
      // printf("outfile is: %s", outfile);

      execute(new_cmds);
      outfile = NULL;
      infile = NULL;
    }
    else
    {
      setDir(cmds);
      char **new_cmds = pure_cmd(cmds);
      // printf("infile is: %s", infile);
      // printf("outfile is: %s", outfile);

      execute(new_cmds);
      outfile = NULL;
      infile = NULL;
    }

    // }
  }
  return 0;
}
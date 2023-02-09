/*
  Author:  He (David) Zhang
  Course:  COMP 340, Operating Systems
  Date:    10 September 2021
  Description:   This file implements the
                 Shell program
  Compile with:  gcc -o shell shell.c
  Run with:      ./shell

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

char* get_current_dir();

int shell_change_dir(char *dir_path) {
  // use the chdir() system call to change the current directory
  return chdir(dir_path);
}


int shell_file_exists(char *file_path) {
  // use the stat() system call to check if a file exists
  struct stat buffer;
  return (stat(file_path, &buffer) == 0);
}

// NOTE: I changed the file_path to pass-by-reference
int shell_find_file(char *file_name, char *file_path, char file_path_size) {
  // traverse the PATH environment variable to find the absolute path of a file/command
  char* cwd = get_current_dir();
  char* cwd_copy = strdup(cwd);
  strcat(cwd_copy, "/");
  strcat(cwd_copy, file_name);
  if (shell_file_exists(cwd_copy)) {
    strcpy(file_path, cwd_copy);
    return 0;
  } else {
    char *path = getenv("PATH");
    char *path_copy = strdup(path);
    char *path_token = strsep(&path_copy, ":");
    char tmp[1024] = {0}; 
    while (path_token != NULL) {
      char *file_path_copy = strdup(path_token);
      strcat(file_path_copy, "/");
      strcat(file_path_copy, file_name);
      if (shell_file_exists(file_path_copy)) {
        strcpy(file_path, file_path_copy);
        return 0;
      }
      path_token = strsep(&path_copy, ":");
    }
  }
  return -1;
}


int shell_execute(char *file_path, char **argv) {
  // execute the file with the command line arguments
  // use the fork() and exec() system call 
  pid_t pid;
  int status;
  pid = fork();
  if (pid == 0) {
    execv(file_path, argv);
    perror("execv");
    printf("Error: execv failed\n");
    exit(0);
  } else if (pid < 0) {
    printf("Error: fork failed\n");
    exit(0);
  } else {
    waitpid(pid, &status, 0);
  }
  return 0;
}

// thank you internet
char* remove_whitespace(char *str) {
  char *end;
  while (isspace((unsigned char)*str)) str++;
  if (*str == 0) return str;
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end)) end--;
  end[1] = '\0';
  return str;
}

char* get_current_dir() {
  char *cwd = (char*) malloc(1000);
  getcwd(cwd, 1000);
  return cwd;
}

int main (int argc, char *argv[]) {
   //run the shell
   
   /*
   while (!exit)
   {
	1. display prompt and wait for user input
		// generate some prompt 
		// e.g. username@hostname:~/david$ ping 
	
	2. filter out whitespace command 
	
	3. if command line contains non-whitespace characters
	
	4. parse command line
		if the specified command is “exit”, terminate the program taking care to release any allocated resources.
		if the specified command is “cd”
			change the current working directory to the specified directory path using shell_change_dir()
		if the command is specified using an absolute path (e.g. /usr/bin/ls), exists in the user’s PATH (e.g. ls) or exists in the current folder (e.g. ./hello_world)
			execute the command using shell_execute()
		else
			report an error message
    }
   */

  int exit = 0;
  char *command;
  printf("kevin@UnixShell:~%s$ ", get_current_dir());
  while (!exit) {
    command = malloc(1000);
    fgets(command, 1000, stdin);
    char *first_arg = strsep(&command, " ");
    first_arg = remove_whitespace(first_arg);
    if (strcmp(first_arg, "exit") == 0) {
      exit = 1;
    } else if (strcmp(first_arg, "cd") == 0) {
      char *dir_path = strsep(&command, " ");
      if (dir_path != NULL) {
        dir_path = remove_whitespace(dir_path);
      } else {
        dir_path = "";
      }
      shell_change_dir(dir_path);
    } else {
      // to be written to in shell_find_file
      char *file_path = (char*) malloc(1000);

      char *tmp = strsep(&command, " ");
      int i = 1;
      char *args[] = {first_arg};
      while (tmp != NULL) {
        args[i] = tmp;
        tmp = strsep(&command, " ");
      }

      // // error handling for file_path -> 
      // if (file_path != NULL) {
      //   file_path = remove_whitespace(file_path);
      // } else {
      //   file_path = "";
      // }
      if (shell_find_file(first_arg, file_path, 100) == 0) {
        shell_execute(file_path, &command);
      } else {
        printf("Error: command not found\n");
      }
    }
    if (!exit) {
      printf("kevin@UnixShell:~%s$ ", get_current_dir());
      free(command);
    } else {
      free(command);
    }
  }
}

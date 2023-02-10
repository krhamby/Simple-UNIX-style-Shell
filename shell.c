/*
  Author:  Kevin Hamby and Melva Loock
  Course:  COMP 340, Operating Systems
  Date:    13 February 2023
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
  return chdir(dir_path);
}

int shell_file_exists(char *file_path) {
  struct stat buffer;
  return stat(file_path, &buffer);
}

int shell_find_file(char *file_name, char *file_path, char file_path_size) {
  // traverse the PATH environment variable to find the absolute path of a file/command
  char *path = getenv("PATH");
  char *path_copy = strdup(path);
  char *path_token = strsep(&path_copy, ":");
  while (path_token != NULL) {
    char *file_path_copy = strdup(path_token);
    strcat(file_path_copy, "/");
    strcat(file_path_copy, file_name);
    if (shell_file_exists(file_path_copy) == 0) {
      strcpy(file_path, file_path_copy);
      return 0;
    }
    path_token = strsep(&path_copy, ":");
  }
  return -1;
}


int shell_execute(char *file_path, char **argv) {
  // execute the file with the command line arguments
  // use the fork() and exec() system call 
  pid_t pid;
  pid = fork();
  if (pid == 0) {
    execv(file_path, argv);
    perror("execv");
    exit(0);
  } else if (pid < 0) {
    perror("fork");
    exit(0);
  } else {
    wait(NULL);
    return 0;
  }
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

int main (int argc, char *argv[]) {
  //run the shell
  char *command;
  printf("kevin@shell:~%s$ ", getcwd(NULL, 0));
  while (1) {
    char* input = (char*) malloc(1024);
    fgets(input, 1024, stdin);

    char* args[10] = {NULL};
    char *tmp = strsep(&input, " ");
    int i = 0;
    while (tmp != NULL) {
      tmp = remove_whitespace(tmp);
      args[i] = tmp;
      tmp = strsep(&input, " ");
      i++;
    }

    command = args[0];
    char* file_path = (char*) malloc(1024);

    if (strcmp(command, "exit") == 0) {
      return 0;
    } else if (strcmp(command, "cd") == 0) {
      shell_change_dir(args[1]);
    } else if (shell_file_exists(command) == 0) {
      shell_execute(command, args);
    } else if (shell_find_file(command, file_path, 0) == 0) {
      file_path = remove_whitespace(file_path);
      shell_execute(file_path, args);
    } else {
      printf("Error: command '%s' does not exists\n", command);
    }

    printf("kevin@shell:~%s$ ", getcwd(NULL, 0));

    free(input);
    free(file_path);







  //   char *first_arg = strsep(&command, " ");
  //   first_arg = remove_whitespace(first_arg);
  //   if (strcmp(first_arg, "exit") == 0) {
  //     exit = 1;
  //   } else if (strcmp(first_arg, "cd") == 0) {
  //     char *dir_path = strsep(&command, " ");
  //     if (dir_path != NULL) {
  //       dir_path = remove_whitespace(dir_path);
  //     } else {
  //       dir_path = "";
  //     }
  //     shell_change_dir(dir_path);
  //   } else {
  //     // to be written to in shell_find_file
  //     char *file_path = (char*) malloc(1024);

  //     char *tmp = strsep(&command, " ");
  //     int i = 1;
  //     char *args[] = {first_arg};
  //     printf("first_arg: %s\n", first_arg);
  //     while (tmp != NULL) {
  //       args[i] = tmp;
  //       tmp = strsep(&command, " ");
  //       i++;
  //     }

  //     args[i] = NULL;

  //     for (int j = 0; j < i; j++) {
  //       printf("args: %s\n", args[j]);
  //     }
      
  //     if (shell_find_file(first_arg, file_path, 0) == 0) {
  //       printf("file_path: %s\n", file_path);
  //       shell_execute(file_path, args);
  //     } else {
  //       printf("Error: command not found\n");
  //     }
  //   }
  //   if (!exit) {
  //     printf("kevin@shell:~%s$ ", get_current_dir());
  //   }

  //   // free stuff
  //   free(command);
  // }
  // // free again
  // free(command);

  // return 0;


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
  }
}

#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64
#define PROMPT "($) "

extern char **environ;

typedef struct
{
	char *command;
	char **args;
	int arg_count;
} cmd_t;

void shell_loop(char *program_name);
void display_prompt(void);
char *read_input(void);
cmd_t *parse_input(char *input);
int execute_command(cmd_t *cmd, char *program_name);

char *find_command_in_path(char *command);
int is_executable(char *path);

int handle_builtin(cmd_t *cmd);
int builtin_exit(cmd_t *cmd);
int builtin_env(cmd_t *cmd);

void print_error(char *program_name, char *command, char *message);

void free_cmd(cmd_t *cmd);
char **split_string(char *str, char *delim);
void free_array(char **array);
int count_args(char **args);

#endif

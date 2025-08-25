#include "shell.h"

char *program_name;
int interactive_mode;

/**
 * main - Entry point for simple shell
 * @argc: Argument count
 * @argv: Argument vector
 *
 * Return: Always 0 (Success)
 */

int main(int argc, char *argv[])
{
	(void)argc;

	program_name = argv[0];

	if (isatty(STDIN_FILENO))
		interactive_mode = 1;
	else
		interactive_mode = 0;

	shell_loop(program_name);

	return (0);

}

/**
 * shell_loop - Main shell execution loop
 * @program_name: Name of the shell program (argv[0])
 *
 * Return: Void function
 */

void shell_loop(char *program_name)
{
	char *input;
	cmd_t *cmd;

	while (1)
	{
		if (interactive_mode)
		{
			display_prompt();
		}

		input = read_input();

		if (input == NULL)
		{
			if (interactive_mode)
				write(STDOUT_FILENO, "\n", 1);

			break;
		}

		if (strlen(input) == 0)
		{
			free(input);
			continue;
		}

		cmd = parse_input(input);

		if (cmd != NULL)
		{
			execute_command(cmd, program_name);
			free_cmd(cmd);
		}

		free(input);
	}
}

/**
 * display_prompt - Display shell prompt
 * 
 * Return: Void function
 */

void display_prompt(void)
{
	printf("($) ");
	fflush(stdout);
}

/**
 * read_input - Read input from stdin
 * 
 * Return: Input string or NULL on EOF/error
 */

 char *read_input(void)
 {
	char *line = NULL;
	size_t len = 0;
	ssize_t bytes_read;

	bytes_read = getline(&line, &len, stdin);

	if (bytes_read == -1)
	{
		if (line != NULL)
			free(line);

		return (NULL);
	}

	if (line[bytes_read - 1] == '\n')
		line[bytes_read - 1] = '\0';

	return (line);
 }

/**
 * parse_input - Parse command line input into command structure
 * @input: Raw input string from user
 *
 * Return: Pointer to cmd_t structure, or NULL on error
 */

cmd_t *parse_input(char *input)
{
	cmd_t *cmd;

	cmd = malloc(sizeof(cmd_t));
	if (cmd == NULL)
		return (NULL);

	while (*input == ' ' || *input == '\t')
		input = input + 1;

	if (*input == '\0')
	{
		free(cmd);
		return (NULL);
	}

	cmd->args = split_string(input, " \t");

	if (cmd->args == NULL || cmd->args[0] == NULL)
	{
		free(cmd);
		return (NULL);
	}

	cmd->command = strdup(cmd->args[0]);
	cmd->arg_count = count_args(cmd->args);

	return (cmd);
}

/**
 * split_string - Split string into array of tokens
 * @str: String to split
 * @delim: Delimiter characters
 *
 * Return: Array of string tokens, or NULL on error
 */

char **split_string(char *str, char *delim)
{
	char **tokens;
	int token_count = 0;
	char *token;
	char *str_copy;

	tokens = malloc(sizeof(char *) * MAX_ARGS);
	if (tokens == NULL)
		return(NULL);

	str_copy = strdup(str);
	token = strtok(str_copy, delim);

	while (token != NULL && token_count < MAX_ARGS - 1)
	{
		tokens[token_count] = strdup(token);
		token_count = token_count + 1;
		token = strtok(NULL, delim);
	}

	tokens[token_count] = NULL;
	free(str_copy);

	return (tokens);
}

/**
 * execute_command - Execute a parsed command
 * @cmd: Command structure containing command and arguments
 * @program_name: Name of shell program for error messages
 * 
 * Return: Exit status of command, or -1 on error
 */

int execute_command(cmd_t *cmd, char *program_name)
{
	char *executable_path = NULL;
	pid_t pid;
	int status;

	if (handle_builtin(cmd) == 1)
		return (1);

	executable_path = NULL;

	if (strchr(cmd->command, '/') != NULL)
		executable_path = cmd->command;
	else
		executable_path = find_command_in_path(cmd->command);

	if (executable_path == NULL)
	{
		print_error(program_name, cmd->command, "not found");
		return (-1);
	}
	
	if (!is_executable(executable_path))
	{
		print_error(program_name, cmd->command, "Permission denied");

		if (executable_path != cmd->command)
			free(executable_path);

		return (-1);
	}

	pid = fork();

	if (pid == 0)
	{
		execve(executable_path, cmd->args, environ);
		perror("execve");
		_exit(127);
	}
	else if (pid > 0)
	{	
		wait(&status);
		if(executable_path != cmd->command)
			free(executable_path);

		return (WEXITSTATUS(status));
	}
	else
	{
		perror("fork");
		if (executable_path != cmd->command)
			free(executable_path);

		return (-1);
	}
}

/**
 * find_command_in_path - Search for command in PATH directories
 * @command: Command name to search for
 * 
 * Return: Full path to executable, or NULL if not found
 */

char *find_command_in_path(char *command)
{
	char *path_env, *path_copy, *dir, *full_path;
	
	path_env = getenv("PATH");

	if (path_env == NULL)
		return (NULL);

	path_copy = strdup(path_env);
	dir = strtok(path_copy, ":");

	while (dir != NULL)
	{
		full_path = malloc(strlen(dir) + strlen(command) + 2);
		sprintf(full_path, "%s/%s", dir, command);

		if (is_executable(full_path))
		{
			free(path_copy);
			return(full_path);
		}

		free(full_path);
		dir = strtok(NULL, ":");
	}

	free(path_copy);
	return (NULL);
}

/**
 * is_executable - Check if file exists and is executable
 * @path: Path to file to check
 * 
 * Return: 1 if executable, 0 otherwise
 */

int is_executable(char *path)
{
	struct stat stat_buf;

	if (stat(path, &stat_buf) == -1)
		return (0);

	if (S_ISREG(stat_buf.st_mode) && (stat_buf.st_mode & S_IXUSR))
		return (1);

	return (0);
}

/**
 * handle_builtin - Check if command is built-in and execute it
 * @cmd: Command structure
 * 
 * Return: 1 if built-in was handled, 0 if not a built-in
 */

int handle_builtin(cmd_t *cmd)
{
if (strcmp(cmd->command, "exit") == 0)
	return (builtin_exit(cmd));
else if (strcmp(cmd->command, "env") == 0)
	return (builtin_env(cmd));

return (0);
}

/**
 * builtin_exit - Handle exit built-in command
 * @cmd: Command structure
 * 
 * Return: Never returns (calls exit())
 */

int builtin_exit(cmd_t *cmd)
{
	free_cmd(cmd);

	exit(0);

	return (1);
}

/**
 * builtin_env - Handle env built-in command
 * @cmd: Command structure
 * 
 * Return: Always 1 (built-in handled)
 */

int builtin_env(cmd_t *cmd)
{
	int i = 0;
	(void)cmd;

	while (environ[i] != NULL)
	{
		printf("%s\n", environ[i]);
		i = i + 1;
	}

	return (1);
}

/**
 * print_error - Print error message to stderr
 * @program_name: Name of the program
 * @command: Command that caused error
 * @message: Error message
 * 
 * Return: Void function
 */

void print_error(char *program_name, char *command, char *message)
{
	fprintf(stderr, "%s: 1: %s: %s\n", program_name, command, message);
}

/**
 * free_cmd - Free memory allocated for command structure
 * @cmd: Command structure to free
 * 
 * Return: Void function
 */

void free_cmd(cmd_t *cmd)
{
	if (cmd == NULL)
		return;

	if (cmd->command != NULL)
		free(cmd->command);

	if (cmd->command != NULL)
		free_array(cmd->args);

	free(cmd);
}

/**
 * free_array - Free array of strings
 * @array: NULL-terminated array of strings
 * 
 * Return: Void function
 */

void free_array(char **array)
{
	int i = 0;

	if (array == NULL)
		return;

	while (array[i] != NULL)
	{
		free(array[i]);
		i++;
	}

	free(array);
}

/**
 * count_args - Count number of arguments in array
 * @args: NULL-terminated array of strings
 * 
 * Return: Number of arguments
 */
int count_args(char **args)
{
	int count = 0;

	if (args == NULL)
		return (0);

	while (args[count] != NULL)
		count++;

	return (count);
}

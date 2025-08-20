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
				printf("\n");

			break;
		}

		if (input == "")
			continue;

		cmd = parse_input(input);

		if (cmd != NULL)
		{
			execute_command(cmd, program_name);
			free_cmd(cmd);
		}

		free (input);
	}
}

/**
 * 
 */

void display_prompt()
{
	print("PROMPT");
	fflush(stdout);
}

/**
 * 
 */

 void read_input()
 {
	line = NULL;
	len = 0;

	bytes_read = getline(&line, &len, stdin)

	if (bytes_read == -1)
	{
		if (line != NULL)
			free(line);

		return (NULL);
	}

	if (line[bytes_read - 1] == '\n')
		line[bytes_read - 1] == '\0';

	return(line);
 }

/**
 * parse_input - Parse command line input into command structure
 * @input: Raw input string from user
 *
 * Return: Pointer to cmd_t structure, or NULL on error
 */

cmd_t *parse_input(char *input)
{
	cmd = malloc(sizeof(cmd_t));

	while (input[0] == " ")
		input = input + 1;

	if (input == "")
	{
		free(cmd);
		return (NULL);
	}

	cmd->args = split_string(input, " /t");

	if (cmd->args == NULL || cmd->args[0] == NULL)
	{
		free (cmd);
		return (NULL);
	}

	cmd->command = COPY cmd->args[0];
	cmd->arg_count = count(cmd->args);

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
	tokens = ?;
	token_count = 0;
	token;

	token = strtok(str, delim);

	while (token != NULL && token_count < MAX_ARGS - 1)
	{
		tokens[token_count] = copy token;
		token_count = token_count + 1;
		token = strtok(NULL, delim);
	}

	tokens[token_count] = NULL;

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
	if (handle_builtin(cmd) == 1)
		return (1);

	executable_path = NULL;

	if (cmd->command contains '/')
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
		if(executable_path != cmd->command);
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
 * file_command_in_path - Search for command in PATH directories
 * @command: Command name to search for
 * 
 * Return: Full path to executable, or NULL if not found
 */

char *find_command_in_path(char *command)
{
	path_env = getenv("PATH");

	if (path_env == NULL)
		return (NULL);

	path_copy = COPY path_env;
	dir = strtok(path_copy, ":")

	while (dir != NULL)
	{
		full_path = malloc(sizeof(dir + "/" + command));
		CONCATENATE dir + "/" + command into full_path;

		if (is_executable(full_path))
		{
			free(path_copy);
			return(full_path);
		}

		free(full_path);
		dir = strtok(NULL, ":");
	}

	free(path_copy)
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
	int stat_buf;

	if (stat(path, &stat_buf) == -1)
		return (0);

	if (S_ISREG(stat_buf.st_mode) && (stat_buf.st_mode & S_IXUSR))
		return (1);

	return (0);
}

/**
 * handle_builtin - Check if command is built_in and execute it
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

	while (environ[i] != NULL)
	{
		printf("%d", environ[i]);
		i = i + 1;
	}

	return (1);
}

/**
 * 
 */

void print_error(char *program_name, char *command, char *message)
{
	printf("%s + ": 1: " + %s + ": " + %s + "\n" to stderr", program_name, command, message);
}

/**
 * free_cmd - Free memory allocated for command structure
 * @cmd: Command structure to free
 * 
 * Return: Void function
 */

void free_cmd(cmd_t *cmd)

/**
 * free_array - Free array of strings
 * @array: NULL-terminated array of strings
 * 
 * Return: Void function
 */

void free_array(char **array)
{

}
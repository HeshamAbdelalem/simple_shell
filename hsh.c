#include "shell.h"

/**
 * hsh - main shell loop
 * @info: the parameter & return info struct
 * @av: the argument vector from main()
 *
 * Return: 0 on success, 1 on error, or error code
 */
int hsh(info_t *info, char **av)
{
	ssize_t r = 0;
	int builtin_ret = 0;

	while (r != -1 && builtin_ret != -2)
	{
		clear_the_info(info);
		if (interactive(info))
			_puts("$ ");
		_error_putchar(BUF_FLUSH);
		r = get_input(info);
		if (r != -1)
		{
			set_info_bf(info, av);
			builtin_ret = find_builtin_em(info);
			if (builtin_ret == -1)
				find_the_cmd(info);
		}
		else if (interactive(info))
			_putchar('\n');
		free_info_bf(info, 0);
	}
	w_history(info);
	free_info_bf(info, 1);
	if (!interactive(info) && info->status)
		exit(info->status);
	if (builtin_ret == -2)
	{
		if (info->err_num == -1)
			exit(info->status);
		exit(info->err_num);
	}
	return (builtin_ret);
}

/**
 * find_builtin_em - finds a builtin command
 * @info: the parameter & return info struct
 *
 * Return: -1 if builtin not found,
 *			0 if builtin executed successfully,
 *			1 if builtin found but not successful,
 *			-2 if builtin signals exit()
 */
int find_builtin_em(info_t *info)
{
	int i, built_in_ret = -1;
	builtin_table builtintbl[] = {
		{"exit", _my_exit},
		{"env", _myenvironment},
		{"help", _my_help},
		{"history", _my_history},
		{"setenv", _mysetenvironment},
		{"unsetenv", _myunsetenvironmet},
		{"cd", _my_cd},
		{"alias", _my_alias},
		{NULL, NULL}
	};

	for (i = 0; builtintbl[i].type; i++)
		if (_string_compare(info->argv[0], builtintbl[i].type) == 0)
		{
			info->line_count++;
			built_in_ret = builtintbl[i].func(info);
			break;
		}
	return (built_in_ret);
}

/**
 * find_the_cmd - finds a command in PATH
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void find_the_cmd(info_t *info)
{
	char *path = NULL;
	int i, k;

	info->path = info->argv[0];
	if (info->linecount_flag == 1)
	{
		info->line_count++;
		info->linecount_flag = 0;
	}
	for (i = 0, k = 0; info->arg[i]; i++)
		if (!isit_delim(info->arg[i], " \t\n"))
			k++;
	if (!k)
		return;

	path = find_the_path(info, _getenv_ironment(info, "PATH="), info->argv[0]);
	if (path)
	{
		info->path = path;
		fork_the_cmd(info);
	}
	else
	{
		if ((interactive(info) || _getenv_ironment(info, "PATH=")
			|| info->argv[0][0] == '/') && isit_cmd(info, info->argv[0]))
			fork_the_cmd(info);
		else if (*(info->arg) != '\n')
		{
			info->status = 127;
			print_the_error(info, "not found\n");
		}
	}
}

/**
 * fork_the_cmd - forks a an exec thread to run cmd
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void fork_the_cmd(info_t *info)
{
	pid_t child_pid;

	child_pid = fork();
	if (child_pid == -1)
	{
		/* TODO: PUT ERROR FUNCTION */
		perror("Error:");
		return;
	}
	if (child_pid == 0)
	{
		if (execve(info->path, info->argv, get_environment(info)) == -1)
		{
			free_info_bf(info, 1);
			if (errno == EACCES)
				exit(126);
			exit(1);
		}
		/* TODO: PUT ERROR FUNCTION */
	}
	else
	{
		wait(&(info->status));
		if (WIFEXITED(info->status))
		{
			info->status = WEXITSTATUS(info->status);
			if (info->status == 126)
				print_the_error(info, "Permission denied\n");
		}
	}
}

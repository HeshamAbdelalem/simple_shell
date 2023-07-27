#include "shell.h"

/**
 * isit_chain ---> test if current char in buffer is a chain delimeter
 * @info: the parameter struct
 * @buf: the char buffer
 * @p: address of current position in buf
 *
 * Return: 1 if chain delimeter, 0 if not
 */
int isit_chain(info_t *info, char *buf, size_t *p)
{
	size_t j = *p;

	if (buf[j] == '|' && buf[j + 1] == '|')
	{
		buf[j] = 0;
		j++;
		info->cmd_buf_type = CMD_OR;
	}
	else if (buf[j] == '&' && buf[j + 1] == '&')
	{
		buf[j] = 0;
		j++;
		info->cmd_buf_type = CMD_AND;
	}
	else if (buf[j] == ';')
	{
		buf[j] = 0;
		info->cmd_buf_type = CMD_CHAIN;
	}
	else
		return (0);
	*p = j;
	return (1);
}

/**
 * chain_check ---> check if we should continue chaining
 * @info: the parameter struct
 * @buf: the char buffer
 * @p: address of current position in buf
 * @i: starting position in buf
 * @len: length of buf
 *
 * Return: Void
 */
void chain_check(info_t *info, char *buf, size_t *p, size_t i, size_t len)
{
	size_t j = *p;

	if (info->cmd_buf_type == CMD_AND)
	{
		if (info->status)
		{
			buf[i] = 0;
			j = len;
		}
	}
	if (info->cmd_buf_type == CMD_OR)
	{
		if (!info->status)
		{
			buf[i] = 0;
			j = len;
		}
	}

	*p = j;
}

/**
 * rep_alias ---> replace an aliases in the tokenized string
 * @info: the parameter struct
 *
 * Return: 1 if replaced, 0 if not
 */
int rep_alias(info_t *info)
{
	int i;
	list_t *node;
	char *p;

	for (i = 0; i < 10; i++)
	{
		node = nodestart_with(info->alias, info->argv[0], '=');
		if (!node)
			return (0);
		free(info->argv[0]);
		p = _string_chr(node->str, '=');
		if (!p)
			return (0);
		p = _string_dup(p + 1);
		if (!p)
			return (0);
		info->argv[0] = p;
	}
	return (1);
}

/**
 * rep_vars - replaces vars in the tokenized string
 * @info: the parameter struct
 *
 * Return: 1 if replaced, 0 if not
 */
int rep_vars(info_t *info)
{
	int i = 0;
	list_t *node;

	for (i = 0; info->argv[i]; i++)
	{
		if (info->argv[i][0] != '$' || !info->argv[i][1])
			continue;

		if (!_string_compare(info->argv[i], "$?"))
		{
			rep_str(&(info->argv[i]),
				_string_dup(conv_number(info->status, 10, 0)));
			continue;
		}
		if (!_string_compare(info->argv[i], "$$"))
		{
			rep_str(&(info->argv[i]),
				_string_dup(conv_number(getpid(), 10, 0)));
			continue;
		}
		node = nodestart_with(info->env, &info->argv[i][1], '=');
		if (node)
		{
			rep_str(&(info->argv[i]),
				_string_dup(_string_chr(node->str, '=') + 1));
			continue;
		}
		rep_str(&info->argv[i], _string_dup(""));

	}
	return (0);
}

/**
 * rep_str - replaces string
 * @old: address of old string
 * @new: new string
 *
 * Return: 1 if replaced, 0 if not
 */
int rep_str(char **old, char *new)
{
	free(*old);
	*old = new;
	return (1);
}

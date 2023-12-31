#include "shell.h"

/**
 * _my_history ---> displays the history list, one command by line, preceded
 *              with line numbers, starting at 0.
 * @info: Structure containing potential arguments. Used to maintain
 *        constant function prototype.
 *  Return: always 0
 */
int _my_history(info_t *info)
{
	show_list(info->history);
	return (0);
}

/**
 * unset_alias ---> sets alias to string
 * @info: parameter struct
 * @str: the string alias
 *
 * Return: always 0 on success, 1 if not
 */
int unset_alias(info_t *info, char *str)
{
	char *p, c;
	int ret;

	p = _string_chr(str, '=');
	if (!p)
		return (1);
	c = *p;
	*p = 0;
	ret = del_nodeat_index(&(info->alias),
		get_index_node(info->alias, nodestart_with(info->alias, str, -1)));
	*p = c;
	return (ret);
}

/**
 * set_alias ---> sets alias to string
 * @info: parameter struct
 * @str: the string alias
 *
 * Return: always 0 on success, 1 if not
 */
int set_alias(info_t *info, char *str)
{
	char *p;

	p = _string_chr(str, '=');
	if (!p)
		return (1);
	if (!*++p)
		return (unset_alias(info, str));

	unset_alias(info, str);
	return (insert_node_at_end(&(info->alias), str, 0) == NULL);
}

/**
 * print_alias ---> prints an alias string
 * @node: the alias node
 *
 * Return: always 0 on success, 1 if not
 */
int print_alias(list_t *node)
{
	char *p = NULL, *a = NULL;

	if (node)
	{
		p = _string_chr(node->str, '=');
		for (a = node->str; a <= p; a++)
			_putchar(*a);
		_putchar('\'');
		_puts(p + 1);
		_puts("'\n");
		return (0);
	}
	return (1);
}

/**
 * _my_alias - mimics the alias builtin (man alias)
 * @info: Structure containing potential arguments. Used to maintain
 *          constant function prototype.
 *  Return: always 0
 */
int _my_alias(info_t *info)
{
	int i = 0;
	char *p = NULL;
	list_t *node = NULL;

	if (info->argc == 1)
	{
		node = info->alias;
		while (node)
		{
			print_alias(node);
			node = node->next;
		}
		return (0);
	}
	for (i = 1; info->argv[i]; i++)
	{
		p = _string_chr(info->argv[i], '=');
		if (p)
			set_alias(info, info->argv[i]);
		else
			print_alias(nodestart_with(info->alias, info->argv[i], '='));
	}

	return (0);
}

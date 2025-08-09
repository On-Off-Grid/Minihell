/* Syntax analysis - parsing tokens into commands */
#include "../includes/minishell.h"

static t_cmd	*new_cmd(void)
{
	t_cmd	*cmd;

	cmd = safe_malloc(sizeof(t_cmd));
	cmd->argv = NULL;
	cmd->in = NULL;
	cmd->out = NULL;
	cmd->next = NULL;
	return (cmd);
}

static void	add_arg(t_cmd *cmd, char *arg)
{
	char	**new_argv;
	int		argc;
	int		i;

	argc = 0;
	while (cmd->argv && cmd->argv[argc])
		argc++;
	new_argv = safe_malloc(sizeof(char *) * (argc + 2));
	i = 0;
	while (i < argc)
	{
		new_argv[i] = cmd->argv[i];
		i++;
	}
	new_argv[argc] = ft_strdup(arg);
	new_argv[argc + 1] = NULL;
	free(cmd->argv);
	cmd->argv = new_argv;
}

static void	add_redir(t_redir **head, int type, char *file)
{
	t_redir	*new_redir;
	t_redir	*current;

	new_redir = safe_malloc(sizeof(t_redir));
	new_redir->type = type;
	new_redir->file = ft_strdup(file);
	new_redir->next = NULL;
	if (!*head)
		*head = new_redir;
	else
	{
		current = *head;
		while (current->next)
			current = current->next;
		current->next = new_redir;
	}
}

static int	is_redirection_token(t_token_type type)
{
	return (type == OP_IN || type == OP_OUT || 
			type == OP_HEREDOC || type == OP_APPEND);
}

int	get_redir_type(t_token_type type)
{
	if (type == OP_IN)
		return (0);
	if (type == OP_OUT)
		return (1);
	if (type == OP_HEREDOC)
		return (2);
	if (type == OP_APPEND)
		return (3);
	return (-1);
}

static t_token	*parse_redirections(t_token *tokens, t_cmd *cmd)
{
	t_token	*current;
	int		redir_type;

	current = tokens;
	while (current && current->type != PIPE)
	{
		if (is_redirection_token(current->type))
		{
			redir_type = get_redir_type(current->type);
			current = current->next;
			if (current && (current->type == WORD || 
				current->type == SQUOTE || current->type == DQUOTE))
			{
				if (redir_type == 0 || redir_type == 2)
					add_redir(&cmd->in, redir_type, current->value);
				else
					add_redir(&cmd->out, redir_type, current->value);
				current = current->next;
			}
		}
		else if (current->type == WORD || current->type == SQUOTE || 
				current->type == DQUOTE)
		{
			add_arg(cmd, current->value);
			current = current->next;
		}
		else
			current = current->next;
	}
	return (current);
}

t_cmd	*parse_tokens(t_token *tokens)
{
	t_cmd	*head;
	t_cmd	*current_cmd;
	t_cmd	*new_command;
	t_token	*current_token;

	if (!tokens)
		return (NULL);
	head = new_cmd();
	current_cmd = head;
	current_token = tokens;
	while (current_token)
	{
		current_token = parse_redirections(current_token, current_cmd);
		if (current_token && current_token->type == PIPE)
		{
			new_command = new_cmd();
			current_cmd->next = new_command;
			current_cmd = new_command;
			current_token = current_token->next;
		}
		else
			break ;
	}
	if (!head->argv && !head->in && !head->out)
	{
		free(head);
		return (NULL);
	}
	return (head);
}

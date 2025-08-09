/* Error handling and memory management utilities */
#include "../includes/minishell.h"

void	perror_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void	*safe_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
		perror_exit("malloc");
	return (ptr);
}

void	free_tokens(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens->next;
		free(tokens->value);
		free(tokens);
		tokens = tmp;
	}
}

void	free_cmds(t_cmd *cmds)
{
	t_cmd	*tmp;
	t_redir	*redir_tmp;
	int		i;

	while (cmds)
	{
		tmp = cmds->next;
		if (cmds->argv)
		{
			i = 0;
			while (cmds->argv[i])
			{
				free(cmds->argv[i]);
				i++;
			}
			free(cmds->argv);
		}
		while (cmds->in)
		{
			redir_tmp = cmds->in->next;
			free(cmds->in->file);
			free(cmds->in);
			cmds->in = redir_tmp;
		}
		while (cmds->out)
		{
			redir_tmp = cmds->out->next;
			free(cmds->out->file);
			free(cmds->out);
			cmds->out = redir_tmp;
		}
		free(cmds);
		cmds = tmp;
	}
}

void	free_ast(t_node *node)
{
	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	free(node);
}

void	free_split(char **split)
{
	int	i;

	if (!split)
		return ;
	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

static char	*expand_single_var(char *str, int *i, char **envp, int last_status)
{
	char	*var_name;
	char	*var_value;
	int		start;
	int		len;

	start = *i + 1;
	if (str[start] == '?')
	{
		*i = start + 1;
		return (ft_itoa(last_status));
	}
	len = 0;
	while (str[start + len] && (ft_isalnum(str[start + len]) || 
		str[start + len] == '_'))
		len++;
	if (len == 0)
	{
		*i = start;
		return (ft_strdup("$"));
	}
	var_name = ft_substr(str, start, len);
	var_value = get_env_value(envp, var_name);
	free(var_name);
	*i = start + len;
	if (var_value)
		return (ft_strdup(var_value));
	return (ft_strdup(""));
}

char	*expand_vars(char *str, char **envp, int last_status)
{
	char	*result;
	char	*expanded;
	char	*temp;
	int		i;

	if (!str)
		return (NULL);
	result = ft_strdup("");
	i = 0;
	while (str[i])
	{
		if (str[i] == '$')
		{
			expanded = expand_single_var(str, &i, envp, last_status);
			temp = ft_strjoin(result, expanded);
			free(result);
			free(expanded);
			result = temp;
		}
		else
		{
			temp = ft_strjoin(result, ft_substr(str, i, 1));
			free(result);
			result = temp;
			i++;
		}
	}
	return (result);
}

void	expand_token_vars(t_token *tokens, char **envp, int last_status)
{
	t_token	*current;
	char	*expanded;

	current = tokens;
	while (current)
	{
		if (current->type == WORD || current->type == DQUOTE)
		{
			expanded = expand_vars(current->value, envp, last_status);
			if (expanded)
			{
				free(current->value);
				current->value = expanded;
			}
		}
		current = current->next;
	}
}

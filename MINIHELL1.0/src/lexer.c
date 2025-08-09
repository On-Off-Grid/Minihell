/* Lexical analysis - tokenization */
#include "../includes/minishell.h"

static int	is_meta_char(char c)
{
	return (c == '<' || c == '>' || c == '|');
}

static t_token	*init_token_list(void)
{
	return (NULL);
}

static void	add_token(t_token **head, t_token_type type, char *value)
{
	t_token	*new_token;
	t_token	*current;

	new_token = safe_malloc(sizeof(t_token));
	new_token->type = type;
	new_token->value = ft_strdup(value);
	new_token->next = NULL;
	if (!*head)
		*head = new_token;
	else
	{
		current = *head;
		while (current->next)
			current = current->next;
		current->next = new_token;
	}
}

static t_token_type	get_operator_type(const char *str, int *len)
{
	if (str[0] == '<' && str[1] == '<')
	{
		*len = 2;
		return (OP_HEREDOC);
	}
	if (str[0] == '>' && str[1] == '>')
	{
		*len = 2;
		return (OP_APPEND);
	}
	if (str[0] == '<')
	{
		*len = 1;
		return (OP_IN);
	}
	if (str[0] == '>')
	{
		*len = 1;
		return (OP_OUT);
	}
	if (str[0] == '|')
	{
		*len = 1;
		return (PIPE);
	}
	*len = 0;
	return (WORD);
}

static void	handle_quotes(const char *line, int *i, t_token **tokens)
{
	char		quote_char;
	int			start;
	char		*value;
	t_token_type	type;

	quote_char = line[*i];
	start = ++(*i);
	while (line[*i] && line[*i] != quote_char)
		(*i)++;
	if (line[*i] == quote_char)
	{
		value = ft_substr(line, start, *i - start);
		type = (quote_char == '\'') ? SQUOTE : DQUOTE;
		add_token(tokens, type, value);
		free(value);
		(*i)++;
	}
}

static void	handle_word(const char *line, int *i, t_token **tokens)
{
	int		start;
	char	*value;

	start = *i;
	while (line[*i] && !is_meta_char(line[*i]) && line[*i] != ' '
		&& line[*i] != '\t' && line[*i] != '\'' && line[*i] != '"')
		(*i)++;
	value = ft_substr(line, start, *i - start);
	add_token(tokens, WORD, value);
	free(value);
}

t_token	*lexer(const char *line)
{
	t_token	*tokens;
	int		i;
	int		op_len;
	char	op_str[3];

	tokens = init_token_list();
	i = 0;
	while (line[i])
	{
		while (line[i] == ' ' || line[i] == '\t')
			i++;
		if (!line[i])
			break ;
		if (line[i] == '\'' || line[i] == '"')
			handle_quotes(line, &i, &tokens);
		else if (is_meta_char(line[i]))
		{
			ft_memset(op_str, 0, 3);
			op_str[0] = line[i];
			if (line[i + 1] && ((line[i] == '<' && line[i + 1] == '<')
					|| (line[i] == '>' && line[i + 1] == '>')))
				op_str[1] = line[i + 1];
			add_token(&tokens, get_operator_type(op_str, &op_len), op_str);
			i += op_len;
		}
		else
			handle_word(line, &i, &tokens);
	}
	return (tokens);
}

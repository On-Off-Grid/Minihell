/* Input/Output redirections */
#include "../includes/minishell.h"

static char	*create_heredoc_file(char *delimiter)
{
	char	*tmp_file;
	char	*line;
	int		fd;
	int		i;

	tmp_file = ft_strjoin("/tmp/minishell_heredoc_", delimiter);
	i = 0;
	while (i < 1000)
	{
		fd = open(tmp_file, O_CREAT | O_EXCL | O_WRONLY, 0600);
		if (fd >= 0)
			break ;
		free(tmp_file);
		tmp_file = ft_strjoin("/tmp/minishell_heredoc_", ft_strjoin(delimiter, "X"));
		i++;
	}
	if (fd < 0)
		return (NULL);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
		free(line);
	}
	close(fd);
	return (tmp_file);
}

static int	handle_input_redir(t_redir *redir)
{
	char	*tmp_file;
	int		fd;

	if (redir->type == 0)
	{
		fd = open(redir->file, O_RDONLY);
		if (fd < 0)
		{
			perror(redir->file);
			return (-1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	else if (redir->type == 2)
	{
		tmp_file = create_heredoc_file(redir->file);
		if (!tmp_file)
		{
			perror("heredoc");
			return (-1);
		}
		fd = open(tmp_file, O_RDONLY);
		if (fd < 0)
		{
			perror(tmp_file);
			unlink(tmp_file);
			free(tmp_file);
			return (-1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
		unlink(tmp_file);
		free(tmp_file);
	}
	return (0);
}

static int	handle_output_redir(t_redir *redir)
{
	int	fd;

	if (redir->type == 1)
		fd = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == 3)
		fd = open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		return (0);
	if (fd < 0)
	{
		perror(redir->file);
		return (-1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

int	apply_redirections(t_redir *in, t_redir *out)
{
	t_redir	*current;

	current = in;
	while (current)
	{
		if (handle_input_redir(current) == -1)
			return (-1);
		current = current->next;
	}
	current = out;
	while (current)
	{
		if (handle_output_redir(current) == -1)
			return (-1);
		current = current->next;
	}
	return (0);
}

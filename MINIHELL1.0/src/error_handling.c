/* Enhanced error handling and reporting */
#include "../includes/minishell.h"

void	print_error(const char *cmd, const char *arg, const char *msg)
{
	ft_putstr_fd("minishell: ", 2);
	if (cmd)
	{
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": ", 2);
	}
	if (arg)
	{
		ft_putstr_fd(arg, 2);
		ft_putstr_fd(": ", 2);
	}
	ft_putendl_fd(msg, 2);
}

void	print_command_not_found(const char *cmd)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putendl_fd(": command not found", 2);
}

void	print_syntax_error(const char *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	if (token)
		ft_putstr_fd(token, 2);
	else
		ft_putstr_fd("newline", 2);
	ft_putendl_fd("'", 2);
}

void	print_file_error(const char *filename, const char *error_type)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(filename, 2);
	ft_putstr_fd(": ", 2);
	ft_putendl_fd(error_type, 2);
}

void	print_permission_denied(const char *filename)
{
	print_file_error(filename, "Permission denied");
}

void	print_no_such_file(const char *filename)
{
	print_file_error(filename, "No such file or directory");
}

void	print_is_directory(const char *filename)
{
	print_file_error(filename, "Is a directory");
}

int	handle_fork_error(void)
{
	perror("minishell: fork");
	return (1);
}

int	handle_pipe_error(void)
{
	perror("minishell: pipe");
	return (1);
}

int	handle_execve_error(const char *cmd)
{
	print_command_not_found(cmd);
	exit(127);
}

int	validate_builtin_args(const char *cmd, char **argv, int min_args, int max_args)
{
	int	argc;

	argc = 0;
	while (argv && argv[argc])
		argc++;
	
	if (argc < min_args)
	{
		print_error(cmd, NULL, "too few arguments");
		return (0);
	}
	
	if (max_args > 0 && argc > max_args)
	{
		print_error(cmd, NULL, "too many arguments");
		return (0);
	}
	
	return (1);
}

int	validate_file_access(const char *filename, int mode)
{
	struct stat	file_stat;

	if (access(filename, F_OK) != 0)
	{
		print_no_such_file(filename);
		return (0);
	}
	
	if (stat(filename, &file_stat) == 0 && S_ISDIR(file_stat.st_mode))
	{
		print_is_directory(filename);
		return (0);
	}
	
	if (access(filename, mode) != 0)
	{
		print_permission_denied(filename);
		return (0);
	}
	
	return (1);
}

int	handle_redirection_error(const char *filename, int redir_type)
{
	if (redir_type == OP_IN)
	{
		if (!validate_file_access(filename, R_OK))
			return (1);
	}
	else if (redir_type == OP_OUT || redir_type == OP_APPEND)
	{
		char	*dir_path;
		char	*last_slash;
		int		result;

		/* Check if we can write to the directory */
		dir_path = ft_strdup(filename);
		last_slash = ft_strrchr(dir_path, '/');
		
		if (last_slash)
		{
			*last_slash = '\0';
			if (access(dir_path, W_OK) != 0)
			{
				print_permission_denied(filename);
				free(dir_path);
				return (1);
			}
		}
		
		/* Check if file exists and is writable, or can be created */
		if (access(filename, F_OK) == 0)
		{
			result = validate_file_access(filename, W_OK);
			free(dir_path);
			return (!result);
		}
		
		free(dir_path);
	}
	
	return (0);
}

void	cleanup_on_error(t_token *tokens, t_cmd *commands)
{
	if (tokens)
		free_tokens(tokens);
	if (commands)
		free_cmds(commands);
}

int	is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (0);
	
	i = 1;
	while (str[i])
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	
	return (1);
}

void	print_invalid_identifier(const char *cmd, const char *identifier)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": `", 2);
	ft_putstr_fd(identifier, 2);
	ft_putendl_fd("': not a valid identifier", 2);
}

int	check_memory_allocation(void *ptr, const char *function_name)
{
	if (!ptr)
	{
		print_error(function_name, NULL, "memory allocation failed");
		return (0);
	}
	return (1);
}

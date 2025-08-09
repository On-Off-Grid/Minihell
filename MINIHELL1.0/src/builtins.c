/* Built-in shell commands */
#include "../includes/minishell.h"
#include <errno.h>

static char **g_original_env = NULL;

void	set_original_env(char **envp)
{
	g_original_env = envp;
}

int	builtin_echo(char **argv)
{
	int		i;
	int		newline;

	newline = 1;
	i = 1;
	if (argv[1] && ft_strcmp(argv[1], "-n") == 0)
	{
		newline = 0;
		i = 2;
	}
	while (argv[i])
	{
		ft_putstr_fd(argv[i], STDOUT_FILENO);
		if (argv[i + 1])
			ft_putstr_fd(" ", STDOUT_FILENO);
		i++;
	}
	if (newline)
		ft_putstr_fd("\n", STDOUT_FILENO);
	return (0);
}

int	builtin_cd(char **argv, char **envp)
{
	char	*path;

	(void)envp;
	if (!argv[1])
		path = get_env_value(envp, "HOME");
	else
		path = argv[1];
	if (!path)
	{
		print_error("cd", NULL, "HOME not set");
		return (1);
	}
	if (chdir(path) != 0)
	{
		print_error("cd", path, strerror(errno));
		return (1);
	}
	return (0);
}

int	builtin_pwd(void)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		perror("pwd");
		return (1);
	}
	ft_putendl_fd(pwd, STDOUT_FILENO);
	free(pwd);
	return (0);
}

int	builtin_export(char **argv, char ***envp)
{
	char	**split;
	char	**new_envp;
	int		i;

	if (!argv[1])
	{
		builtin_env(*envp);
		return (0);
	}
	i = 1;
	while (argv[i])
	{
		split = ft_split(argv[i], '=');
		if (!split || !split[0])
		{
			free_split(split);
			return (1);
		}
		if (!is_valid_identifier(split[0]))
		{
			print_invalid_identifier("export", split[0]);
			free_split(split);
			return (1);
		}
		if (!split[1])
		{
			if (env_var_exists(*envp, split[0]))
				new_envp = env_update_var(*envp, split[0], "");
			else
				new_envp = env_add_var(*envp, split[0], "");
		}
		else
		{
			if (env_var_exists(*envp, split[0]))
				new_envp = env_update_var(*envp, split[0], split[1]);
			else
				new_envp = env_add_var(*envp, split[0], split[1]);
		}
		free_split(split);
		/* Only free if this isn't the original envp from main */
		if (*envp != g_original_env)
			free_split(*envp);
		*envp = new_envp;
		i++;
	}
	return (0);
}

int	builtin_unset(char **argv, char ***envp)
{
	char	**new_envp;
	int		i;

	if (!argv[1])
		return (0);
	i = 1;
	while (argv[i])
	{
		if (!is_valid_identifier(argv[i]))
		{
			print_invalid_identifier("unset", argv[i]);
			i++;
			continue;
		}
		if (env_var_exists(*envp, argv[i]))
		{
			new_envp = env_remove_var(*envp, argv[i]);
			if (*envp != g_original_env)
				free_split(*envp);
			*envp = new_envp;
		}
		i++;
	}
	return (0);
}

int	builtin_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		ft_putendl_fd(envp[i], STDOUT_FILENO);
		i++;
	}
	return (0);
}

int	builtin_exit(char **argv)
{
	int	exit_code;
	int	i;

	exit_code = 0;
	ft_putendl_fd("exit", STDOUT_FILENO);
	if (argv[1])
	{
		i = 0;
		if (argv[1][0] == '-' || argv[1][0] == '+')
			i = 1;
		while (argv[1][i])
		{
			if (argv[1][i] < '0' || argv[1][i] > '9')
			{
				ft_putstr_fd("exit: ", 2);
				ft_putstr_fd(argv[1], 2);
				ft_putendl_fd(": numeric argument required", 2);
				exit(255);
			}
			i++;
		}
		exit_code = ft_atoi(argv[1]);
	}
	exit(exit_code);
}

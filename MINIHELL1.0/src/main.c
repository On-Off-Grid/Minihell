/* Main shell loop and initialization */
#include "../includes/minishell.h"

volatile sig_atomic_t	g_signal = 0;
static int				g_last_status = 0;

static void	handle_signal_event(void)
{
	if (g_signal == SIGINT)
	{
		g_last_status = 130;
		g_signal = 0;
	}
	else if (g_signal == SIGQUIT)
	{
		g_last_status = 131;
		g_signal = 0;
	}
}

static void	process_line(char *line, char ***envp)
{
	t_token	*tokens;
	t_cmd	*commands;

	if (!line || !*line)
		return ;
	tokens = lexer(line);
	if (!tokens)
		return ;
	/* Expand environment variables in tokens */
	expand_token_vars(tokens, *envp, g_last_status);
	commands = parse_tokens(tokens);
	if (commands)
	{
		g_last_status = exec_cmds(commands, envp);
		free_cmds(commands);
	}
	free_tokens(tokens);
}

int	main(int argc, char **argv, char **envp)
{
	char	*line;
	char	**local_env;

	(void)argc;
	(void)argv;
	local_env = envp;
	set_original_env(envp);
	init_signals();
	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
		{
			write(STDOUT_FILENO, "exit\n", 5);
			break ;
		}
		handle_signal_event();
		if (*line)
			add_history(line);
		process_line(line, &local_env);
		free(line);
	}
	return (g_last_status);
}

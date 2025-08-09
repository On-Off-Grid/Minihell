/* Command execution engine */
#include "../includes/minishell.h"

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd, "export") == 0)
		return (1);
	if (ft_strcmp(cmd, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd, "env") == 0)
		return (1);
	if (ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

static int	exec_builtin(t_cmd *cmd, char ***envp)
{
	if (ft_strcmp(cmd->argv[0], "echo") == 0)
		return (builtin_echo(cmd->argv));
	if (ft_strcmp(cmd->argv[0], "cd") == 0)
		return (builtin_cd(cmd->argv, *envp));
	if (ft_strcmp(cmd->argv[0], "pwd") == 0)
		return (builtin_pwd());
	if (ft_strcmp(cmd->argv[0], "export") == 0)
		return (builtin_export(cmd->argv, envp));
	if (ft_strcmp(cmd->argv[0], "unset") == 0)
		return (builtin_unset(cmd->argv, envp));
	if (ft_strcmp(cmd->argv[0], "env") == 0)
		return (builtin_env(*envp));
	if (ft_strcmp(cmd->argv[0], "exit") == 0)
		return (builtin_exit(cmd->argv));
	return (1);
}

char	*find_command_path(char *cmd, char **envp)
{
	char	*path_env;
	char	**paths;
	char	*full_path;
	char	*temp;
	int		i;

	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = get_env_value(envp, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(temp, cmd);
		free(temp);
		if (access(full_path, X_OK) == 0)
		{
			free_split(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_split(paths);
	return (NULL);
}

static int	exec_external_cmd(t_cmd *cmd, char **envp)
{
	pid_t	pid;
	char	*cmd_path;
	int		status;

	cmd_path = find_command_path(cmd->argv[0], envp);
	if (!cmd_path)
	{
		print_command_not_found(cmd->argv[0]);
		return (127);
	}
	pid = fork();
	if (pid == 0)
	{
		init_child_signals();
		if (apply_redirections(cmd->in, cmd->out) == -1)
			exit(1);
		execve(cmd_path, cmd->argv, envp);
		handle_execve_error(cmd->argv[0]);
	}
	else if (pid > 0)
	{
		free(cmd_path);
		restore_signals();
		waitpid(pid, &status, 0);
		init_signals();
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		if (WIFSIGNALED(status))
		{
			g_signal = WTERMSIG(status);
			if (WTERMSIG(status) == SIGINT)
				write(STDOUT_FILENO, "\n", 1);
			else if (WTERMSIG(status) == SIGQUIT)
				write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
			return (128 + WTERMSIG(status));
		}
	}
	else
	{
		free(cmd_path);
		return (handle_fork_error());
	}
	return (1);
}

/* khdamna ***pointer l envp 7itach l'environment imkn itmodifia b builtin functions b7al export o unset */
/*  # dup(): catcreer wahd duplicate file descriptor li kay pointi 3la nafss lfile/stream; kat creeer wahd new entry f file table entry li f l kernel.
			khadamnaha to save l original stdin o stdout fd bach n9dro ndirou redirections f l child process.
	# execve(): kat execute l command li 3tina, o katdir redirections li 3tina f l child process.
	# fork(): kat create wahd child process li kaykhdem f nafss l program, o katdir copy l memory space dyal parent process.
	# waitpid(): kat khdem 3la l child process o kat tsnna l termination dyal l child process, o kat recupiri l exit status dyal l child.
	# handle_execve_error(): kat handle l error li kayn f execve() b7al l command ma kaynach f l PATH, o kat printi message li
	# handle_fork_error(): kat handle l error li kayn f fork() b7al ma t9drch tkhdem fork, o kat printi message li kayn f
	# restore_signals(): kat restore l signals li kanou f parent process bach l child process ykhdem b signals li kayn f parent.
	# init_child_signals():
	# init_signals(): kat initialize l signals li kayn f parent process bach l child process ykhdem b signals li kayn f parent.
	# dup2(): kat replace lfd li kayn f l first argument b lfd li kayn f l second argument, 
	# close(): kat close lfd li kayn f l first argument, o kat9ta3 l connection ma bin lfd o lfile/stream.
	# perror(): kat printi error message li kayn f l first argument, o kat9ta3 l connection ma bin lfd o lfile/stream.*/
static int	exec_single_cmd(t_cmd *cmd, char ***envp)
{
	int	stdin_backup;
	int	stdout_backup;
	int	result;

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (0);
	if (is_builtin(cmd->argv[0]))
	{
		stdin_backup = dup(STDIN_FILENO);
		stdout_backup = dup(STDOUT_FILENO);
		if (apply_redirections(cmd->in, cmd->out) == -1)
		{
			close(stdin_backup);
			close(stdout_backup);
			return (1);
		}
		result = exec_builtin(cmd, envp);
		dup2(stdin_backup, STDIN_FILENO);
		dup2(stdout_backup, STDOUT_FILENO);
		close(stdin_backup);
		close(stdout_backup);
		return (result);
	}
	else
		return (exec_external_cmd(cmd, *envp));
}

int	exec_cmds(t_cmd *cmds, char ***envp)
{
	t_cmd	*current;
	int		status;

	if (!cmds)
		return (0);
	current = cmds;
	status = 0;
	while (current)
	{
		status = exec_single_cmd(current, envp);
		current = current->next;
	}
	return (status);
}

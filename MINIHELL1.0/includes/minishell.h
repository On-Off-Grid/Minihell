#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <signal.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/libft.h"

extern volatile sig_atomic_t	g_signal;

typedef enum e_token_type
{
	WORD,
	SQUOTE,
	DQUOTE,
	OP_IN,
	OP_OUT,
	OP_HEREDOC,
	OP_APPEND,
	PIPE,
	VAR,
	DOLLAR
}	t_token_type;

typedef struct s_token
{
	t_token_type		type;
	char				*value;
	struct s_token		*next;
}	t_token;

typedef struct s_redir
{
	int					type;
	char				*file;
	struct s_redir		*next;
}	t_redir;

typedef struct s_cmd
{
	char				**argv;
	t_redir				*in;
	t_redir				*out;
	struct s_cmd		*next;
}	t_cmd;

typedef struct s_node
{
	int					type;
	struct s_node		*left;
	struct s_node		*right;
	t_cmd				*cmd;
}	t_node;

typedef struct s_env
{
	char				*key;
	char				*value;
	struct s_env		*next;
}	t_env;

# define NODE_CMD 0
# define NODE_PIPE 1
# define NODE_SEQ 2

t_token		*lexer(const char *line);
t_cmd		*parse_tokens(t_token *tokens);
int			get_redir_type(t_token_type type);
t_node		*build_ast(t_cmd *cmds);
int			exec_cmds(t_cmd *cmds, char ***envp);
char		*find_command_path(char *cmd, char **envp);
int			apply_redirections(t_redir *in, t_redir *out);
int			is_builtin(char *cmd);

int			builtin_echo(char **argv);
int			builtin_cd(char **argv, char **envp);
int			builtin_pwd(void);
int			builtin_export(char **argv, char ***envp);
int			builtin_unset(char **argv, char ***envp);
int			builtin_env(char **envp);
int			builtin_exit(char **argv);
void		set_original_env(char **envp);

char		*get_env(t_env *env, const char *key);

void		init_signals(void);
void		handle_sigint(int sig);
void		handle_sigquit(int sig);
void		handle_sigint_child(int sig);
void		handle_sigquit_child(int sig);
void		init_child_signals(void);
void		restore_signals(void);

void		perror_exit(const char *msg);
void		*safe_malloc(size_t size);

/* Enhanced error handling functions */
void		print_error(const char *cmd, const char *arg, const char *msg);
void		print_command_not_found(const char *cmd);
void		print_syntax_error(const char *token);
void		print_file_error(const char *filename, const char *error_type);
void		print_permission_denied(const char *filename);
void		print_no_such_file(const char *filename);
void		print_is_directory(const char *filename);
int			handle_fork_error(void);
int			handle_pipe_error(void);
int			handle_execve_error(const char *cmd);
int			validate_builtin_args(const char *cmd, char **argv, int min_args, int max_args);
int			validate_file_access(const char *filename, int mode);
int			handle_redirection_error(const char *filename, int redir_type);
void		cleanup_on_error(t_token *tokens, t_cmd *commands);
int			is_valid_identifier(const char *str);
void		print_invalid_identifier(const char *cmd, const char *identifier);
int			check_memory_allocation(void *ptr, const char *function_name);

void		free_tokens(t_token *tokens);
void		free_cmds(t_cmd *cmds);
void		free_ast(t_node *node);
char		*get_env_value(char **envp, const char *key);
void		free_split(char **split);
char		**env_add_var(char **envp, const char *key, const char *value);
char		**env_update_var(char **envp, const char *key, const char *value);
char		**env_remove_var(char **envp, const char *key);
int			env_var_exists(char **envp, const char *key);
char		*expand_vars(char *str, char **envp, int last_status);
void		expand_token_vars(t_token *tokens, char **envp, int last_status);

#endif

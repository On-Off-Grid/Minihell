/* Environment variable management */
#include "../includes/minishell.h"

char	*get_env_value(char **envp, const char *key)
{
	int		i;
	int		key_len;
	char	*equals;

	if (!envp || !key)
		return (NULL);
	key_len = ft_strlen(key);
	i = 0;
	while (envp[i])
	{
		equals = ft_strchr(envp[i], '=');
		if (equals && (equals - envp[i]) == key_len)
		{
			if (ft_strncmp(envp[i], key, key_len) == 0)
				return (equals + 1);
		}
		i++;
	}
	return (NULL);
}

int	env_var_exists(char **envp, const char *key)
{
	return (get_env_value(envp, key) != NULL);
}

char	**env_add_var(char **envp, const char *key, const char *value)
{
	char	**new_envp;
	char	*new_var;
	int		count;
	int		i;

	count = 0;
	while (envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 2));
	if (!new_envp)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_envp[i] = ft_strdup(envp[i]);
		i++;
	}
	new_var = malloc(ft_strlen(key) + ft_strlen(value) + 2);
	if (!new_var)
	{
		free_split(new_envp);
		return (NULL);
	}
	ft_strcpy(new_var, key);
	ft_strcat(new_var, "=");
	ft_strcat(new_var, value);
	new_envp[count] = new_var;
	new_envp[count + 1] = NULL;
	return (new_envp);
}

char	**env_update_var(char **envp, const char *key, const char *value)
{
	char	**new_envp;
	char	*new_var;
	int		count;
	int		i;
	int		key_len;

	count = 0;
	while (envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * (count + 1));
	if (!new_envp)
		return (NULL);
	key_len = ft_strlen(key);
	i = 0;
	while (i < count)
	{
		if (ft_strncmp(envp[i], key, key_len) == 0 && envp[i][key_len] == '=')
		{
			new_var = malloc(ft_strlen(key) + ft_strlen(value) + 2);
			if (!new_var)
				return (NULL);
			ft_strcpy(new_var, key);
			ft_strcat(new_var, "=");
			ft_strcat(new_var, value);
			new_envp[i] = new_var;
		}
		else
			new_envp[i] = ft_strdup(envp[i]);
		i++;
	}
	new_envp[count] = NULL;
	return (new_envp);
}

char	**env_remove_var(char **envp, const char *key)
{
	char	**new_envp;
	int		count;
	int		i;
	int		j;
	int		key_len;

	count = 0;
	while (envp[count])
		count++;
	new_envp = malloc(sizeof(char *) * count);
	if (!new_envp)
		return (NULL);
	key_len = ft_strlen(key);
	i = 0;
	j = 0;
	while (i < count)
	{
		if (!(ft_strncmp(envp[i], key, key_len) == 0 && envp[i][key_len] == '='))
		{
			new_envp[j] = ft_strdup(envp[i]);
			j++;
		}
		i++;
	}
	new_envp[j] = NULL;
	return (new_envp);
}

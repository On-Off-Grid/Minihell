/* Abstract Syntax Tree construction */
#include "../includes/minishell.h"

static t_node	*new_node(int type, t_cmd *cmd)
{
	t_node	*node;

	node = safe_malloc(sizeof(t_node));
	node->type = type;
	node->left = NULL;
	node->right = NULL;
	node->cmd = cmd;
	return (node);
}

static t_node	*attach_pipe(t_node *left, t_node *right)
{
	t_node	*pipe_node;

	pipe_node = new_node(NODE_PIPE, NULL);
	pipe_node->left = left;
	pipe_node->right = right;
	return (pipe_node);
}

static t_node	*build_pipeline_ast(t_cmd *cmds)
{
	t_node	*left;
	t_node	*right;
	t_cmd	*current;

	if (!cmds)
		return (NULL);
	if (!cmds->next)
		return (new_node(NODE_CMD, cmds));
	left = new_node(NODE_CMD, cmds);
	current = cmds->next;
	while (current)
	{
		right = new_node(NODE_CMD, current);
		left = attach_pipe(left, right);
		current = current->next;
	}
	return (left);
}

t_node	*build_ast(t_cmd *cmds)
{
	t_node	*ast;

	if (!cmds)
		return (NULL);
	ast = build_pipeline_ast(cmds);
	return (ast);
}

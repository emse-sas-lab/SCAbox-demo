//
// Created by Sami Dahoux (s.dahoux@emse.fr) on 19/06/2020.
//

#include <cmd/cmd.h>


void CMD_cmd_reset(CMD_cmd_t *cmd)
{
	strcpy(cmd->name, "");
	for (size_t o = 0; o < CMD_MAX_OPTS; o++)
	{
		CMD_opt_reset(&cmd->options[o]);
	}
}

CMD_cmd_t CMD_cmd(char *name)
{
	CMD_cmd_t cmd;
	CMD_cmd_reset(&cmd);
	strcpy(cmd.name, name);
	return cmd;
}

CMD_cmd_t *CMD_cmd_copy(CMD_cmd_t *dst, const CMD_cmd_t *src)
{
	strcpy(dst->name, src->name);
	for (size_t o = 0; o < CMD_MAX_OPTS; o++)
	{
		CMD_opt_copy(&dst->options[o], &src->options[o]);
	}
	return dst;
}


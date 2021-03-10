/**
 * @file cmd.h
 * @author Sami Dahoux (s.dahoux@emse.fr)
 * @brief Command parsing module featuring options
 */

#ifndef TINY_CMD_CMD_H
#define TINY_CMD_CMD_H

#include <cmd/consts.h>
#include <cmd/opt.h>


typedef struct
{
	char name[CMD_LINE_LEN];
	CMD_opt_t options[CMD_MAX_OPTS];
} CMD_cmd_t;

CMD_cmd_t CMD_cmd(char *name);

void CMD_cmd_reset(CMD_cmd_t *cmd);

CMD_cmd_t *CMD_cmd_copy(CMD_cmd_t *dst, const CMD_cmd_t *src);

#endif //TINY_CMD_CMD_H

//
// Created by samid on 19/08/2020.
//

#ifndef TINY_CMD_DESCR_H
#define TINY_CMD_DESCR_H

#include <cmd/consts.h>
#include <cmd/err.h>
#include <cmd/opt.h>
#include <cmd/cmd.h>


typedef CMD_err_t *(*CMD_action_t)(const CMD_cmd_t *cmd);

typedef struct
{
	char label;
	CMD_opt_val_type_t type;
	int optional;
	char brief[CMD_DESCR_LEN];
} CMD_descr_opt_t;

typedef struct
{
	char name[CMD_LINE_LEN];
	char brief[CMD_DESCR_LEN];
	CMD_descr_opt_t options[CMD_MAX_OPTS];
	CMD_opt_pair_t exclusions[CMD_MAX_EXCL];
	CMD_action_t action;

	size_t optionals[CMD_MAX_OPTS];
	size_t nones[CMD_MAX_OPTS];
	size_t typed[CMD_MAX_OPTS];
	size_t o_opts;
	size_t o_excls;
	size_t o_optls;
	size_t o_nones;
	size_t o_typed;
} CMD_descr_cmd_t;

typedef struct
{
	CMD_descr_cmd_t commands[CMD_MAX_CMD];
	int c_cmds;
} CMD_descr_tab_t;

CMD_descr_opt_t CMD_descr_opt(char label, CMD_opt_val_type_t type, int optional, const char *brief);

void CMD_descr_opt_reset(CMD_descr_opt_t *descr);

CMD_descr_opt_t *CMD_descr_opt_copy(CMD_descr_opt_t *dst, const CMD_descr_opt_t *src);

CMD_descr_cmd_t CMD_descr_cmd(const char *name, CMD_action_t action, const char *brief);

void CMD_descr_cmd_reset(CMD_descr_cmd_t *cmd);

CMD_descr_cmd_t *CMD_descr_cmd_copy(CMD_descr_cmd_t *dst, const CMD_descr_cmd_t *src);

char *CMD_descr_usage(const CMD_descr_cmd_t *cmd, char *str);

char *CMD_descr_help(const CMD_descr_cmd_t *cmd, char *str);

int CMD_descr_find_opt(const CMD_descr_cmd_t *cmd, char label);

CMD_err_t *CMD_descr_push_opt(CMD_descr_cmd_t *cmd, CMD_descr_opt_t opt);

CMD_err_t *CMD_descr_push_excl(CMD_descr_cmd_t *cmd, CMD_opt_pair_t excl);

CMD_descr_tab_t CMD_descr_tab();

void CMD_descr_tab_reset(CMD_descr_tab_t *tab);

char *CMD_descr_man(const CMD_descr_tab_t *tab, char *str);

int CMD_descr_find_cmd(const CMD_descr_tab_t *tab, const char *name);

CMD_err_t *CMD_descr_push_cmd(CMD_descr_tab_t *tab, CMD_descr_cmd_t cmd);

#endif //TINY_CMD_DESCR_H

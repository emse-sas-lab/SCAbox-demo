//
// Created by samid on 19/08/2020.
//

#include <cmd/descr.h>


CMD_descr_opt_t CMD_descr_opt(char label, CMD_opt_val_type_t type, int optional, const char *brief)
{
	CMD_descr_opt_t opt;
	CMD_descr_opt_reset(&opt);
	opt.label = label;
	opt.optional = type == CMD_OPT_NONE ? 1 : optional;
	opt.type = type;
	strcpy(opt.brief, brief);
	return opt;
}

void CMD_descr_opt_reset(CMD_descr_opt_t *opt)
{
	opt->label = CMD_NULL_OPT;
	opt->optional = 0;
	opt->type = CMD_OPT_VAL_TYPE_COUNT;
	strcpy(opt->brief, "");
}

CMD_descr_opt_t *CMD_descr_opt_copy(CMD_descr_opt_t *dst, const CMD_descr_opt_t *src)
{
	dst->label = src->label;
	dst->optional = src->optional;
	dst->type = src->type;
	strcpy(dst->brief, src->brief);
	return dst;
}

CMD_descr_cmd_t CMD_descr_cmd(const char *name, CMD_action_t action, const char *brief)
{
	CMD_descr_cmd_t cmd;
	CMD_descr_cmd_reset(&cmd);
	strcpy(cmd.name, name);
	strcpy(cmd.brief, brief);
	cmd.action = action;
	return cmd;
}

void CMD_descr_cmd_reset(CMD_descr_cmd_t *cmd)
{
	strcpy(cmd->name, "");
	strcpy(cmd->brief, "");
	cmd->action = NULL;
	for (size_t o = 0; o < CMD_MAX_OPTS; o++)
	{
		CMD_descr_opt_reset(&cmd->options[o]);
	}
	memset(cmd->optionals, 0, CMD_MAX_OPTS * sizeof(size_t));
	memset(cmd->nones, 0, CMD_MAX_OPTS * sizeof(size_t));
	memset(cmd->typed, 0, CMD_MAX_OPTS * sizeof(size_t));
	for (size_t e = 0; e < CMD_MAX_EXCL; e++)
	{
		cmd->exclusions->first = CMD_NULL_OPT;
		cmd->exclusions->second = CMD_NULL_OPT;
	}
	cmd->o_opts = 0;
	cmd->o_excls = 0;
	cmd->o_optls = 0;
	cmd->o_nones = 0;
	cmd->o_typed = 0;
}

CMD_descr_cmd_t *CMD_descr_cmd_copy(CMD_descr_cmd_t *dst, const CMD_descr_cmd_t *src)
{
	strcpy(dst->name, src->name);
	strcpy(dst->brief, src->brief);
	dst->action = src->action;
	for (size_t o = 0; o < CMD_MAX_OPTS; o++)
	{
		CMD_descr_opt_copy(&dst->options[o], &src->options[o]);
	}
	memcpy(dst->optionals, src->optionals, CMD_MAX_OPTS * sizeof(size_t));
	memcpy(dst->nones, src->nones, CMD_MAX_OPTS * sizeof(size_t));
	memcpy(dst->typed, src->typed, CMD_MAX_OPTS * sizeof(size_t));
	for (int e = 0; e < CMD_MAX_EXCL; e++)
	{
		dst->exclusions[e] = src->exclusions[e];
	}
	dst->o_opts = src->o_opts;
	dst->o_excls = src->o_excls;
	dst->o_optls = src->o_optls;
	dst->o_nones = src->o_nones;
	dst->o_typed = src->o_typed;
	return dst;
}

char *CMD_descr_usage(const CMD_descr_cmd_t *cmd, char *str)
{
	char *ptr = str;
	const CMD_descr_opt_t *opt;
	char buffer[CMD_MAX_OPTS] = "";

	sprintf(ptr, "%-8s ", cmd->name);
	ptr += strlen(ptr);
	for (size_t o = 0; o < cmd->o_nones; o++)
	{
		opt = &cmd->options[cmd->nones[o]];
		buffer[o] = opt->label;
	}
	buffer[cmd->o_nones] = '\0';

	if (cmd->o_nones > 0)
	{
		sprintf(ptr, "[-%s]  ", buffer);
		ptr += strlen(ptr);
	}

	for (size_t o = 0; o < cmd->o_typed; o++)
	{
		opt = &cmd->options[cmd->typed[o]];
		sprintf(ptr, "-%c <%s>  ", opt->label, CMD_opt_type_labels[opt->type]);
		ptr += strlen(ptr);
	}

	for (size_t o = 0; o < cmd->o_optls; o++)
	{
		opt = &cmd->options[cmd->optionals[o]];
		sprintf(ptr, "-%c [<%s>]  ", opt->label, CMD_opt_type_labels[opt->type]);
		ptr += strlen(ptr);
	}
	return str;
}

char *CMD_descr_help(const CMD_descr_cmd_t *cmd, char *str)
{
	const CMD_descr_opt_t *opt;
	char *ptr = str;

	sprintf(ptr, "%s: %-64s", cmd->name, cmd->brief);
	ptr += strlen(ptr);
	for (size_t o = 0; o < cmd->o_typed; o++)
	{
		opt = &cmd->options[cmd->typed[o]];
		sprintf(ptr, "\n-%c <%s>", opt->label, CMD_opt_type_labels[opt->type]);
		sprintf(ptr, "%-16s %-64s", ptr, opt->brief);
		ptr += strlen(ptr);
	}

	for (size_t o = 0; o < cmd->o_nones; o++)
	{
		opt = &cmd->options[cmd->nones[o]];
		sprintf(ptr, "\n[-%c]", opt->label);
		sprintf(ptr, "%-16s %-64s", ptr, opt->brief);
		ptr += strlen(ptr);
	}

	for (size_t o = 0; o < cmd->o_optls; o++)
	{
		opt = &cmd->options[cmd->optionals[o]];
		sprintf(ptr, "\n-%c [<%s>]", opt->label, CMD_opt_type_labels[opt->type]);
		sprintf(ptr, "%-16s %-64s", ptr, opt->brief);
		ptr += strlen(ptr);
	}
	return str;
}

int CMD_descr_find_opt(const CMD_descr_cmd_t *cmd, char label)
{
	for (size_t o = 0; o < cmd->o_opts; o++)
	{
		if (cmd->options[o].label == label)
		{
			return o;
		}
	}
	return -1;
}

CMD_err_t *CMD_descr_push_opt(CMD_descr_cmd_t *cmd, CMD_descr_opt_t opt)
{
	if (cmd->o_opts == CMD_MAX_OPTS)
	{
		return CMD_err(CMD_ERR_DESCR, "descriptor cannot handle more options");
	}

	if (opt.type != CMD_OPT_NONE && !opt.optional)
	{
		cmd->typed[cmd->o_typed++] = cmd->o_opts;
	} else if (opt.type == CMD_OPT_NONE)
	{
		cmd->nones[cmd->o_nones++] = cmd->o_opts;
	} else
	{
		cmd->optionals[cmd->o_optls++] = cmd->o_opts;
	}

	CMD_descr_opt_copy(&cmd->options[cmd->o_opts++], &opt);
	return NULL;
}

CMD_err_t *CMD_descr_push_excl(CMD_descr_cmd_t *cmd, CMD_opt_pair_t excl)
{
	if (cmd->o_excls == CMD_MAX_EXCL)
	{
		return CMD_err(CMD_ERR_DESCR, "descriptor cannot handle more exclusions");
	}
	cmd->exclusions[cmd->o_excls].first = excl.first;
	cmd->exclusions[cmd->o_excls++].second = excl.second;
	return NULL;
}

CMD_descr_tab_t CMD_descr_tab()
{
	CMD_descr_tab_t tab;
	CMD_descr_cmd_t man = CMD_descr_cmd("man", NULL, "Displays the prompt manual or the help for a specific command.");
	CMD_descr_cmd_t quit = CMD_descr_cmd("quit", NULL, "Quits the command prompt.");
	CMD_descr_tab_reset(&tab);
	CMD_descr_push_opt(&man, CMD_descr_opt('c', CMD_OPT_STRING, 1, "Command name."));
	CMD_descr_push_cmd(&tab, man);
	CMD_descr_push_cmd(&tab, quit);
	return tab;
}

void CMD_descr_tab_reset(CMD_descr_tab_t *tab)
{
	for (size_t c = 0; c < CMD_MAX_CMD; c++)
	{
		CMD_descr_cmd_reset(&tab->commands[c]);
	}
	tab->c_cmds = 0;
}

char *CMD_descr_man(const CMD_descr_tab_t *tab, char *str)
{
	char *ptr = str;
	for (size_t c = 0; c < tab->c_cmds; c++)
	{
		CMD_descr_usage(&tab->commands[c], ptr);
		sprintf(ptr, "%-64s\n\t%s\n", ptr, tab->commands[c].brief);
		ptr += strlen(ptr);
	}
	return str;
}

int CMD_descr_find_cmd(const CMD_descr_tab_t *tab, const char *word)
{
	for (size_t c = 0; c < tab->c_cmds; c++)
	{
		if (!strcmp(word, tab->commands[c].name))
		{
			return c;
		}
	}
	return -1;
}

CMD_err_t *CMD_descr_push_cmd(CMD_descr_tab_t *tab, CMD_descr_cmd_t cmd)
{
	if (tab->c_cmds == CMD_MAX_CMD)
	{
		return CMD_err(CMD_ERR_DESCR, "table cannot handle more commands");
	}
	CMD_descr_cmd_copy(&tab->commands[tab->c_cmds++], &cmd);
	return NULL;
}

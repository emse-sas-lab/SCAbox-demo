//
// Created by samid on 21/08/2020.
//

#include <cmd/run.h>


CMD_err_t *CMD_parse_cmd(CMD_cmd_t *cmd, char *str, const CMD_descr_tab_t *tab)
{
	CMD_err_t *err = NULL;
	char *words[CMD_LINE_LEN];
	IO_init_words(words, CMD_LINE_LEN);
	size_t words_count = IO_get_words(IO_trim(str, NULL), words);
	int found, violation;

	if ((found = CMD_descr_find_cmd(tab, words[0])) == -1)
	{
		return CMD_err(CMD_ERR_FORMAT, "unknown command: %s", words[0]);
	}
	strcpy(cmd->name, words[0]);

	if ((err = CMD_parse_opts(cmd->options, &tab->commands[found], words, words_count)) != NULL)
	{
		return err;
	}

	if ((violation = CMD_check_excl(cmd->options, &tab->commands[found])) != -1)
	{
		CMD_opt_pair_t excl = tab->commands[found].exclusions[violation];
		return CMD_err(CMD_ERR_USAGE, "exclusive options: -%c, -%c", excl.first, excl.second);
	}
	if ((violation = CMD_check_optl(cmd->options, &tab->commands[found])) != -1)
	{
		char label = tab->commands[found].options[violation].label;
		return CMD_err(CMD_ERR_USAGE, "option required -%c", label);
	}

	return err;
}

CMD_err_t *CMD_parse_opt(CMD_opt_t *opt, const CMD_descr_opt_t *descr, char *word, size_t len)
{
	CMD_err_t *err;
	CMD_opt_reset(opt);
	opt->label = descr->label;
	opt->type = descr->type;

	if ((word == NULL || word[0] == '-' || strlen(word) == 0) && opt->type != CMD_OPT_NONE)
	{
		return CMD_err(CMD_ERR_USAGE, "option -%c expects a value", descr->label);
	}

	if (word != NULL && word[0] != '-' && strlen(word) > 0 && opt->type == CMD_OPT_NONE)
	{
		return CMD_err(CMD_ERR_USAGE, "option -%c does not expect a value", descr->label);
	}

	switch (descr->type)
	{
		case CMD_OPT_NONE:
			break;
		case CMD_OPT_INT:
			if ((err = CMD_set_int(&opt->value, word, len)) != NULL)
			{
				return err;
			}
			break;
		case CMD_OPT_BYTES:
			if ((err = CMD_set_bytes(&opt->value, word, len)) != NULL)
			{
				return err;
			}
			break;
		case CMD_OPT_STRING:
			strcpy(opt->value.string, word);
			break;
		default:
			return CMD_err(CMD_ERR_FORMAT, "unknown descriptor type");
	}
	return NULL;
}

CMD_err_t *CMD_parse_opts(CMD_opt_t opts[], const CMD_descr_cmd_t *descr, char *words[], size_t len)
{
	size_t curr = 1, next = 2;
	size_t o = 0, l;
	CMD_err_t *err;
	int found;
	char *word;
	while (curr < len)
	{
		word = next < len ? words[next] : NULL;
		l = next < len ? strlen(words[next]) : 0;
		if (words[curr][0] != '-')
		{
			return CMD_err(CMD_ERR_USAGE, "options must start with -");
		}
		if ((found = CMD_descr_find_opt(descr, words[curr][1])) == -1)
		{
			return CMD_err(CMD_ERR_USAGE, "unknown option: %s", words[curr]);
		}
		if ((err = CMD_parse_opt(&opts[o], &descr->options[found], word, l)) != NULL)
		{
			return err;
		}
		if (opts[o].type != CMD_OPT_NONE)
		{
			curr++;
			next++;
		}
		o++;
		curr++;
		next++;
	}
	return NULL;
}

int CMD_check_excl(const CMD_opt_t opts[], const CMD_descr_cmd_t *descr)
{
	CMD_opt_pair_t excl;
	char label1, label2;
	for (size_t o_cmd_1 = 0; opts[o_cmd_1].label != CMD_NULL_OPT; o_cmd_1++)
	{
		for (size_t o_cmd_2 = 0; opts[o_cmd_2].label != CMD_NULL_OPT; o_cmd_2++)
		{
			for (size_t e = 0; e < descr->o_excls; e++)
			{
				excl = descr->exclusions[e];
				label1 = opts[o_cmd_1].label;
				label2 = opts[o_cmd_2].label;
				if (excl.first == label1 && excl.second == label2)
				{
					return e;
				}
			}
		}
	}
	return -1;
}

int CMD_check_optl(const CMD_opt_t opts[], const CMD_descr_cmd_t *descr)
{
	for (size_t o = 0; o < descr->o_opts; o++)
	{
		if (CMD_opt_find(opts, descr->options[o].label) == -1 && !descr->options[o].optional)
		{
			return o;
		}
	}
	return -1;
}

CMD_err_t *CMD_run(const CMD_descr_tab_t *tab)
{
	char line[CMD_LINE_LEN], buffer[CMD_LINE_LEN];
	CMD_cmd_t cmd;
	CMD_err_t *error = NULL;
	int found;
	CMD_cmd_reset(&cmd);
	do
	{
		strcpy(buffer, line);
		IO_clear_line(line, CMD_LINE_LEN);
		CMD_cmd_reset(&cmd);
		printf("> ");

		if (IO_get_line(line, CMD_LINE_LEN) != IO_SUCCESS)
		{
			return CMD_err(CMD_ERR_IO, "read error: errno=%d\n", errno);
		}

		if (strlen(line) < 3)
		{
			continue;
		}

		if (line[0] == '\033' && line[2] == (char) IO_KEYCODE_UP_ARROW)
		{
			fprintf(stdin, "%s", buffer);
			strcpy(line, buffer);
		}

		strcpy(buffer, line);
		if ((error = CMD_parse_cmd(&cmd, buffer, tab)) != NULL)
		{
			fprintf(stderr, "%s\n", error->message);
			if (error->status == CMD_ERR_USAGE)
			{
				found = CMD_descr_find_cmd(tab, cmd.name);
				printf("%s\n", CMD_descr_usage(&tab->commands[found], buffer));
			}
			free(error);
			continue;
		}

		if (!strcmp("man", cmd.name))
		{
			if ((found = CMD_opt_find(cmd.options, 'c')) == -1)
			{
				printf("%s\n", CMD_descr_man(tab, buffer));
				continue;
			} else if ((found = CMD_descr_find_cmd(tab, cmd.options[found].value.string)) == -1)
			{
				printf("%s\n", CMD_descr_usage(&tab->commands[0], buffer));
				continue;
			}
			printf("%s\n", CMD_descr_help(&tab->commands[found], buffer));
			continue;
		}

		if (!strcmp("quit", cmd.name))
		{
			return NULL;
		}

		if (tab->commands[(found = CMD_descr_find_cmd(tab, cmd.name))].action != NULL)
		{
			if ((error = tab->commands[found].action(&cmd)) != NULL)
			{
				fprintf(stderr, "%s\n", error->message);
				free(error);
				continue;
			}
		}
	} while (1);
}

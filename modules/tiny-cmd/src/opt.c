//
// Created by samid on 19/08/2020.
//

#include <cmd/opt.h>


void CMD_opt_val_reset(CMD_opt_val_t *val)
{
	for (size_t i = 0; i < CMD_LINE_LEN; i++)
	{
		val->bytes[i] = 0;
	}
}

CMD_err_t *CMD_set_int(CMD_opt_val_t *val, char *word, size_t len)
{
	errno = 0;
	CMD_err_t *err = NULL;
	char *end = &word[len];
	int num = strtol(word, &end, 10);
	if ((errno != 0 && num == 0) || end != word + len)
	{
		val->integer = 0;
		return CMD_err(CMD_ERR_FORMAT, "invalid format for %s: %s", CMD_opt_type_labels[CMD_OPT_INT], word);
	}
	val->integer = num;
	return err;
}

CMD_err_t *CMD_set_bytes(CMD_opt_val_t *val, char *word, size_t len)
{
	CMD_err_t *err = NULL;

	errno = 0;
	size_t c_init = word[1] != 'x' ? 0 : 2, b;
	char buffer[3] = "\0\0";
	char *end = NULL;
	for (size_t c = c_init; c < len; c += 2)
	{
		buffer[0] = word[c];
		buffer[1] = word[c + 1];
		end = buffer + 2;
		b = (c - c_init) >> 1;
		val->bytes[b] = strtol(buffer, &end, 16);
		if ((errno != 0 && val->bytes[b] == 0) || end != buffer + 2)
		{
			memset(val->bytes, 0, CMD_LINE_LEN);
			return CMD_err(CMD_ERR_FORMAT, "invalid format for %s: %s", CMD_opt_type_labels[CMD_OPT_BYTES], word);
		}
	}
	return err;
}

CMD_opt_t CMD_opt(char label, CMD_opt_val_type_t type)
{
	CMD_opt_t opt;
	CMD_opt_reset(&opt);
	opt.label = label;
	opt.type = type;
	return opt;
}

void CMD_opt_reset(CMD_opt_t *opt)
{
	CMD_opt_val_reset(&opt->value);
	opt->label = CMD_NULL_OPT;
	opt->type = CMD_OPT_VAL_TYPE_COUNT;
}

CMD_opt_t *CMD_opt_copy(CMD_opt_t *dst, const CMD_opt_t *src)
{
	dst->label = src->label;
	dst->type = src->type;
	memcpy(dst->value.bytes, src->value.bytes, CMD_LINE_LEN);
	return dst;
}

int CMD_opt_find(const CMD_opt_t opts[], char label)
{
	for (int o = 0; o < CMD_MAX_OPTS; o++)
	{
		if (opts[o].label != label)
		{
			continue;
		}
		return o;
	}
	return -1;
}

CMD_opt_pair_t CMD_opt_pair(char first, char second)
{
	CMD_opt_pair_t pair;
	pair.first = first;
	pair.second = second;
	return pair;
}

void CMD_opt_pair_reset(CMD_opt_pair_t *pair)
{
	pair->first = CMD_NULL_OPT;
	pair->second = CMD_NULL_OPT;
}
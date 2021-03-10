#include <stdio.h>
#include <cmd/hex.h>
#include <cmd/run.h>

CMD_err_t *test(const CMD_cmd_t *cmd)
{
	char buffer[CMD_LINE_LEN];
	for (size_t o = 0; cmd->options[o].label != CMD_NULL_OPT; o++)
	{
		printf("opt -%c\n", cmd->options[o].label);
		printf("type: %s\n", CMD_opt_type_labels[cmd->options[o].type]);
		printf("integer: %lld\n", cmd->options[o].value.integer);
		printf("bytes: %s\n", HEX_bytes_to_string(buffer, cmd->options[o].value.bytes, 16));
		printf("words: %s\n", HEX_words_to_string(buffer, cmd->options[o].value.words, 4));
		printf("string: %s\n", cmd->options[o].value.string);
	}
	return NULL;
}

int main()
{
	CMD_err_t *error;
	CMD_descr_cmd_t test_cmd = CMD_descr_cmd("test", &test, "test command");
	CMD_descr_cmd_t other_cmd = CMD_descr_cmd("other", NULL, "other command with a very long description you know!");
	CMD_descr_tab_t tab = CMD_descr_tab();

	CMD_descr_push_opt(&test_cmd, CMD_descr_opt('a', CMD_OPT_INT, 0, "option1"));
	CMD_descr_push_opt(&test_cmd, CMD_descr_opt('b', CMD_OPT_STRING, 1, "option2"));
	CMD_descr_push_opt(&test_cmd, CMD_descr_opt('c', CMD_OPT_NONE, 1, "option3"));
	CMD_descr_push_opt(&test_cmd, CMD_descr_opt('d', CMD_OPT_NONE, 1, "option4"));
	CMD_descr_push_opt(&test_cmd, CMD_descr_opt('e', CMD_OPT_BYTES, 1, "option5"));
	CMD_descr_push_opt(&test_cmd, CMD_descr_opt('f', CMD_OPT_BYTES, 1, "option6"));
	CMD_descr_push_excl(&test_cmd, CMD_opt_pair('c', 'd'));

	CMD_descr_push_cmd(&tab, test_cmd);

	CMD_descr_push_opt(&other_cmd, CMD_descr_opt('x', CMD_OPT_NONE, 1, "option1"));
	CMD_descr_push_opt(&other_cmd, CMD_descr_opt('z', CMD_OPT_NONE, 1, "option2"));

	CMD_descr_push_cmd(&tab, other_cmd);

	if ((error = CMD_run(&tab)) != NULL)
	{
		fprintf(stderr, "%s\n", error->message);
		free(error);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
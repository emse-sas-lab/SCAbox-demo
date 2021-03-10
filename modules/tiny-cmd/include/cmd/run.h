//
// Created by samid on 21/08/2020.
//

#ifndef CMD_RUN_H
#define CMD_RUN_H

#include <cmd/io.h>
#include <cmd/err.h>
#include <cmd/descr.h>
#include <cmd/cmd.h>


CMD_err_t *CMD_parse_cmd(CMD_cmd_t *cmd, char *str, const CMD_descr_tab_t *tab);

CMD_err_t *CMD_parse_opt(CMD_opt_t *opt, const CMD_descr_opt_t *descr, char *word, size_t len);

CMD_err_t *CMD_parse_opts(CMD_opt_t opts[], const CMD_descr_cmd_t *descr, char *words[], size_t len);

int CMD_check_excl(const CMD_opt_t opts[], const CMD_descr_cmd_t *descr);

int CMD_check_optl(const CMD_opt_t opts[], const CMD_descr_cmd_t *descr);

CMD_err_t *CMD_run(const CMD_descr_tab_t *tab);

#endif //CMD_RUN_H

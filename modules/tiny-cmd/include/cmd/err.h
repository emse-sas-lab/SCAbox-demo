//
// Created by samid on 19/08/2020.
//

#ifndef TINY_CMD_ERR_H
#define TINY_CMD_ERR_H

#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <cmd/consts.h>


/**
 * @brief enumerations of errors
 */
typedef enum
{
	CMD_ERR_USAGE,
	CMD_ERR_FORMAT,
	CMD_ERR_DESCR,
	CMD_ERR_IO
} CMD_err_status_t;

typedef struct
{
	CMD_err_status_t status;
	char message[CMD_LINE_LEN];
} CMD_err_t;

CMD_err_t *CMD_err(CMD_err_status_t status, char *message, ...);

#endif //TINY_CMD_ERR_H

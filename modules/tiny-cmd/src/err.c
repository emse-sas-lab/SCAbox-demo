//
// Created by samid on 19/08/2020.
//

#include <cmd/err.h>


CMD_err_t *CMD_err(CMD_err_status_t status, char *message, ...)
{
	va_list params;
	CMD_err_t *err = malloc(sizeof(CMD_err_t));
	err->status = status;
	va_start(params, message);
	vsprintf(err->message, message, params);
	va_end(params);
	return err;
}

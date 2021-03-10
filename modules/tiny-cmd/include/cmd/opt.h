//
// Created by samid on 19/08/2020.
//

#ifndef TINY_CMD_OPT_H
#define TINY_CMD_OPT_H

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <cmd/consts.h>
#include <cmd/err.h>


/**
 * @brief handled option value type
 */
typedef enum
{
	CMD_OPT_NONE,
	CMD_OPT_INT, /** base 10 `int` value */
	CMD_OPT_BYTES, /** base 16 unconstrained integer value as bytes */
	CMD_OPT_STRING, /** string value */
	CMD_OPT_VAL_TYPE_COUNT   /** count of available options value types */
} CMD_opt_val_type_t;

/**
 * @brief variable type value for the option
 *
 * The members accessed must be compatible with the option value type `CMD_opt_val_type_t`.
 */
typedef union
{
	char string[CMD_LINE_LEN];         /** string component of the value */
	long long integer;                  /** integer component of the value */
	uint8_t bytes[CMD_LINE_LEN];       /** raw bytes of the value*/
	uint32_t words[CMD_LINE_LEN >> 2]; /** 32-bit raw words of the value*/
} CMD_opt_val_t;

/**
 * @brief option handling structure
 */
typedef struct
{
	char label;              /** character labeling the option */
	CMD_opt_val_t value;     /** value of the option argument */
	CMD_opt_val_type_t type; /** type for the option argument */
} CMD_opt_t;

typedef struct
{
	char first;
	char second;
} CMD_opt_pair_t;

void CMD_opt_val_reset(CMD_opt_val_t *val);

CMD_err_t *CMD_set_int(CMD_opt_val_t *val, char *str, size_t len);

CMD_err_t *CMD_set_bytes(CMD_opt_val_t *val, char *str, size_t len);

void CMD_set_string(CMD_opt_val_t *val, char *str);

CMD_opt_t CMD_opt(char label, CMD_opt_val_type_t type);

void CMD_opt_reset(CMD_opt_t *opt);

CMD_opt_t *CMD_opt_copy(CMD_opt_t *dst, const CMD_opt_t *src);

int CMD_opt_find(const CMD_opt_t opts[], char label);

CMD_opt_pair_t CMD_opt_pair(char first, char second);

void CMD_opt_pair_reset(CMD_opt_pair_t *pair);

static char *CMD_opt_type_labels[CMD_OPT_VAL_TYPE_COUNT] = {
		"none",
		"int",
		"bytes",
		"str"
};

#endif //TINY_CMD_OPT_H

/**
 * @file io.h
 * @author Sami Dahoux (s.dahoux@emse.fr)
 * @brief Module to perform safe reading into stdin
 */

#ifndef TINY_CMD_IO_H
#define TINY_CMD_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum
{
	IO_SUCCESS = -1,
	IO_FAILURE = -2
} IO_status_t;

typedef enum
{
	IO_KEYCODE_UP_ARROW = 'A',
	IO_KEYCODE_DOWN_ARROW = 'B',
	IO_KEYCODE_RIGHT_ARROW = 'C',
	IO_KEYCODE_LEFT_ARROW = 'D',

} IO_keycode_t;

/**
 * @brief Clears the first `len` chars of the given buffer by adding '\0' chars
 * @param line buffer to be cleared
 * @param len count of chars to clear, must not exceed the allocated size for `line`
 */
void IO_clear_line(char *line, size_t len);

/**
 * @brief Clears the given array of words, words[i][j] will contain `NULL` pointer
 * @param words array of words ie. array of strings without ' ' chars
 * @param len count of words contained in the array
 */
void IO_init_words(char *words[], size_t len);

/**
 * @brief Get a line from stdin
 * @param buffer line string
 * @param len buffer length, maximum count of chars to parse
 */
IO_status_t IO_get_line(char *buffer, size_t len);

/**
 * @brief Splits a string according to the ' ' delimiter char
 * @param line line to split
 * @param words words of the splitted line
 */
size_t IO_get_words(char *line, char *words[]);

char *IO_ltrim(char *str, const char *seps);

char *IO_rtrim(char *str, const char *seps);

char *IO_trim(char *str, const char *seps);

#endif //TINY_CMD_IO_H

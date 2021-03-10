//
// Created by Sami Dahoux (s.dahoux@emse.fr) on 26/06/2020.
//

#include <cmd/io.h>


void IO_clear_line(char *line, size_t len)
{
	for (size_t i = 0; i < len; i++)
	{
		line[i] = '\0';
	}
}

void IO_init_words(char *words[], size_t len)
{
	for (size_t w = 0; w < len; w++)
	{
		words[w] = NULL;
	}
}

IO_status_t IO_get_line(char *buffer, size_t len)
{
	size_t i = 0;

	fflush(stdin);
	do
	{
		if ((buffer[i] = (char) getchar()) == EOF)
		{
			return IO_FAILURE;
		}
	} while (buffer[i] != '\n' && buffer[i] != '\r' && i++ < len);
	buffer[i] = '\0';
	return IO_SUCCESS;
}

size_t IO_get_words(char *line, char *words[])
{
	char *token = strtok(line, " ");
	char *ret_char;
	size_t w = 0;
	for (; token != NULL; w++)
	{
		words[w] = token;
		if ((ret_char = strchr(words[w], '\n')) != NULL ||
			(ret_char = strchr(words[w], '\r')) != NULL)
			*ret_char = '\0';
		token = strtok(NULL, " ");
	}
	return w;
}

char *IO_ltrim(char *str, const char *seps)
{
	size_t to_trim;
	if (seps == NULL)
	{
		seps = "\t\n\v\f\r ";
	}
	to_trim = strspn(str, seps);
	if (to_trim > 0)
	{
		size_t len = strlen(str);
		if (to_trim == len)
		{
			str[0] = '\0';
		} else
		{
			memmove(str, str + to_trim, len + 1 - to_trim);
		}
	}
	return str;
}

char *IO_rtrim(char *str, const char *seps)
{
	long int i;
	if (seps == NULL)
	{
		seps = "\t\n\v\f\r ";
	}
	i = (long int) strlen(str) - 1;
	while (i >= 0 && strchr(seps, str[i]) != NULL)
	{
		str[i] = '\0';
		i--;
	}
	return str;
}

char *IO_trim(char *str, const char *seps)
{
	return IO_ltrim(IO_rtrim(str, seps), seps);
}
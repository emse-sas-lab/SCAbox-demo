/**
 * @file hex.h
 * @author Sami Dahoux (s.dahoux@emse.fr)
 * @brief Module to perform 8-bits and 32-bits raw arrays serialization and conversions
 */

#ifndef TINY_CMD_HEX_H
#define TINY_CMD_HEX_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


char *HEX_bytes_to_string(char *str, const uint8_t *bytes, size_t len);

char *HEX_words_to_string(char *str, const uint32_t *words, size_t len);

/**
 * @brief Copy `uint8_t` array to `uint32_t` array
 * @param words destination words
 * @param bytes source bytes
 * @param len length of `bytes` buffer
 */
uint32_t *HEX_bytes_to_words(uint32_t *words, const uint8_t *bytes, size_t len);

/**
 * @brief Copy `uint32_t` array to `uint8_t` array
 * @param bytes destination bytes
 * @param words source words
 * @param len length of `bytes` buffer
 */
uint8_t *HEX_words_to_bytes(uint8_t *bytes, const uint32_t *words, size_t len);

/**
 * @brief Generates uniformly distributed `uint32_t` array
 * @param words random words
 * @param seed random seed
 * @param len `words` length
 */
uint32_t *HEX_random_words(uint32_t *words, int seed, size_t len);

/**
 * @brief Generates zero `uint32_t` array
 * @param words random words
 * @param len `words` length
 */
uint32_t *HEX_zeros_words(uint32_t *words, size_t len);
#endif //TINY_CMD_HEX_H

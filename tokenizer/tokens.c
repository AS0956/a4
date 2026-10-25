/**
 * A tokenizer for a simple shell language.
 *
 * Tokens:
 *   ( ) < > ; |      single-character special tokens
 *   "some text"      a quoted string; special characters lose their meaning
 *                    inside quotes, and the quotes are not part of the token
 *   ls, foo_bar      a word: a sequence of non-special characters
 *
 * Whitespace (space, tab, newline) is not a token, it only separates tokens.
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

/** Is the given character whitespace (space, tab or newline)? */
static int is_whitespace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n';
}

/** Is the given character a single-character special token? */
static int is_special_token(char ch) {
  return ch == '(' || ch == ')' || ch == '<' || ch == '>' ||
         ch == ';' || ch == '|';
}

/** Can the given character be part of a word? */
static int is_word_char(char ch) {
  return ch != '\0' && ch != '"' && !is_whitespace(ch) && !is_special_token(ch);
}

/**
 * Add the first len characters of start to the vector as a new token.
 * The vector makes its own copy, so our temporary buffer is freed here.
 */
static void add_token(vect_t *tokens, const char *start, size_t len) {
  char *token = malloc(len + 1);
  assert(token != NULL);

  memcpy(token, start, len);
  token[len] = '\0';

  vect_add(tokens, token);
  free(token);
}

/**
 * Read a word starting at input[0] and add it to tokens.
 * Returns the number of characters consumed.
 */
static size_t read_word(const char *input, vect_t *tokens) {
  size_t len = 0;
  while (is_word_char(input[len])) {
    len++;
  }
  add_token(tokens, input, len);
  return len;
}

/**
 * Read a quoted string starting at the opening quote input[0] and add its
 * contents (without the quotes) to tokens. If there is no closing quote, the
 * string runs to the end of the input (ignoring a trailing newline).
 * Returns the number of characters consumed, including the quotes.
 */
static size_t read_string(const char *input, vect_t *tokens) {
  assert(input[0] == '"');

  const char *start = input + 1;   /* skip the opening quote */
  const char *end = strchr(start, '"');

  if (end != NULL) {
    add_token(tokens, start, end - start);
    return (end - input) + 1;      /* +1 for the closing quote */
  }

  /* unterminated string: take the rest of the line */
  size_t len = strlen(start);
  size_t consumed = len + 1;       /* opening quote + rest of input */
  if (len > 0 && start[len - 1] == '\n') {
    len--;
  }
  add_token(tokens, start, len);
  return consumed;
}

/** Split the given string into a vector of tokens. The caller must delete
 *  the returned vector with vect_delete. */
vect_t *tokenize(const char *input) {
  assert(input != NULL);

  vect_t *tokens = vect_new();
  size_t i = 0;

  while (input[i] != '\0') {
    char ch = input[i];

    if (is_whitespace(ch)) {
      i++;                                   /* skip whitespace */
    } else if (is_special_token(ch)) {
      add_token(tokens, &input[i], 1);
      i++;
    } else if (ch == '"') {
      i += read_string(&input[i], tokens);
    } else {
      i += read_word(&input[i], tokens);
    }
  }

  return tokens;
}

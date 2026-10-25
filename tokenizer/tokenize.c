/**
 * Driver for the shell tokenizer: reads one line from standard input and
 * prints every token on its own line.
 */
#include <stdio.h>

#include "../vector/vect.h"
#include "tokens.h"

#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
  char buffer[BUFFER_SIZE];

  /* fgets returns NULL on end-of-file (or error) with nothing read */
  if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
    return 0;
  }

  vect_t *tokens = tokenize(buffer);

  for (unsigned int i = 0; i < vect_size(tokens); i++) {
    printf("%s\n", vect_get(tokens, i));
  }

  vect_delete(tokens);
  return 0;
}

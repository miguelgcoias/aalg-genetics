#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/** String matching algorithms **/
void naive(char *text, char *pattern, size_t text_sz, size_t pattern_sz);
void kmp(char *text, char *pattern, size_t text_sz, size_t pattern_sz);
void bm();

/** Helper functions **/
char *read();
void kmpTable(size_t *table, char *pattern, size_t pattern_sz);

int main() {
  char *text = NULL, *pattern = NULL;
  size_t text_sz = 0, pattern_sz = 0;
  int command = getc(stdin);

  while (command != 'X') {
    switch (command) {
      case 'T': {
        if (text) {
          free(text);
          text = NULL;
        }
        text = read(&text_sz);
        break;
      }
      case 'N': {
        if (pattern) {
          free(pattern);
          pattern = NULL;
        }
        pattern = read(&pattern_sz);
        assert(pattern && text);
        assert(pattern_sz > 0 && text_sz > 0);
        naive(text, pattern, text_sz, pattern_sz);
        break;
      }
      case 'K': {
        if (pattern) {
          free(pattern);
          pattern = NULL;
        }
        pattern = read(&pattern_sz);
        assert(pattern && text);
        assert(pattern_sz > 0 && text_sz > 0);
        kmp(text, pattern, text_sz, pattern_sz);
        break;
      }
      case 'B': {
        break;
      }
      default:
        printf("Invalid command\n");
        break;
    }

    command = getc(stdin);
  }

  return EXIT_SUCCESS;
}

char *read(size_t *str_sz) {
  /* Skip trailing whitespace */
  getc(stdin);

  char *str = NULL, *tmp = NULL;
  size_t buffer_sz = 0;
  int ch = getc(stdin);
  *str_sz = 0;
  
  while (ch != '\n') {
    /* Memory allocated BUFSIZ bytes at a time */
    if (buffer_sz <= *str_sz) {
      buffer_sz += BUFSIZ;
      tmp = realloc(str, buffer_sz);
      /* To do: deal with eventual situations where this happens */
      if (!tmp) {
        free(str);
        str = NULL;
        break;
      }
      str = tmp;
    }
    str[*str_sz] = ch;
    *str_sz = *str_sz + 1;
    ch = getc(stdin);
  }
  return str;
}

void naive(char *text, char *pattern, size_t text_sz, size_t pattern_sz) {
  size_t i, j;

  for (i = 0; i < text_sz - pattern_sz; i++) {
    for (j = 0; j < pattern_sz; j++) {
      if (text[i+j] != pattern[j])
        break;
    }
    if (j == pattern_sz)
      printf("%zu ", i);
  }
  printf("\n");
}

void kmp(char *text, char *pattern, size_t text_sz, size_t pattern_sz) {
  size_t i, k = 0;
  size_t table[pattern_sz];

  kmpTable(table, pattern, pattern_sz);

  for (i = 0; i < text_sz; i++) {
    /* printf("i: %zu\n", i);
    printf("k: %zu\n", k); */
    while (k > 0 && pattern[k] != text[i])
      k = table[k-1];
    if (pattern[k] == text[i])
      k++;
    if (k == pattern_sz) {
      /* printf("k: %zu \n", k); */
      k = table[k-1];
      printf("%zu ", i - pattern_sz + 1);
      /* printf("i: %zu \n", i); */
    }
  }
  /* printf("Pattern size: %zu\n", pattern_sz); */
  printf("\n");
}

void kmpTable(size_t *table, char *pattern, size_t pattern_sz) {
  size_t i, k = 0;

  table[0] = 0;
  /* printf("%zu ", table[0]); */

  for (i = 1; i < pattern_sz; i++) {
    while (k > 0 && pattern[k] != pattern[i])
      k = table[k-1];
    if (pattern[k] == pattern[i])
      k++;
    table[i] = k;
    /* printf("%zu ", table[i]); */
  }
  /* printf("\n"); */
}

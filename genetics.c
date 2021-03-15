#include <stdio.h>
#include <stdlib.h>

/** String matching algorithms **/
/* Naive algorithm */
void naive(char *text, char *pattern, size_t text_sz, size_t pattern_sz);
/* Knuth-Morris-Pratt algorithm */
void kmp(char *text, char *pattern, size_t *lps, size_t text_sz, size_t pattern_sz);
/* Boyer-Moore algorithm */
void bm();

/** Helper functions **/
/* Read from stdin */
char *read();
/* Construct prefix/suffix table for Knuth-Morris-Pratt algorithm*/
void constructlps(size_t *lps, char *pattern, size_t pattern_sz);

int main() {
  char *text = NULL, *pattern;
  int command = getc(stdin);
  size_t text_sz, pattern_sz;

  while (command != 'X') {
    switch (command) {
      case 'T': {
        /* Free memory used for previous text */
        if (text) {
          free(text);
          text = NULL;
        }
        text_sz = 0;
        text = read(&text_sz);
        break;
      }
      /* Naive case */
      case 'N': {
        pattern = read(&pattern_sz);
        naive(text, pattern, text_sz, pattern_sz);
        break;
      }
      /* Knuth-Morris-Pratt case */
      case 'K': {
        pattern = read(&pattern_sz);
        size_t lps[pattern_sz];
        constructlps(lps, pattern, pattern_sz);
        kmp(text, pattern, lps, text_sz, pattern_sz);
        break;
      }
      /* Boyer-Moore case */
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

  *str_sz = 0;

  char *str = NULL, *tmp = NULL;
  size_t buffer_sz = 0;
  int ch = getc(stdin);
  
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

void kmp(char *text, char *pattern, size_t *lps, size_t text_sz, size_t pattern_sz) {
  size_t i = 0, j = 0, n_comp = 0;

  while (i < text_sz) {
    if (j == pattern_sz) {
      printf("%zu ", i - j);
      j = lps[j-1];
      n_comp++;
    }

    if (text[i] == pattern[j]) {
      i++;
      j++;
    }
    else {
      if (j > 0)
        j--;
      else
        i++;
    }

    n_comp++;
  }

  if (j == pattern_sz)
    printf("%zu ", i - j);
  
  printf("\n%zu\n", n_comp);
}

void constructlps(size_t *lps, char *pattern, size_t pattern_sz) {
  lps[0] = 0;

  if (pattern_sz > 1) {
    size_t i;
    for (i = 1; i < pattern_sz; i++) {
      if (pattern[i] == pattern[lps[i-1]])
        lps[i] = lps[i-1] + 1;
      else
        lps[i] = 0;
    }
  }
}

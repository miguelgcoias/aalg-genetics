#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/** String matching algorithms **/
void naive(char *text, char *pattern, size_t text_sz, size_t pattern_sz);
void kmp(char *text, char *pattern, size_t text_sz, size_t pattern_sz);
void bm(char *text, char *pattern, size_t text_sz, size_t pattern_sz);

/** Helper function **/
char *read(size_t *str_sz);

int main() {
  char *text = NULL, *pattern = NULL;
  size_t text_sz = 0, pattern_sz = 0;
  int command = getc(stdin);

  while (command != 'X') {
    if (pattern) {
      free(pattern);
      pattern = NULL;
    }
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
        pattern = read(&pattern_sz);
        assert(pattern && text && pattern_sz && text_sz);
        naive(text, pattern, text_sz, pattern_sz);
        break;
      }
      case 'K': {
        pattern = read(&pattern_sz);
        assert(pattern && text && pattern_sz && text_sz);
        kmp(text, pattern, text_sz, pattern_sz);
        break;
      }
      case 'B': {
        pattern = read(&pattern_sz);
        assert(pattern && text && pattern_sz && text_sz);
        bm(text, pattern, text_sz, pattern_sz);
        break;
      }
      default:
        printf("Invalid command\n");
        break;
    }

    command = getc(stdin);
  }

  if (text) {
    free(text);
    text = NULL;
  }

  return 0;
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

  for (i = 0; i < text_sz - pattern_sz + 1; i++) {
    for (j = 0; j < pattern_sz; j++) {
      if (text[i + j] != pattern[j])
        break;
    }
    if (j == pattern_sz)
      printf("%zu ", i);
  }
  printf("\n");
}

void kmp(char *text, char *pattern, size_t text_sz, size_t pattern_sz) {
  size_t i;
  size_t j;
  size_t table[pattern_sz];
  size_t nc;

  table[0] = 0;
  j = 0;
  for (i = 1; i < pattern_sz; i++) {
    while (j > 0 && pattern[j] != pattern[i])
      j = table[j - 1];
    if (pattern[j] == pattern[i])
      j++;
    table[i] = j;
  }

  nc = 0;
  j = 0;
  for (i = 0; i < text_sz; i++) {
    while (j > 0 && pattern[j] != text[i]) {
      j = table[j - 1];
      nc++;
    }
    if (pattern[j] == text[i]) {
      j++;
    }
    nc++;
    if (j == pattern_sz) {
      printf("%zu ", i - pattern_sz + 1);
      if (i != text_sz - 1) {
        j = table[j - 1];
        nc++;
      }
    }
  }
  printf("\n%zu \n", nc);
}

void bm(char *text, char *pattern, size_t text_sz, size_t pattern_sz) {
  size_t i;
  size_t nc = 0;

  /* bc holds the right-most position of every character, or (size_t) -1 if it
  does not appear in the pattern */
  size_t bc[256];
  size_t ch;
  for (i = 0; i < 256; i++) {
    bc[i] = -1;
  }
  for (i = 0; i < pattern_sz; i++) {
    ch = (size_t) pattern[i];
    bc[ch] = (i > bc[ch] || bc[ch] == -1) ? i : bc[ch];
  }

  size_t z[pattern_sz]; /* Z values */
  size_t left = 0; /* Left box endpoint */
  size_t right = 0; /* Right box endpoint */
  size_t j; /* Auxiliary variable for loops */
  char pattern_r[pattern_sz]; /* Reversed pattern */

  size_t sl[pattern_sz];
  size_t bl[pattern_sz];

  /* Reverse pattern */
  for (j = 0; j < pattern_sz; j++)
    pattern_r[pattern_sz - j - 1] = pattern[j];

  z[0] = pattern_sz;
  for (i = 1; i < pattern_sz; i++) {
    if (i > right) {
      j = 0;
      while (i + j < pattern_sz && pattern_r[j] == pattern_r[j + i])
        j++;
      z[i] = j;
      if (j > 0) {
        left = i;
        right = i + j - 1;
      }
    }
    else {
      if (z[i - left] < right - i + 1)
        z[i] = z[i - left];
      else {
        j = right + 1;
        while (j < pattern_sz && pattern_r[j] == pattern_r[j - i])
          j++;
        z[i] = j - i;
        left = i;
        right = j - 1;
      }
    }
  }

  /* Reverse z in place */
  size_t temp;
  for (j = 0; j < pattern_sz/2; j++) {
    temp = z[pattern_sz - j - 1];
    z[pattern_sz - j - 1] = z[j];
    z[j] = temp;
  }
  
  for (j = 0; j < pattern_sz; j++) {
    bl[j] = 0;
    sl[j] = 0;
  }

  for (j = 0; j < pattern_sz - 1; j++) {
    if (z[j] != 0)
      bl[pattern_sz - z[j]] = j;
  }

  /* Fix sl, z[j - 1] == j */
  /* sl[pattern_sz - 1] = z[0]; */
  size_t sl_max = 0;
  for (j = 1; j < pattern_sz; j++) {
    sl_max = (z[j - 1] == j) ? j : sl_max;
    sl[pattern_sz - j] = sl_max;
  }

  size_t bc_skip;
  size_t gs_skip;
  for (i = 0; i < text_sz - pattern_sz + 1; i++) {
    j = 0;
    while (j < pattern_sz && pattern[pattern_sz - 1 - j] == text[i + pattern_sz - 1 - j]) {
      j++;
      nc++;
    }
    if (j != pattern_sz) {
      nc++;
      ch = (size_t) text[i + pattern_sz - 1 - j];

      /* Character does not exist in the pattern */
      if (bc[ch] == -1)
        bc_skip = pattern_sz - 1 - j;
      /* Character is at the left of the current position */
      else if (pattern_sz - 1 - j > bc[ch])
        bc_skip = pattern_sz - 1 - j - bc[ch] - 1;
      /* Character is at the right of the current position */
      else
        bc_skip = 0;
      
      /* This part is likely broken */
      if (j == 0)
        gs_skip = 0;
      else if (bl[pattern_sz - j] > 0)
        gs_skip = pattern_sz - 2 - bl[pattern_sz - j];
      else
        gs_skip = pattern_sz - 1 - sl[pattern_sz - j];
      
      i += (gs_skip > bc_skip) ? gs_skip : bc_skip;
    }
    else {
      printf("%zu ", i);
      i += pattern_sz - 1 - sl[1];
    }
  }
  printf("\n%zu \n", nc);

}

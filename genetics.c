#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/** String matching algorithms **/
void naive(char *text, char *pattern, size_t text_sz, size_t pattern_sz);
void kmp(char *text, char *pattern, size_t text_sz, size_t pattern_sz);
void bm(char *text, char *pattern, size_t text_sz, size_t pattern_sz);

/** Helper functions **/
char *read(size_t *str_sz);
void kmpTable(size_t *table, char *pattern, size_t pattern_sz);

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

  for (i = 0; i < text_sz - pattern_sz; i++) {
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
  size_t i, k = 0;
  size_t table[pattern_sz];

  kmpTable(table, pattern, pattern_sz);

  for (i = 0; i < text_sz; i++) {
    /* printf("i: %zu\n", i);
    printf("k: %zu\n", k); */
    while (k > 0 && pattern[k] != text[i])
      k = table[k - 1];
    if (pattern[k] == text[i])
      k++;
    if (k == pattern_sz) {
      /* printf("k: %zu \n", k); */
      k = table[k - 1];
      printf("%zu ", i - pattern_sz + 1);
      /* printf("i: %zu \n", i); */
    }
  }
  /* printf("Pattern size: %zu\n", pattern_sz); */
  printf("\n");
}

/* Do this inside kmp() */
void kmpTable(size_t *table, char *pattern, size_t pattern_sz) {
  size_t i, k = 0;

  table[0] = 0;
  /* printf("%zu ", table[0]); */

  for (i = 1; i < pattern_sz; i++) {
    while (k > 0 && pattern[k] != pattern[i])
      k = table[k - 1];
    if (pattern[k] == pattern[i])
      k++;
    table[i] = k;
    /* printf("%zu ", table[i]); */
  }
  /* printf("\n"); */
}

void bm(char *text, char *pattern, size_t text_sz, size_t pattern_sz) {
  size_t i;
  size_t nc = 0;

  /*** Bad character rule implementation ***/

  /* badc holds the right-most position of each character that appears in the
  pattern; 'A' is on position 0, 'C' on 1, 'G' on 2 and 'T' on 3
  If a character does not appear in the pattern, then its value is the maximum
  size_t possible, i.e. (size_t) -1 */
  /* size_t bc[4] = {-1, -1, -1, -1}; */
  
  /* To do: explore better ways to do this */
  /* for (i = 0; i < pattern_sz; i++) {
    switch (pattern[i]) {
      case ('A'):
        bc[0] = (i > bc[0] || bc[0] == -1) ? i : bc[0];
        break;
      case ('C'):
        bc[1] = (i > bc[1] || bc[1] == -1) ? i : bc[1];
        break;
      case ('G'):
        bc[2] = (i > bc[2] || bc[2] == -1) ? i : bc[2];
        break;
      case ('T'):
        bc[3] = (i > bc[3] || bc[3] == -1) ? i : bc[3];
        break;
    }
  } */

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

  /* printf("bc values: %zu %zu %zu %zu\n", bc[0], bc[1], bc[2], bc[3]); */

  /*** Strong good suffix rule implementation ***/
  /** Z algorithm **/
  size_t z[pattern_sz]; /* Z values */
  size_t left = 0; /* Left box endpoint */
  size_t right = 0; /* Right box endpoint */
  size_t j; /* Auxiliary variable for loops */
  size_t gs[pattern_sz];
  char pattern_r[pattern_sz]; /* Reversed pattern */

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
  
  for (j = 0; j < pattern_sz; j++)
    gs[j] = 0;
  for (j = 0; j < pattern_sz - 1; j++) {
    gs[z[j]] = pattern_sz - j - 2;
  }

  printf("gs before max_gs: ");
  for (j = 0; j < pattern_sz; j++) {
    printf("%zu ", gs[j]);
  }
  printf("\n");

  size_t max_gs = 0;
  for (j = 1; j < pattern_sz - 1; j++) {
    max_gs = (z[j] == j + 1) ? pattern_sz - j - 2 : max_gs;
    gs[j] = (gs[j] == 0) ? max_gs : gs[j];
  }

  /* Missing bad character integration */
  size_t bc_skip;
  for (i = 0; i < text_sz - pattern_sz + 1; i++) {
    j = 0;
    while (j < pattern_sz && text[i + pattern_sz - j - 1] == pattern[pattern_sz - j - 1]) {
      j++;
      nc++;
    }
    if (j != pattern_sz) {
      nc++;
      ch = (size_t) text[i + pattern_sz - j - 1];
      if (bc[ch] == -1)
        bc_skip = pattern_sz - j - 1;
      else {
        if (pattern_sz - j - 1 > bc[ch])
          bc_skip = pattern_sz - j - bc[ch] - 2;
        else
          bc_skip = 0;
      }
      i += (gs[j] > bc_skip) ? gs[j] : bc_skip;
    }
    else {
      printf("%zu ", i);
      i += gs[pattern_sz - 1];
    }
  }
  printf("\n%zu \n", nc);


  printf("z: ");
  for (j = 0; j < pattern_sz; j++) {
    printf("%zu ", z[j]);
  }
  printf("\n");

  /* printf("           ");
  for (j = 0; j < ((pattern_sz < 10) ? pattern_sz : 10); j++) {
    printf("%zu  ", j);
  }
  for (j = 10; j < pattern_sz; j++) {
    printf("%zu ", j);
  }
  printf("\n"); */
/*   printf("String: ");
  for (j = 0; j < pattern_sz; j++) {
    printf("%c", pattern[j]);
  } */
  /* printf("\n"); */
  /* printf("gs: ");
  for (j = 0; j < pattern_sz; j++) {
    printf("%zu ", gs[j]);
  }
  printf("\n"); */

}
# Genetics string matching tool
aalg-genetics is a tool for matching small patterns against large strings composed by the letters A, C, T and G, the four types of bases found in a DNA molecule. To this effect, the naive, Knuth-Morris-Pratt and Boyer-Moore string matching algorithms are implemented.

## Input format
Input files should consist of lines containing one of five commands T (text), N (naive), K (KMP), B (BM) and X (exit). On what follows, <string> and <pattern> should be replaced by proper strings containing only the letters A, C, T or G.
  - T <string> specifies the string which will be subject to a search for a pattern.
  - N <pattern> searches the last string specified by a T command for occurrences of <pattern> using the naive string matching algorithm.
  - K <pattern> searches the last string specified by a T command for occurrences of <pattern> using the Knuth-Morris-Pratt string matching algorithm.
  - B <pattern> searches the last string specified by a T command for occurrences of <pattern> using the Boyer-Moore string matching algorithm.
  - X terminates the program.

## Output format
One line is output whenever a command N is executed, while two are output in the case of the K and B commands.
  - In the case of N <pattern>, a line containing the indices of the first character at which <pattern> was matched on the last specified string.
  - In the case of K <pattern> and B <pattern>, an additional line is output containing the number of character comparisons performed.

## Execution
The tool should be executed with the command
```
/path/to/genetics < input > output
```

## Example
An example of a proper input file is
```
T ACTGGTCCAAAAAAGTCAG
N CA
K TC
B AG
T AAAAAAAAA
K AA
B AA
X
```
Its corresponding output is
```
T ACTGGTCCAAAAAAGTCAG
N CA
K TC
B AG
T AAAAAAAAA
K AA
B AA
X
```

## Dependencies
None, other than a C compiler (Makefile assumes gcc for this effect).

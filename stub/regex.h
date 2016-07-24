typedef struct {
  size_t min, max;
  char mask[4];
} regex_instr;

typedef struct {
  size_t len;
  regex_instr instr[];
} regex;

regex *newregex(const void *, size_t, int);

int doregex(const regex *, 

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  size_t i, argnum = 300000, iterations = 10;
  int status;
  char **args = malloc(sizeof(char *) * (argnum+3)), *printf = argv[1];
  if (argc < 2 || !args) return 1;
  args[0] = "printf";
  args[1] = "%.s";
  args[argnum+2] = NULL;
  for (i = 2; i < argnum+2; i++) args[i] = "x";
  for (i = 0; i < iterations; i++)
    switch (fork()) {
      case -1: return 1;
      case 0: return execve(printf, args, NULL);
      default: wait(&status);
    }
  return 0;
}

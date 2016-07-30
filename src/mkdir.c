#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("pv" /* "m:" */, .argleast = 1);
  mode_t mode = flag('m') ? lastnum('m') : 0777;
  DIR* startdir = opendir(".");
  if (!startdir) return errno;
  int startfd = dirfd(startdir), saverrno;

  while (*++argv) {
    char *outstr;
    size_t outsiz;
    FILE *outfile = open_memstream(&outstr, &outsiz);
    // first remove all the //     FIXME: don't do this for a leading //
    char *ptr, *dir = *argv;
    while ((ptr = strstr(dir, "//")))
      memmove(ptr, ptr+1, strlen(ptr)); // copies the trailing null byte

    if (dir[strlen(dir)-1] == '/') dir[strlen(dir)-1] = 0;

    if (dir[0] == '/') {
      fputc('/', outfile);
      if (chdir("/") == -1) break;
      dir++;
    }

    if (flag('p')) {
      ptr = strtok(dir, "/");
      while (ptr) {
        fprintf(outfile, "%s", ptr);
        fflush(outfile);
        saverrno = errno;
        if (mkdirat(AT_FDCWD, ptr, mode) == -1) {
          if (errno == EEXIST) errno = saverrno;
          else break;
        }
        else if (flag('v')) printf("mkdir: created directory '%s'\n", outstr);
        fputc('/', outfile);
        if (chdir(ptr) == -1) break;
        ptr = strtok(NULL, "/");
      }
    }
    else {
      saverrno = errno;
      if (mkdirat(AT_FDCWD, dir, mode) == -1) {
        if (errno == EEXIST) errno = saverrno;
        else break;
      }
      else if (flag('v')) printf("mkdir: created directory '%s'\n", dir);
    }

    // it would be cool to reuse the buffer with rewind
    // but there's no way to make it work in glibc and musl at the same time
    // musl doesn't append the \0 where needed, so you have to fseek back and add 0 yourself
    // glibc doesn't like writes not at the end of the stream and adds 0 where it's not needed
    // https://gist.github.com/ae6f18ab631ac62f70958fbe6268b900
    // i don't even wanna know who's right, just get over it and free this shit
    fclose(outfile);
    free(outstr);

    fchdir(startfd);
  }
  return errno;
}

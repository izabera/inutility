#include "lib/common.h"

int main(int argc, char *argv[]) {
  options("dqup:", .arglessthan = 2);

  char *template, *dir, *path;
  if (argc == 2) {
    template = argv[1];
    dir = flag('p') ? lastarg('p') : "";
  }
  else {
    template = "tmp.XXXXXX";
    if (!(dir = flag('p') ? lastarg('p') : getenv("TMPDIR"))) dir = "/tmp";
  }
  asprintf(&path, "%s%s%s", dir, dir[0] ? "/" : "", template);

  if (flag('d')) { if (!mkdtemp(path)) return errno; }
  else { if (mkstemp(path) == -1) return errno; }
  puts(path);
  if (flag('u')) (flag('d') ? rmdir : unlink)(path);
  return errno;
}

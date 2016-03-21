#include <unistd.h>
#include <string.h>
/*#include <stdlib.h>*/
#include <errno.h>

#include "lib/common.h"

/* handy for lookups, and make it easier to add/remove options in the future */
enum { optbs, optcbs, optconv, optcount, optibs, optiflag, optobs, optoflag,
  optseek, optskip, optstatus };
enum { convasci, convebcdic, convibm, convblock, convunblock, convlcase,
  convucase, convsparse, convswab, convsync, convexcl, convnocreat,
  convnotrunc, convnoerror, convfdatasync, convfsync };
enum { flagappend, flagdirect, flagdirectory, flagdsync, flagsync,
  flagfullblock, flagnonblock, flagnoatime, flagnocache, flagnoctty,
  flagnofollow, flagcount_bytes, flagskip_bytes, flagseek_bytes };

int parsecomma(const char *string, const char *valid[], size_t validcount) {
  int retval = 0;
  char *token = strtok(string, ",");
  while (token) {
    for (int i = 0; i < validcount; i++) {
      if (valid[i] && !strcmp(token, valid[i])) { /* check for null */
        retval |= 1 << i;
        goto nextwhile;
      }
    }
    exit(-1);
nextwhile:
    token = strtok(NULL, ",");
  }
  return retval;
}

int parseconv(const char *string) {
  const char *convs[] = { [convascii    ] = "ascii",
                          [convebcdic   ] = "ebcdic",
                          [convibm      ] = "ibm",
                          [convblock    ] = "block",
                          [convunblock  ] = "unblock",
                          [convlcase    ] = "lcase",
                          [convucase    ] = "ucase",
                          [convsparse   ] = "sparse",
                          [convswab     ] = "swab",
                          [convsync     ] = "sync",
                          [convexcl     ] = "excl",
                          [convnocreat  ] = "nocreat",
                          [convnotrunc  ] = "notrunc",
                          [convnoerror  ] = "noerror",
                          [convfdatasync] = "fdatasync",
                          [convfsync    ] = "fsync" };
  return parsecomma(string, conv, arrsize(conv));
}

int parseflag(const char *string) {
  const char *flags[] = { [flagappend     ] = "append",
                          [flagdirect     ] = "direct",
                          [flagdirectory  ] = "directory",
                          [flagdsync      ] = "dsync",
                          [flagsync       ] = "sync",
                       /* [flagfullblock  ] = "fullblock", */
                          [flagnonblock   ] = "nonblock",
                          [flagnoatime    ] = "noatime",
                          [flagnocache    ] = "nocache",
                          [flagnoctty     ] = "noctty",
                          [flagnofollow   ] = "nofollow",
                          [flagcount_bytes] = "count_bytes",
                          [flagskip_bytes ] = "skip_bytes",
                          [flagseek_bytes ] = "seek_bytes" };
  return parsecomma(string, flags, arrsize(flags));
}

int parsestat(const char *string) {
  if (!strcmp("none", string)) return 0;
  if (!strcmp("noxfer", string)) return 1;
//if (!strcmp("progress", string)) return 2;  naaaah...
  return -1;
}

int main(int argc, char *argv[]) {
  struct {
    const char *name;
    int (*func)(const char *);
    int value;
  } options[] = { [optbs    ] = {     "bs=", parsenumb, 0 },
                  [optcbs   ] = {    "cbs=", parsenumb, 0 },
                  [optconv  ] = {   "conv=", parseconv, 0 },
                  [optcount ] = {  "count=", parsenumb, 0 },
                  [optibs   ] = {    "ibs=", parsenumb, 0 },
                  [optiflag ] = {  "iflag=", parseflag, 0 },
                  [optobs   ] = {    "obs=", parsenumb, 0 },
                  [optoflag ] = {  "oflag=", parseflag, 0 },
                  [optseek  ] = {   "seek=", parsenumb, 0 },
                  [optskip  ] = {   "skip=", parsenumb, 0 },
                  [optstatus] = { "status=", parsestat, 0 }  };

  int optvalues[arrsize(options)] = { 0 };
  int ifd = 0, iflag = O_RDONLY, ofd = 1, oflag = O_WRONLY|O_CREAT|O_TRUNC;
  char *ifile = NULL, *ofile = NULL;
  if (!strcmp("--help", argv[0])) {
    puts("dd [bs=num] [cbs=num] [conv=conv] [count=num] [ibs=num] [if=file] [iflag=flag] "
         "[obs=num] [of=file] [oflag=flag] [seek=num] [skip=num] [status=stat]");
    return 0;
  }
  if (!strcmp("--", argv[0])) argv++;
  while (*++argv) {
         if (!strncmp("if=", argv[0], 3)) ifile = argv[0]+3; 
    else if (!strncmp("of=", argv[0], 3)) ofile = argv[0]+3;
    else {
      for (int opt = 0; opt < arrsize(options); opt++) {
        if (options[opt] && !strncmp(options[opt], argv[0], strlen(options[opt]))) {
          options[opt].value = options[opt].func(argv[0]+strlen(options[opt]));
          goto nextwhile;
        }
      }
    }
    return 1;
nextwhile: ;
  }

  if (options[optiflag] & 1 << flagdirect   ) iflag |= O_DIRECT;
  if (options[optiflag] & 1 << flagdirectory) iflag |= O_DIRECTORY;
  if (options[optiflag] & 1 << flagdsync    ) iflag |= O_DSYNC;
  if (options[optiflag] & 1 << flagsync     ) iflag |= O_SYNC;
  if (options[optiflag] & 1 << flagnonblock ) iflag |= O_NONBLOCK;
  if (options[optiflag] & 1 << flagnoatime  ) iflag |= O_NOATIME;
  if (options[optiflag] & 1 << flagnoctty   ) iflag |= O_NOCTTY;
  if (options[optiflag] & 1 << flagnofollow ) iflag |= O_NOFOLLOW;

  if (options[optoflag] & 1 << flagappend   ) oflag |= O_APPEND;
  if (options[optoflag] & 1 << flagdirect   ) oflag |= O_DIRECT;
  if (options[optoflag] & 1 << flagdirectory) oflag |= O_DIRECTORY;
  if (options[optoflag] & 1 << flagdsync    ) oflag |= O_DSYNC;
  if (options[optoflag] & 1 << flagsync     ) oflag |= O_SYNC;
  if (options[optoflag] & 1 << flagnonblock ) oflag |= O_NONBLOCK;
  if (options[optoflag] & 1 << flagnoatime  ) oflag |= O_NOATIME;
  if (options[optoflag] & 1 << flagnoctty   ) oflag |= O_NOCTTY;
  if (options[optoflag] & 1 << flagnofollow ) oflag |= O_NOFOLLOW;

  if (options[optconv ] & 1 << convexcl     ) oflag |= O_EXCL;
  if (options[optconv ] & 1 << convnotrunc  ) oflag &= ~O_TRUNC;
  if (options[optconv ] & 1 << convnocreat  ) oflag &= ~O_CREAT;

  if (ifile) if (!(ifd = open(ifile, iflag))) return errno;
  if (ofile) if (!(ofd = open(ofile, oflag))) return errno;

  size_t ibs = 512, obs = 512;
  if (options[optbs   ]) ibs = obs = options[optbs];
  if (options[optibs  ]) ibs = options[optibs];
  if (options[optobs  ]) obs = options[optobs];
  if (options[optobs  ]) obs = options[optobs];
  if (options[optseek ]) {
    if (options[seek_bytes]) lseek(ofd,       options[optseek], SEEK_CUR);
    else                     lseek(ofd, obs * options[optseek], SEEK_CUR);
  }
  if (options[optskip ]) {
    if (options[skip_bytes]) lseek(ifd,       options[optskip], SEEK_CUR);
    else                     lseek(ifd, ibs * options[optskip], SEEK_CUR); /* todo: fallback? */
  }

  struct buffer { off_t begin, end; } writebuf = { 0 }, readbuf = { 0 };
  /* do the thing */
  char *buf = malloc(ibs + obs + 1); /* +1 for stuff like conv=swab */
  ssize_t readsiz, writesiz;
  if (!buf) return errno;

  while (1) {
    errno = 0;
    if (writebuf.end - writebuf.begin >= writesize) {
      if (write(ofd, writebuf.buf + writebuf.begin, obs) == -1) goto out;
    }
    else if (readsiz = read
    switch (loopstate) {
      default: goto out;

      case  canread: readsiz = read(file, buf, ibs);
                          if (readsiz == 0) readstate = eof;
                     else if (readsiz  < 0) readstate = err;
                     else 
                     break;
      case canwrite: if (write(ofd, buf, obs) == -1) goto out; /* todo: eagain */
                     foo;
                     break;
      case  canconv: 
    }
  }

out:
  if (options[optconv ] & 1 << convfdatasync) fdatasync(ofd);
  if (options[optconv ] & 1 << convfsync    ) fsync(ofd);
  if (options[optiflag] & 1 << flagnocache  ) posix_fadvise(ifd, isoffset, ieoffset, POSIX_FADV_DONTNEED);
  if (options[optoflag] & 1 << flagnocache  ) posix_fadvise(ofd, osoffset, oeoffset, POSIX_FADV_DONTNEED);
  
  return errno;
}

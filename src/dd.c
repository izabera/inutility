#include "lib/common.h"

/* handy for lookups, and make it easier to add/remove options in the future */
enum { optbs, optcbs, optconv, optcount, optibs, optiflag, optobs, optoflag,
  optseek, optskip, optstatus };
enum { convascii, convebcdic, convibm, convblock, convunblock, convlcase,
  convucase, convsparse, convswab, convsync, convexcl, convnocreat,
  convnotrunc, convnoerror, convfdatasync, convfsync };
enum { flagappend, flagdirect, flagdirectory, flagdsync, flagsync,
  flagfullblock, flagnonblock, flagnoatime, flagnocache, flagnoctty,
  flagnofollow, flagcount_bytes, flagskip_bytes, flagseek_bytes };

static int64_t parsecomma(const char *string, const char *valid[], size_t validcount) {
  int retval = 0;
  char *str = strdup(string);
  char *token = strtok(str, ",");
  while (token) {
    for (size_t i = 0; i < validcount; i++) {
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

static int64_t parseconv(const char *string) {
  const char *convs[] = { [convascii    ] = "ascii",
                          [convebcdic   ] = "ebcdic",
                          [convibm      ] = "ibm",
                       // [convblock    ] = "block",
                       // [convunblock  ] = "unblock",
                          [convlcase    ] = "lcase",
                          [convucase    ] = "ucase",
                       // [convsparse   ] = "sparse",
                          [convswab     ] = "swab",
                          [convsync     ] = "sync",
                          [convexcl     ] = "excl",
                          [convnocreat  ] = "nocreat",
                          [convnotrunc  ] = "notrunc",
                          [convnoerror  ] = "noerror",
                          [convfdatasync] = "fdatasync",
                          [convfsync    ] = "fsync" };
  return parsecomma(string, convs, arrsize(convs));
}

static int64_t parseflag(const char *string) {
  const char *ddflags[] = { [flagappend     ] = "append",
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
  return parsecomma(string, ddflags, arrsize(ddflags));
}

static int64_t parsestat(const char *string) {
  if (!strcmp("none", string)) return 1;
  if (!strcmp("noxfer", string)) return 2;
//if (!strcmp("progress", string)) return 3;  naaaah...
  exit(-1);
}

static volatile int sig;
static void sighandler(int s) { sig = s; }

static struct timeval begintime, endtime;
static size_t rpart = 0, rfull = 0, wpart = 0, wfull = 0, bytes = 0;

static struct {
  const char *name;
  int64_t (*func)(const char *); // function that parses this option
  int64_t value; // bitmask or number returned from that function
} options[] = { [optbs    ] = {     "bs=", parsebyte, 0 },
             // [optcbs   ] = {    "cbs=", parsebyte, 0 },
                [optconv  ] = {   "conv=", parseconv, 0 },
                [optcount ] = {  "count=", parsebyte, 0 },
                [optibs   ] = {    "ibs=", parsebyte, 0 },
                [optiflag ] = {  "iflag=", parseflag, 0 },
                [optobs   ] = {    "obs=", parsebyte, 0 },
                [optoflag ] = {  "oflag=", parseflag, 0 },
                [optseek  ] = {   "seek=", parsebyte, 0 },
                [optskip  ] = {   "skip=", parsebyte, 0 },
                [optstatus] = { "status=", parsestat, 0 }  };

static void printstat() {
  if (options[optstatus].value == 1) return;
  gettimeofday(&endtime, NULL);
  double seconds = ((endtime.tv_sec * 1000000 + endtime.tv_usec) -
                    (begintime.tv_sec * 1000000 + begintime.tv_usec)) / 1000000.0;

  fprintf(stderr, "%zu+%zu records in\n", rfull, rpart);
  fprintf(stderr, "%zu+%zu records out\n", wfull, wpart);
  if (options[optstatus].value == 2) return;
  fprintf(stderr, "%zu bytes (%.1f%c) copied, %fs, %.1f%c/s\n", bytes, scale(bytes), seconds, scale(bytes/seconds));
}

#include "lib/tables.data"

int main(int argc, char *argv[]) {
  options("", .help = "[bs=num] [conv=conv] [count=num] [ibs=num] [if=file] [iflag=flag]\n"
                      "[obs=num] [of=file] [oflag=flag] [seek=num] [skip=num] [status=stat]");

  int ifd = 0, iflag = O_RDONLY, ofd = 1, oflag = O_WRONLY|O_CREAT|O_TRUNC;
  char *ifile = NULL, *ofile = NULL;

  options[optcount].value = -1;
  while (*++argv) {
         if (!strncmp("if=", argv[0], 3)) ifile = argv[0]+3; 
    else if (!strncmp("of=", argv[0], 3)) ofile = argv[0]+3;
    else {
      for (size_t opt = 0; opt < arrsize(options); opt++) {
        if (options[opt].name && !strncmp(options[opt].name, argv[0], strlen(options[opt].name))) {
          options[opt].value = options[opt].func(argv[0]+strlen(options[opt].name));
          goto nextwhile;
        }
      }
      return -1;
    }
nextwhile: ;
  }

  if (options[optiflag].value & 1 << flagdirect   ) iflag |= O_DIRECT;
  if (options[optiflag].value & 1 << flagdirectory) iflag |= O_DIRECTORY;
  if (options[optiflag].value & 1 << flagdsync    ) iflag |= O_DSYNC;
  if (options[optiflag].value & 1 << flagsync     ) iflag |= O_SYNC;
  if (options[optiflag].value & 1 << flagnonblock ) iflag |= O_NONBLOCK;
  if (options[optiflag].value & 1 << flagnoatime  ) iflag |= O_NOATIME;
  if (options[optiflag].value & 1 << flagnoctty   ) iflag |= O_NOCTTY;
  if (options[optiflag].value & 1 << flagnofollow ) iflag |= O_NOFOLLOW;

  if (options[optoflag].value & 1 << flagappend   ) oflag |= O_APPEND;
  if (options[optoflag].value & 1 << flagdirect   ) oflag |= O_DIRECT;
  if (options[optoflag].value & 1 << flagdirectory) oflag |= O_DIRECTORY;
  if (options[optoflag].value & 1 << flagdsync    ) oflag |= O_DSYNC;
  if (options[optoflag].value & 1 << flagsync     ) oflag |= O_SYNC;
  if (options[optoflag].value & 1 << flagnonblock ) oflag |= O_NONBLOCK;
  if (options[optoflag].value & 1 << flagnoatime  ) oflag |= O_NOATIME;
  if (options[optoflag].value & 1 << flagnoctty   ) oflag |= O_NOCTTY;
  if (options[optoflag].value & 1 << flagnofollow ) oflag |= O_NOFOLLOW;

  if (options[optconv ].value & 1 << convexcl     ) oflag |= O_EXCL;
  if (options[optconv ].value & 1 << convnotrunc  ) oflag &= ~O_TRUNC;
  if (options[optconv ].value & 1 << convnocreat  ) oflag &= ~O_CREAT;

  size_t ibs = 512, obs = 512; //, cbs = 512;
  if (options[optbs   ].value)       ibs = obs = options[optbs ].value;
//if (options[optcbs  ].value) cbs =             options[optcbs].value;
  if (options[optibs  ].value)       ibs =       options[optibs].value;
  if (options[optobs  ].value)             obs = options[optobs].value;

  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sighandler;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);
  atexit(printstat);

  gettimeofday(&begintime, NULL);

  if (ifile) { if ((ifd = open(ifile, iflag, 0666)) == -1) return errno; }
  else fcntl(0, F_SETFL, fcntl(0, F_GETFL) | iflag);
  if (ofile) { if ((ofd = open(ofile, oflag, 0666)) == -1) return errno; }
  else fcntl(1, F_SETFL, fcntl(1, F_GETFL) | oflag);

  struct {
    size_t len;
    char buf[];
  } *rbuf = malloc(sizeof(rbuf)+ibs),
 // *cbuf = malloc(sizeof(cbuf)+cbs),
    *wbuf = malloc(sizeof(wbuf)+obs);

  struct { // 4096 byte buffer for swab
    union {
      uint64_t u64buf[512];
      uint16_t u16buf[2048];
      char buf[4096];
    };
    size_t len;
  } swab, *sbuf = &swab; // this is to get a similar interface with the other buffers

  if (!rbuf || /* !cbuf || */ !wbuf) return errno;
  rbuf->len /* = cbuf->len */ = wbuf->len = sbuf->len = 0;

  char tmpbuf[4096];
  if (options[optskip ].value) {
    if (options[optiflag].value & 1 << flagskip_bytes)
      lseek(ifd,       options[optskip].value, SEEK_CUR);
    else
      lseek(ifd, ibs * options[optskip].value, SEEK_CUR);
    if (errno == EINVAL && options[optskip].value < 0)
      lseek(ifd, 0, SEEK_SET); // this is really useful and it's a shame that it doesn't work in gnu dd
    else if (errno == ESPIPE) {
      size_t total = options[optskip].value * (options[optiflag].value & 1 << flagskip_bytes ? 1 : ibs);
      ssize_t res;
      while (total > 4096) {
        if ((res = read(ifd, tmpbuf, 4096)) <= 0) goto skipped;
        total -= res;
      }
      while (total) {
        if ((res = read(ifd, tmpbuf, total)) <= 0) goto skipped;
        total -= res;
      }
    }
  }
skipped: errno = 0;

  if (!options[optcount].value && options[optseek].value) { // this is totally non obvious
    UNUSED(ftruncate(ofd, options[optseek].value *
          ((options[optoflag].value & 1 << flagseek_bytes ? 1 : obs))));
    return errno;
  }

  if (options[optseek ].value) {
    if (options[optoflag].value & 1 << flagseek_bytes)
      lseek(ofd,       options[optseek].value, SEEK_CUR);
    else
      lseek(ofd, obs * options[optseek].value, SEEK_CUR);
  }
  errno = 0;

  ssize_t ret;
  size_t canread = 1, count = 0, swapped = 0;
  /* do the thing */
  while (1) {
    if (sig == SIGUSR1) {
      sigaction(SIGINT, &sa, NULL);
      sig = 0;
      printstat();
    }
    else if (sig == SIGINT) {
      sa.sa_handler = SIG_DFL;
      sigaction(SIGINT, &sa, NULL);
      printstat();
      raise(SIGINT);
      return (SIGINT+128) | errno; // not reached
    }

    if (wbuf->len == obs) {
      if ((ret = write(ofd, wbuf->buf, obs)) == -1) break;
      if ((size_t) ret == obs) wfull++;
      else {
        memmove(wbuf->buf, wbuf->buf+ret, obs-ret);
        wpart++;
      }
      wbuf->len -= ret;
      bytes += ret;
    }
    else if (sbuf->len == 4096) {
      if (options[optconv].value & 1 << convswab) {
        for (size_t q = 0; q < 512; q++)
          sbuf->u64buf[q] = ((0x00ff00ff00ff00ffULL & sbuf->u64buf[q]) << 8) |
            ((0xff00ff00ff00ff00ULL & sbuf->u64buf[q]) >> 8);
      }
      swapped = 1;

      size_t len = min(obs-wbuf->len, 4096);
      memmove(wbuf->buf+wbuf->len, sbuf->buf, len);
      wbuf->len += len;
      memmove(sbuf->buf, sbuf->buf+len, sbuf->len-len);
      sbuf->len -= len;
    }
    else if (swapped && sbuf->len) {
      size_t len = min(obs-wbuf->len, 4096);
      memmove(wbuf->buf+wbuf->len, sbuf->buf, len);
      wbuf->len += len;
      memmove(sbuf->buf, sbuf->buf+len, sbuf->len-len);
      sbuf->len -= len;
    }
    else if (rbuf->len) {
      size_t len = min(4096-sbuf->len, rbuf->len);
      if (options[optconv].value & 1 << convascii)
        for (size_t q = 0; q < rbuf->len; q++)
          rbuf->buf[q] = ebcdicascii[(size_t) (unsigned char) rbuf->buf[q]];
      if (options[optconv].value & 1 << convucase) {
        for (size_t q = 0; q < rbuf->len; q++)
          if (rbuf->buf[q] >= 97 && rbuf->buf[q] <= 122) rbuf->buf[q] &= ~32;
      }
      else if (options[optconv].value & 1 << convlcase) {
        for (size_t q = 0; q < rbuf->len; q++)
          if (rbuf->buf[q] >= 65 && rbuf->buf[q] <=  90) rbuf->buf[q] |=  32;
      }
      if (options[optconv].value & 1 << convascii) { } // avoid doing both
      else if (options[optconv].value & 1 << convebcdic)
        for (size_t q = 0; q < rbuf->len; q++)
          rbuf->buf[q] = asciiebcdic[(size_t) (unsigned char) rbuf->buf[q]];
      else if (options[optconv].value & 1 << convibm)
        for (size_t q = 0; q < rbuf->len; q++)
          rbuf->buf[q] = asciiibm[(size_t) (unsigned char) rbuf->buf[q]];

      memmove(sbuf->buf+sbuf->len, rbuf->buf, len);
      sbuf->len += len;
      memmove(rbuf->buf, rbuf->buf+len, rbuf->len-len);
      rbuf->len -= len;
      swapped = 0;
    }
    else if (canread && count < (size_t) options[optcount].value) {
      ret = read(ifd, rbuf->buf, ibs);
      if (ret == 0 || (ret == -1 && errno != EINTR &&
            !(options[optconv].value & 1 << convnoerror))) canread = 0;
      if (ret != -1) rbuf->len = ret;
      if (ret > 0) {
        if ((size_t) ret != ibs && options[optconv].value & 1 << convsync) {
          memset(rbuf->buf+ret, 0, ibs-ret); // posix recommends to do this *before* reading??
          rbuf->len = ibs;
        }
      }
      if (options[optiflag].value & 1 << flagcount_bytes) {
        count += ret;
        if (count > (size_t) options[optcount].value) rbuf->len -= count - options[optcount].value;
      }
      else count++;
      if ((size_t) ret == ibs) rfull++;
      else if (ret > 0) rpart++;
    }
    else break;
  }

  if (sbuf->len) {
    if (options[optconv].value & 1 << convswab) {
      for (size_t q = 0; q < sbuf->len/2; q++) // rounded down, don't swab the last byte
        sbuf->u16buf[q] = ((0x00ff & sbuf->u16buf[q]) << 8) |
                          ((0xff00 & sbuf->u16buf[q]) >> 8);
    }
  }

  while (wbuf->len || sbuf->len) { // leftovers
    if (sbuf->len) {
      size_t len = min(obs-wbuf->len, sbuf->len);
      memmove(wbuf->buf+wbuf->len, sbuf->buf, len);
      wbuf->len += len;
      memmove(sbuf->buf, sbuf->buf+len, sbuf->len-len);
      sbuf->len -= len;
    }
    if (wbuf->len) {
      size_t len = min(obs, wbuf->len);
      if ((ret = write(ofd, wbuf->buf, len)) == -1) break;
      if ((size_t) ret == obs) wfull++;
      else wpart++;
      if ((size_t) ret != wbuf->len) memmove(wbuf->buf, wbuf->buf+ret, wbuf->len-ret);
      wbuf->len -= ret;
      bytes += ret;
    }
  }

  if (options[optconv ].value & 1 << convfdatasync) fdatasync(ofd);
  if (options[optconv ].value & 1 << convfsync    ) fsync(ofd);
  if (options[optiflag].value & 1 << flagnocache  ) posix_fadvise(ifd, 0, 0, POSIX_FADV_DONTNEED);
  if (options[optoflag].value & 1 << flagnocache  ) posix_fadvise(ofd, 0, 0, POSIX_FADV_DONTNEED);
  
  return errno;
}

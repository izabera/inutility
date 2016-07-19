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

int64_t parsecomma(const char *string, const char *valid[], size_t validcount) {
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

int64_t parseconv(const char *string) {
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
  return parsecomma(string, convs, arrsize(convs));
}

int64_t parseflag(const char *string) {
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

int64_t parsestat(const char *string) {
  if (!strcmp("none", string)) return 1;
  if (!strcmp("noxfer", string)) return 2;
//if (!strcmp("progress", string)) return 3;  naaaah...
  exit(-1);
}

volatile int sig;
void sighandler(int s) { sig = s; }

struct timeval begintime, endtime;
size_t rpart = 0, rfull = 0, wpart = 0, wfull = 0, bytes = 0;

struct {
  const char *name;
  int64_t (*func)(const char *); // function that parses this option
  int64_t value; // bitmask or number returned from that function
} options[] = { [optbs    ] = {     "bs=", parsebyte, 0 },
                [optcbs   ] = {    "cbs=", parsebyte, 0 },
                [optconv  ] = {   "conv=", parseconv, 0 },
                [optcount ] = {  "count=", parsebyte, 0 },
                [optibs   ] = {    "ibs=", parsebyte, 0 },
                [optiflag ] = {  "iflag=", parseflag, 0 },
                [optobs   ] = {    "obs=", parsebyte, 0 },
                [optoflag ] = {  "oflag=", parseflag, 0 },
                [optseek  ] = {   "seek=", parsebyte, 0 },
                [optskip  ] = {   "skip=", parsebyte, 0 },
                [optstatus] = { "status=", parsestat, 0 }  };

void printstat() {
  if (options[optstatus].value == 1) return;
  gettimeofday(&endtime, NULL);
  double seconds = ((endtime.tv_sec * 1000000 + endtime.tv_usec) -
                    (begintime.tv_sec * 1000000 + begintime.tv_usec)) / 1000000.0;

  fprintf(stderr, "%zu+%zu records in\n", rfull, rpart);
  fprintf(stderr, "%zu+%zu records out\n", wfull, wpart);
  if (options[optstatus].value == 2) return;
  fprintf(stderr, "%zu bytes (%.1f%c) copied, %fs, %.1f%c/s\n", bytes, scale(bytes), seconds, scale(bytes/seconds));
}

static const char ebcdicascii[] = {  // grabbed these tables from gnu dd
  '\000', '\001', '\002', '\003', '\234', '\011', '\206', '\177',
  '\227', '\215', '\216', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\235', '\205', '\010', '\207',
  '\030', '\031', '\222', '\217', '\034', '\035', '\036', '\037',
  '\200', '\201', '\202', '\203', '\204', '\012', '\027', '\033',
  '\210', '\211', '\212', '\213', '\214', '\005', '\006', '\007',
  '\220', '\221', '\026', '\223', '\224', '\225', '\226', '\004',
  '\230', '\231', '\232', '\233', '\024', '\025', '\236', '\032',
  '\040', '\240', '\241', '\242', '\243', '\244', '\245', '\246',
  '\247', '\250', '\325', '\056', '\074', '\050', '\053', '\174',
  '\046', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\041', '\044', '\052', '\051', '\073', '\176',
  '\055', '\057', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\313', '\054', '\045', '\137', '\076', '\077',
  '\272', '\273', '\274', '\275', '\276', '\277', '\300', '\301',
  '\302', '\140', '\072', '\043', '\100', '\047', '\075', '\042',
  '\303', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\304', '\305', '\306', '\307', '\310', '\311',
  '\312', '\152', '\153', '\154', '\155', '\156', '\157', '\160',
  '\161', '\162', '\136', '\314', '\315', '\316', '\317', '\320',
  '\321', '\345', '\163', '\164', '\165', '\166', '\167', '\170',
  '\171', '\172', '\322', '\323', '\324', '\133', '\326', '\327',
  '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
  '\340', '\341', '\342', '\343', '\344', '\135', '\346', '\347',
  '\173', '\101', '\102', '\103', '\104', '\105', '\106', '\107',
  '\110', '\111', '\350', '\351', '\352', '\353', '\354', '\355',
  '\175', '\112', '\113', '\114', '\115', '\116', '\117', '\120',
  '\121', '\122', '\356', '\357', '\360', '\361', '\362', '\363',
  '\134', '\237', '\123', '\124', '\125', '\126', '\127', '\130',
  '\131', '\132', '\364', '\365', '\366', '\367', '\370', '\371',
  '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
  '\070', '\071', '\372', '\373', '\374', '\375', '\376', '\377'
};

static const char asciiebcdic[] = {
  '\000', '\001', '\002', '\003', '\067', '\055', '\056', '\057',
  '\026', '\005', '\045', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\074', '\075', '\062', '\046',
  '\030', '\031', '\077', '\047', '\034', '\035', '\036', '\037',
  '\100', '\132', '\177', '\173', '\133', '\154', '\120', '\175',
  '\115', '\135', '\134', '\116', '\153', '\140', '\113', '\141',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\172', '\136', '\114', '\176', '\156', '\157',
  '\174', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
  '\310', '\311', '\321', '\322', '\323', '\324', '\325', '\326',
  '\327', '\330', '\331', '\342', '\343', '\344', '\345', '\346',
  '\347', '\350', '\351', '\255', '\340', '\275', '\232', '\155',
  '\171', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\221', '\222', '\223', '\224', '\225', '\226',
  '\227', '\230', '\231', '\242', '\243', '\244', '\245', '\246',
  '\247', '\250', '\251', '\300', '\117', '\320', '\137', '\007',
  '\040', '\041', '\042', '\043', '\044', '\025', '\006', '\027',
  '\050', '\051', '\052', '\053', '\054', '\011', '\012', '\033',
  '\060', '\061', '\032', '\063', '\064', '\065', '\066', '\010',
  '\070', '\071', '\072', '\073', '\004', '\024', '\076', '\341',
  '\101', '\102', '\103', '\104', '\105', '\106', '\107', '\110',
  '\111', '\121', '\122', '\123', '\124', '\125', '\126', '\127',
  '\130', '\131', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\160', '\161', '\162', '\163', '\164', '\165',
  '\166', '\167', '\170', '\200', '\212', '\213', '\214', '\215',
  '\216', '\217', '\220', '\152', '\233', '\234', '\235', '\236',
  '\237', '\240', '\252', '\253', '\254', '\112', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\241', '\276', '\277',
  '\312', '\313', '\314', '\315', '\316', '\317', '\332', '\333',
  '\334', '\335', '\336', '\337', '\352', '\353', '\354', '\355',
  '\356', '\357', '\372', '\373', '\374', '\375', '\376', '\377'
};

static const char asciiibm[] = {
  '\000', '\001', '\002', '\003', '\067', '\055', '\056', '\057',
  '\026', '\005', '\045', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\074', '\075', '\062', '\046',
  '\030', '\031', '\077', '\047', '\034', '\035', '\036', '\037',
  '\100', '\132', '\177', '\173', '\133', '\154', '\120', '\175',
  '\115', '\135', '\134', '\116', '\153', '\140', '\113', '\141',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\172', '\136', '\114', '\176', '\156', '\157',
  '\174', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
  '\310', '\311', '\321', '\322', '\323', '\324', '\325', '\326',
  '\327', '\330', '\331', '\342', '\343', '\344', '\345', '\346',
  '\347', '\350', '\351', '\255', '\340', '\275', '\137', '\155',
  '\171', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\221', '\222', '\223', '\224', '\225', '\226',
  '\227', '\230', '\231', '\242', '\243', '\244', '\245', '\246',
  '\247', '\250', '\251', '\300', '\117', '\320', '\241', '\007',
  '\040', '\041', '\042', '\043', '\044', '\025', '\006', '\027',
  '\050', '\051', '\052', '\053', '\054', '\011', '\012', '\033',
  '\060', '\061', '\032', '\063', '\064', '\065', '\066', '\010',
  '\070', '\071', '\072', '\073', '\004', '\024', '\076', '\341',
  '\101', '\102', '\103', '\104', '\105', '\106', '\107', '\110',
  '\111', '\121', '\122', '\123', '\124', '\125', '\126', '\127',
  '\130', '\131', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\160', '\161', '\162', '\163', '\164', '\165',
  '\166', '\167', '\170', '\200', '\212', '\213', '\214', '\215',
  '\216', '\217', '\220', '\232', '\233', '\234', '\235', '\236',
  '\237', '\240', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
  '\312', '\313', '\314', '\315', '\316', '\317', '\332', '\333',
  '\334', '\335', '\336', '\337', '\352', '\353', '\354', '\355',
  '\356', '\357', '\372', '\373', '\374', '\375', '\376', '\377'
};

int main(int argc, char *argv[]) {
  options("", .help = "[bs=num] [cbs=num] [conv=conv] [count=num] [ibs=num] [if=file] [iflag=flag]\n"
                      "[obs=num] [of=file] [oflag=flag] [seek=num] [skip=num] [status=stat]");

  int ifd = 0, iflag = O_RDONLY, ofd = 1, oflag = O_WRONLY|O_CREAT|O_TRUNC;
  char *ifile = NULL, *ofile = NULL;
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

  size_t ibs = 512, obs = 512, cbs = 512;
  if (options[optbs   ].value) cbs = ibs = obs = options[optbs ].value;
  if (options[optcbs  ].value) cbs =             options[optcbs].value;
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
    *cbuf = malloc(sizeof(cbuf)+cbs),
    *wbuf = malloc(sizeof(wbuf)+obs);

  struct { // 4096 byte buffer for swab
    union {
      uint64_t u64buf[512];
      uint16_t u16buf[2048];
      char buf[4096];
    };
    size_t len;
  } swab, *sbuf = &swab; // this is to get a similar interface with the other buffers

  if (!rbuf || !cbuf || !wbuf) return errno;
  rbuf->len = cbuf->len = wbuf->len = sbuf->len = 0;

  if (options[optseek ].value) {
    if (options[optoflag].value & 1 << flagskip_bytes)
      lseek(ofd,       options[optseek].value, SEEK_CUR);
    else
      lseek(ofd, obs * options[optseek].value, SEEK_CUR);
  }

  errno = 0;
  char tmpbuf[4096];
  if (options[optskip ].value) {
    if (options[optiflag].value & 1 << flagskip_bytes)
      lseek(ifd,       options[optskip].value, SEEK_CUR);
    else
      lseek(ifd, ibs * options[optskip].value, SEEK_CUR);
    if (errno == ESPIPE) {
      size_t total = options[optskip].value * (options[optiflag].value & 1 << flagskip_bytes ? ibs : 1);
      ssize_t res;
      while (total > 4096) {
        res = read(ifd, tmpbuf, 4096);
        total -= res;
      }
      while (total) {
        res = read(ifd, tmpbuf, total);
        total -= res;
      }
    }
  }
  errno = 0;

  if (!options[optcount].value) options[optcount].value = -1;
  ssize_t ret;
  size_t canread = 1, count = 0, swapped = 0;
  /* do the thing */
  while (1) {
    if (sig == SIGUSR1) {
      sigaction(SIGINT, &sa, NULL);
      sig = 0;
      printstat();
    }
    else if (sig == SIGINT) return (SIGINT+128) | errno;

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
      if ((ret = read(ifd, rbuf->buf, ibs)) <= 0 && errno != EINTR) canread = 0;
      if (ret != -1) rbuf->len = ret;
      if (options[optiflag].value & 1 << flagcount_bytes) count += ret;
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

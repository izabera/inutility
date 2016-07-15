#include "lib/common.h"
static int ascii(char *buf, size_t siz) {
  int cr = 0;
  for (size_t i = 0; i < siz; i++) {
    if ((buf[i] >= 32 && buf[i] <= 126) || buf[i] == 9 || buf[i] == 11 || buf[i] == 12) continue;
    if (buf[i] == '\n') { cr |= 1; continue; }
    if (buf[i] == '\r') {
      if (i+1 < siz) {
        if (buf[i+1] == '\n') { cr |= 2; i++; }
        else cr |= 4;
      }
    }
    else return 0;
  }
  return cr;
}

// most values are from toybox, elf values are from the common file implementation
int main(int argc, char **argv) {
  options("hL");

  int fd = 0;
  char pathbuf[PATH_MAX];
  unsigned char buf[512];
  struct stat st;
  if (argc == 1) {
    argv[0] = "-";
    fstat(fd, &st);
    goto inner;
  }
  while (*++argv) {
    if (argv[0][0] == '-' && argv[0][1] == 0) fstat(fd = 0, &st);
    else {
      if (flag('h') || !flag('L')) {
        if (lstat(argv[0], &st) == -1) { printf("%s: cannot stat\n", argv[0]); continue; }
        if (S_ISLNK(st.st_mode)) {
          UNUSED(readlink(argv[0], pathbuf, sizeof(pathbuf)));
          printf("%s: symbolic link to %s\n", argv[0], pathbuf);
          continue;
        }
      }
      if ((fd = open(argv[0], O_RDONLY)) == -1) { printf("%s: cannot open\n", argv[0]); continue; }
      fstat(fd, &st);
    }
inner:
#define printtype(str) { printf("%s: %s\n", argv[0], str); if (fd) close(fd); continue; }
    if (S_ISFIFO(st.st_mode)) printtype("fifo");
    if (S_ISSOCK(st.st_mode)) printtype("socket");
    if (S_ISBLK(st.st_mode))  printtype("block special");
    if (S_ISCHR(st.st_mode))  printtype("character special");
    if (S_ISDIR(st.st_mode))  printtype("directory");
    if (st.st_size == 0)      printtype("empty");

    ssize_t siz = read(fd, buf, 512);
    lseek(fd, 0, SEEK_SET);

    if (buf[0] == '#' && buf[1] == '!') {
      char *tmp = (char *) &buf[2];
      while (tmp[0] == ' ' || tmp[0] == '\t') tmp++; 
      if (strncmp("/usr/bin/env", tmp, strlen("/usr/bin/env")) == 0) tmp += strlen("/usr/bin/env");
      while (tmp[0] == ' ' || tmp[0] == '\t') tmp++;
      if (strchr(tmp, '\n')) *(strchr(tmp, '\n')) = 0;
      if (strpbrk(tmp, " \t")) *(strpbrk(tmp, " \t")) = 0;
      printf("%s: %s script\n", argv[0], tmp);
      close(fd);
      continue;
    }

    if (siz >= 32 && buf[0] == 0xff && buf[1] == 0xd8)     printtype("JPEG image data");
    if (siz >= 28 && !memcmp("\x89PNG\r\n\x1A\n", buf, 8)) printtype("PNG image data");
    if (siz >= 28 && !memcmp("GIF87a", buf, 8))            printtype("GIF image data, version 87a");
    if (siz >= 28 && !memcmp("GIF89a", buf, 8))            printtype("GIF image data, version 89a");
    if (siz >=  8 && !memcmp("\xCA\xFE\xBA\xBE", buf, 8))  printtype("Java class file");
    if (siz >=  5 && !memcmp("PK\3\4", buf, 4))            printtype("Zip archive data");
    if (siz >= 40 && !memcmp("\x7f" "ELF", buf, 4)) {
      char *bit     = buf[4] == 1 ? "32" : buf[4] == 2 ? "64" : "??",
            endian  = buf[5] == 1 ? 'L'  : buf[5] == 2 ? 'M'  : '?' ,
            version = buf[6] == 1 ? '1'  :                      '?' ,
           *abi     = buf[7] ==   0 ? "SYSV"                           :
                      buf[7] ==   1 ? "HP-UX"                          :
                      buf[7] ==   2 ? "NetBSD"                         :
                      buf[7] ==   3 ? "GNU/Linux"                      :
                      buf[7] ==   4 ? "GNU/Hurd"                       :
                      buf[7] ==   5 ? "86Open"                         :
                      buf[7] ==   6 ? "Solaris"                        :
                      buf[7] ==   7 ? "Monterey"                       :
                      buf[7] ==   8 ? "IRIX"                           :
                      buf[7] ==   9 ? "FreeBSD"                        :
                      buf[7] ==  10 ? "Tru64"                          :
                      buf[7] ==  11 ? "Novell Modesto"                 :
                      buf[7] ==  12 ? "OpenBSD"                        :
                      buf[7] ==  13 ? "OpenVMS"                        :
                      buf[7] ==  14 ? "HP NonStop Kernel"              :
                      buf[7] ==  15 ? "AROS Research Operating System" :
                      buf[7] ==  16 ? "FenixOS"                        :
                      buf[7] ==  17 ? "Nuxi CloudABI"                  :
                      buf[7] ==  97 ? "ARM"                            :
                      buf[7] == 255 ? "embedded"                       : "???",
           *type    = buf[16] == 1 ? "relocatable"    :
                      buf[16] == 2 ? "executable"     :
                      buf[16] == 3 ? "shared object"  :
                      buf[16] == 4 ? "core dump"      : "bad type";
      printf("%s: ELF %s-bit %cSB %s, version %c (%s)\n", argv[0], bit, endian, type, version, abi);
      // TODO check dyn/stat linked
      close(fd);
      continue;
    }
    switch (ascii((char *)buf, siz)) {
      case 0: printtype("data");                                           break;
      case 1: printtype("ASCII text");                                     break;
      case 2: printtype("ASCII text, with CRLF line terminators");         break;
      case 3: printtype("ASCII text, with LF, CRLF line terminators");     break;
      case 4: printtype("ASCII text, with CR line terminators");           break;
      case 5: printtype("ASCII text, with CR, LF line terminators");       break;
      case 6: printtype("ASCII text, with CRLF, CR line terminators");     break;
      case 7: printtype("ASCII text, with CRLF, CR, LF line terminators"); break;
    }
  }
  return errno;
}

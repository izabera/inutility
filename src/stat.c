#include "lib/common.h"

#define Case    break; case
#define Default break; default
void fsstatprint(char fmt, struct statfs *fs) {
  switch (fmt) {
    Case 'a': printf("%lu", fs->f_bavail);
    Case 'b': printf("%lu", fs->f_blocks);
    Case 'c': printf("%lu", fs->f_files);
    Case 'd': printf("%lu", fs->f_ffree);
    Case 'f': printf("%lu", fs->f_bfree);
    Case 'i': printf("%" PRIx64,
        (uint64_t) fs->f_fsid.__val[0] << 32 | (fs->f_fsid.__val[1] & 0xffffffff));
    Case 'l': printf("%ld", fs->f_namelen);
    Case 's': printf("%ld", fs->f_frsize);
    Case 'S': printf("%ld", fs->f_bsize);
    Case 't': printf("%lx", fs->f_type);
    Case 'T': ;
      struct {
        uint32_t type;
        char *name;
      } names[] = { // stolen from busybox + toybox
        { 0xadff    , "affs"                },  { 0x6969    , "nfs"      },
        { 0x1badface, "bfs"                 },  { 0x3434    , "nilfs"    },
        { 0x9123683e, "btrfs"               },  { 0x564c    , "novell"   },
        { 0x012ff7b7, "coh"                 },  { 0x5346544e, "ntfs"     },
        { 0x28cd3d45, "cramfs"              },  { 0x9fa0    , "proc"     },
        { 0x1cd1    , "devpts"              },  { 0x52654973, "reiserfs" },
        { 0x137d    , "ext"                 },  { 0x7275    , "romfs"    },
        { 0xef51    , "ext2"                },  { 0x858458f6, "romfs"    },
        { 0xef53    , "ext2/ext3"           },  { 0x43415d53, "smackfs"  },
        { 0x4006    , "fat"                 },  { 0x517b    , "smb"      },
        { 0xf995e849, "hpfs"                },  { 0x534f434b, "sockfs"   },
        { 0x9660    , "isofs"               },  { 0x73717368, "squashfs" },
        { 0x4000    , "isofs"               },  { 0x62656572, "sysfs"    },
        { 0x4004    , "isofs"               },  { 0x012ff7b5, "sysv4"    },
        { 0x3153464a, "jfs"                 },  { 0x012ff7b6, "sysv2"    },
        { 0x137f    , "minix"               },  { 0x01021994, "tmpfs"    },
        { 0x138f    , "minix (30 char.)"    },  { 0x00011954, "ufs"      },
        { 0x2468    , "minix v2"            },  { 0x012ff7b4, "xenix"    },
        { 0x2478    , "minix v2 (30 char.)" },  { 0x58465342, "xfs"      },
        { 0x4d44    , "msdos"               },  { 0x012fd16d, "xia"      },
      };
      char *name = "unknown";
      for (size_t i = 0; i < arrsize(names); i++)
        if (names[i].type == fs->f_type) {
          name = names[i].name;
          break;
        }
      printf("%s", name);
    Default: putchar_unlocked('?'); return;
  }
}

void statprint(char fmt, struct stat *st) {
  char buf[512];
  struct group *g;
  struct passwd *pw;
  switch (fmt) {
    Case 'a': printf("%04o", st->st_mode & ~S_IFMT);
    Case 'A': printf("%c" "%c%c%c" "%c%c%c" "%c%c%c",
        S_ISDIR(st->st_mode)  ? 'd' : S_ISCHR(st->st_mode)  ? 'c' :
        S_ISBLK(st->st_mode)  ? 'b' : S_ISLNK(st->st_mode)  ? 'l' :
        S_ISFIFO(st->st_mode) ? 'p' : S_ISSOCK(st->st_mode) ? 's' : '-',
        S_IRUSR & st->st_mode ? 'r' : '-',
        S_IWUSR & st->st_mode ? 'w' : '-',
        "-xSs"[(!!(S_IXUSR & st->st_mode))|((!!(S_ISUID & st->st_mode))<<1)],
        S_IRGRP & st->st_mode ? 'r' : '-',
        S_IWGRP & st->st_mode ? 'w' : '-',
        "-xSs"[(!!(S_IXGRP & st->st_mode))|((!!(S_ISGID & st->st_mode))<<1)],
        S_IROTH & st->st_mode ? 'r' : '-',
        S_IWOTH & st->st_mode ? 'w' : '-',
        "-xTt"[(!!(S_IXOTH & st->st_mode))|((!!(S_ISVTX & st->st_mode))<<1)]
        );
    Case 'b': printf("%lu", st->st_blocks);
    Case 'B': printf("512");
    Case 'd': printf("%ld", st->st_dev);
    Case 'D': printf("%lx", st->st_dev);
    Case 'f': printf("%x", st->st_mode);
    Case 'F': ;
      struct {
        uint32_t type;
        char *name;
      } names[] = {
        { S_IFSOCK, "socket"       }, { S_IFLNK , "symbolic link"    },
        { S_IFREG , "regular file" }, { S_IFBLK , "block device"     },
        { S_IFDIR , "directory"    }, { S_IFCHR , "character device" },
        { S_IFIFO , "fifo"         },
      };
      char *name = "unknown";
      for (size_t i = 0; i < arrsize(names); i++)
        if (names[i].type == (st->st_mode & S_IFMT)) {
          name = names[i].name;
          break;
        }
      printf("%s", name);
    Case 'g': printf("%u", st->st_gid);
    Case 'G': if (!(g = getgrgid(st->st_gid))) return;
              printf("%s", g->gr_name);
    Case 'h': printf("%lu", st->st_nlink);
    Case 'i': printf("%lu", st->st_ino);
 // Case 'm': todo mountpoint
    Case 'o': printf("%lu", st->st_blksize);
    Case 's': printf("%lu", st->st_size);
    Case 't': printf("%lx", (st->st_rdev & 0x0fff00) >> 8);
    Case 'T': printf("%lx", ((st->st_rdev & 0xfff0000) >> 12) | (st->st_rdev & 0xff));
    Case 'u': printf("%u", st->st_uid);
    Case 'U': if (!(pw = getpwuid(st->st_uid))) return;
              printf("%s", pw->pw_name);
#define dodate(x) strftime(buf, sizeof(buf), "%F %T.XXXXXXXXX %z", localtime(&st->st_##x.tv_sec)); \
                  snprintf(strchr(buf, '.')+1, 10, "%09ld", st->st_##x.tv_nsec);                   \
                  buf[strlen(buf)] = ' ';                                                          \
                  printf("%s", buf);
    Case 'x': dodate(atim);
    Case 'X': printf("%lu", st->st_atime);
    Case 'y': dodate(mtim);
    Case 'Y': printf("%lu", st->st_mtime);
    Case 'z': dodate(ctim);
    Case 'Z': printf("%lu", st->st_ctime);
    Default: putchar_unlocked('?'); return;
  }
}

int main(int argc, char *argv[]) {
  options("Lftc:", .argleast = 1);
  struct stat st;
  struct statfs fs;
  char *format = flag('c') ? lastarg('c')                                                :
    flag('t') && flag('f') ? "%n %i %l %t %s %S %b %f %a %c %d"                          :
    flag('t')              ? "%n %s %b %f %u %g %D %i %h %t %T %X %Y %Z %o"              :
                 flag('f') ? "  File: \"%n\"\n    ID: %i Namelen: %l    Type: %T\n"
                             "Block Size: %s    Fundamental block size: %S\n"
                             "Blocks: Total: %b\tFree: %f\tAvailable: %a\n"
                             "Inodes: Total: %c\tFree: %d"                               :
                             "  File: %N\n  Size: %s\t Blocks: %b\t IO Blocks: %B\t%F\n"
                             "Device: %Dh/%dd\t Inode: %i\t Links: %h\n"
                             "Access: (%a/%A)\tUid: (%u/%U)\tGid: (%g/%G)\n"
                             "Access: %x\nModify: %y\nChange: %z";

  while (*++argv) {
    if (flag('f')) { if (statfs(*argv, &fs) == -1) continue; }
    else {
      if (!strcmp(*argv, "-")) { if (fstat(    0, &st) == -1) continue; }
      else if (flag('L'))      { if ( stat(*argv, &st) == -1) continue; }
      else                       if (lstat(*argv, &st) == -1) continue;
    }

    for (char *fmt = format; *fmt; fmt++) { // stole the idea from toybox
      if (*fmt == '%') {
        fmt++;
             if (*fmt ==  0 ) { putchar_unlocked('%'); break; }
        else if (*fmt == '%') putchar_unlocked('%');
        else if (*fmt == 'n') printf("%s", *argv); 
        else if (*fmt == 'N' && !flag('f')) { // toybox does this with a global var
          if (S_ISLNK(st.st_mode)) {
            char linktarget[PATH_MAX] = { 0 };
            readlink(*argv, linktarget, sizeof(linktarget));
            printf("`%s' -> `%s'", *argv, linktarget); 
          }
          else printf("`%s'", *argv); 
        }
        else if (flag('f')) fsstatprint(*fmt, &fs);
        else                  statprint(*fmt, &st);
      }
      else putchar_unlocked(*fmt);
    }
    putchar_unlocked('\n');
  }
  return errno;
}

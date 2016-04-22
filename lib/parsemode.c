#include "common.h"

// ok so these are the posix states
typedef enum { symbolic_mode, clause, wholist, who, actionlist, action,
  permcopy, op, permlist, perm,
// and a few ones for internal usage
  ugoa, octal, comma, end, error } state;


#define casesign '+': case '-': case '='
#define caseugoa 'u': case 'g': case 'o': case 'a'
#define caseperm 'r': case 'w': case 'x': /* case 'X': */ case 's': case 't'

// let there be switches *EVERYWHERE*
static state getstate(const char *str) {
  switch (*str) {
    case casesign: return op;
    case caseperm: return perm;
    case caseugoa: return ugoa;
    case caseoct:  return octal;
    case ',':      return comma;
    case 0:        return end;
  }
  return error;
}

int64_t parsemode(const char *modestr, mode_t mode) {
  int64_t modeint = mode;

  enum { equal, plus, minus };
  enum { a, u, g, o };
  enum { r, w, x, /* X, */ s, t };
  enum { whichnone, whichoctal, whichcopy, whichperm };
  union {
    struct { uint16_t sign:2, modenum:12; };
    struct { uint16_t signplaceholder:2, to:3, from:3, perm:6; };
    struct { uint16_t filler:14, which:2; };
    uint16_t all;
  } modearray[strlen(modestr)]; // bigger than it needs to be
  size_t arrpos = 0;

  char *tmp;
#define egoto { printf("line: %d str: %s\n", __LINE__, modestr); goto badreturn; }
  if (*modestr == ',' || !*modestr) egoto;
  // finite state machine to convert human input to machine parsable data
  while (*modestr) {
    modearray[arrpos].all = 0;
    switch (getstate(modestr)) {
      default: // not reached but gcc and clang issue a warning otherwise
      case comma:
        if (*++modestr == ',') egoto;
        arrpos++;
        break;
      case error: egoto;
      case ugoa:
        do {
          switch (*modestr) {
            case 'u': modearray[arrpos].to |= 1; break;
            case 'g': modearray[arrpos].to |= 2; break;
            case 'o': modearray[arrpos].to |= 4; break;
            case 'a': modearray[arrpos].to |= 7; break;
            default: goto outugoa;
          }
          modestr++;
        } while (1);
      outugoa:
        if (getstate(modestr) != op || getstate(modestr+1) == octal) egoto;
        break;
      case octal:
        modearray[arrpos].which = whichoctal;
        modearray[arrpos].sign = equal;
        modearray[arrpos].modenum = 4095 & strtol(modestr, &tmp, 8); // 4095 == lowest 12 bits
        modestr = tmp;
        break;
      case op:
        gotoop:
        switch (*modestr++) {
          case '+': modearray[arrpos].sign = plus ; break;
          case '-': modearray[arrpos].sign = minus; break;
          case '=': modearray[arrpos].sign = equal; break;
        }
        switch (getstate(modestr)) {
          case end: case comma: break;
          case octal:
            modearray[arrpos].which = whichoctal;
            modearray[arrpos].modenum = 4095 & strtol(modestr, &tmp, 8);
            modestr = tmp;
            break;
          case ugoa:
            modearray[arrpos].which = whichcopy;
            switch (*modestr++) {
              case 'a': egoto;
              case 'u': modearray[arrpos].from = u; break;
              case 'g': modearray[arrpos].from = g; break;
              case 'o': modearray[arrpos].from = o; break;
            }
            break;
          case perm:
            modearray[arrpos].which = whichperm;
            do {
              switch (*modestr) {
                case 'r': modearray[arrpos].perm |= 1 << r; break;
                case 'w': modearray[arrpos].perm |= 1 << w; break;
                case 'x': modearray[arrpos].perm |= 1 << x; break;
//              case 'X': modearray[arrpos].perm |= 1 << X; break;
                case 's': modearray[arrpos].perm |= 1 << s; break;
                case 't': modearray[arrpos].perm |= 1 << t; break;
                default: goto outperm;
              }
              modestr++;
            } while (1);
            outperm: break;
          default: egoto;
        }
        switch (getstate(modestr)) {
          case op:
            modearray[arrpos+1].all = 0;
            modearray[arrpos+1].to = modearray[arrpos].to;
            arrpos++;
            goto gotoop;
          case end: break;
          default: egoto;
        }
        break;
    }
  }

  // the actual loop is now much simpler
  for (size_t i = 0; i < arrpos; i++) {
    uint16_t mask = 0;
    if (modearray[i].to & 1) mask |= S_IRWXU;
    if (modearray[i].to & 2) mask |= S_IRWXG;
    if (modearray[i].to & 4) mask |= S_IRWXO;
    switch (modearray[i].which) {
      default: // not reached
      case whichperm:
        if (modearray[i].perm & 1 << r) modeint |= mask & (S_IRUSR|S_IRGRP|S_IROTH); break;
        if (modearray[i].perm & 1 << w) modeint |= mask & (S_IWUSR|S_IWGRP|S_IWOTH); break;
        if (modearray[i].perm & 1 << x) modeint |= mask & (S_IXUSR|S_IXGRP|S_IXOTH); break;
//      if (modearray[i].perm & 1 << X) modeint |= mask & (S_IXUSR|S_IXGRP|S_IXOTH); break;
        if (modearray[i].perm & 1 << s) modeint |= mask & (S_ISUID|S_ISGID        ); break;
        if (modearray[i].perm & 1 << t) modeint |=        (S_ISVTX                ); break;
        break;
      case whichcopy:
        switch (modearray[i].from) {
          case u: modearray[i].modenum = mode & S_IRWXU; break;
          case g: modearray[i].modenum = mode & S_IRWXG; break;
          case o: modearray[i].modenum = mode & S_IRWXO; break;
        }
        // fallthrough
      case whichoctal:
        switch (modearray[i].sign) {
          case equal: modeint  =  modearray[i].modenum; break;
          case minus: modeint &= ~modearray[i].modenum; break;
          case plus : modeint |=  modearray[i].modenum; break;
        }
        break;
    }
  }
  printf("<%" PRId64 ">\n", modeint);
  return modeint;
  badreturn:
  printf("error %" PRId64 "\n", modeint);
  return INT64_MIN;
}

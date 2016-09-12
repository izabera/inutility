srcdir = src
objdir = obj
libdir = lib
programs = $(basename $(notdir $(wildcard $(srcdir)/*.c)))
libheaders = $(wildcard $(libdir)/*.h)
lib = $(wildcard $(libdir)/*.[ch])
objlib = $(addsuffix .o,$(basename $(libheaders)))
src = $(addprefix $(srcdir)/,$(addsuffix .c,$(programs)))
obj = $(addprefix $(objdir)/,$(addsuffix .o,$(programs))) $(objlib)

CFLAGS = -Wall -Wextra -Wshadow -pedantic -O2 -g -march=native
LDFLAGS = -flto
override CFLAGS += -std=c11 -DVERSION='"$(shell git describe --always --dirty || echo xxxxxxx)"' -D_GNU_SOURCE

all: $(objdir) inutility

inutility: $(obj) $(libheaders) $(srcdir)/proto.h $(srcdir)/struct.h inutility.c
	$(CC) $(CFLAGS) $(LDFLAGS) $@.c $(obj) -o $@

.PHONY: clean install uninstall small fast all

$(srcdir)/proto.h: $(src)
	cd src && {							\
		for f in *.c; do					\
		echo "int main_$${f%.c}(int, char **);";		\
		done > proto.h;						\
	}

$(srcdir)/struct.h: $(src)
	cd src && {							\
		for f in *.c; do					\
			echo "{ \"$${f%.c}\", main_$${f%.c} },";	\
		done > struct.h;					\
	}

$(objdir)/%.o: $(srcdir)/%.c $(objlib)
	$(CC) $(CFLAGS) -Dmain=main_$* -c $(srcdir)/$*.c -o $@

$(objdir):
	mkdir -p $(objdir)

$(libdir)/%.o: $(libdir)/%.c
	$(CC) $(CFLAGS) -c $(libdir)/$*.c -o $@

clean:
	rm -f $(obj) $(srcdir)/proto.h $(srcdir)/struct.h

install: inutility
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp inutility $(DESTDIR)$(PREFIX)/bin
	for target in $(programs) list; do				\
		ln -fs inutility "$(DESTDIR)$(PREFIX)/bin/$$target";	\
	done

uninstall:
	for target in inutility $(programs) list; do			\
		rm -f "$(DESTDIR)$(PREFIX)/bin/$$target";		\
	done

fast:
	make CFLAGS='-Ofast -march=native' LDFLAGS=-flto

small:
	make CC=musl-gcc CFLAGS='-DSMALL -Os -fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections -march=native' LDFLAGS='-flto -s -Wl,-gc-sections -static'
	strip --strip-all --remove-section=.comment --remove-section=.note inutility
	if type sstrip >/dev/null 2>&1; then sstrip inutility; fi

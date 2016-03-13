srcdir = src
objdir = obj
libdir = lib
programs = $(basename $(notdir $(wildcard $(srcdir)/*.c)))
libheaders = $(wildcard $(libdir)/*.h)
lib = $(wildcard $(libdir)/*.[ch])
objlib = $(addsuffix .o,$(basename $(libheaders)))
src = $(addprefix $(srcdir)/,$(addsuffix .c,$(programs)))
obj = $(addprefix $(objdir)/,$(addsuffix .o,$(programs))) $(objlib)

CFLAGS = -Wall -O2 -g -march=native -D_GNU_SOURCE
LDFLAGS = -flto

all: $(objdir) inutility

inutility: $(obj) $(libheaders) $(srcdir)/proto.h $(srcdir)/struct.h inutility.c
	$(CC) $(CFLAGS) $(LDFLAGS) $@.c $(obj) -o $@

.PHONY: clean install uninstall

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
	strip $(DESTDIR)$(PREFIX)/bin/inutility
	for target in $(programs) list; do				\
		ln -fs inutility "$(DESTDIR)$(PREFIX)/bin/$$target";	\
	done

uninstall:
	for target in inutility $(programs) list; do			\
		rm -f "$(DESTDIR)$(PREFIX)/bin/$$target";		\
	done

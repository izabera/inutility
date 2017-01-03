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

ifeq ($(notdir $(CC)),gcc)
	override CFLAGS += -Wsuggest-attribute=const -Wsuggest-attribute=pure -Wsuggest-attribute=format -Wsuggest-attribute=noreturn
endif

all: $(objdir) inutility

inutility: $(obj) $(libheaders) $(srcdir)/proto.h $(srcdir)/struct.h inutility.c
	$(CC) $(CFLAGS) $(LDFLAGS) $@.c $(obj) -o $@

.PHONY: clean install uninstall small smalldyn fast all

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
		done;							\
		echo "{ \"[\", main_test },";				\
		echo "{ \"egrep\", main_grep },";				\
		echo "{ \"fgrep\", main_grep },";				\
	} > struct.h

$(objdir)/errno.o: $(srcdir)/errno.c $(objlib)
	cd src &&							\
	echo '#include <errno.h>' |					\
	$(CC) -dD -E - |						\
	awk '/^#define E/ { printf "{ \"%s\", %s },\n", $$2, $$2 }' |	\
	sort > errnolist.h
	$(CC) $(CFLAGS) -Dmain=main_errno -c $(srcdir)/errno.c -o $@

$(objdir)/%.o: $(srcdir)/%.c $(objlib)
	$(CC) $(CFLAGS) -Dmain=main_$* -c $(srcdir)/$*.c -o $@

$(objdir):
	mkdir -p $(objdir)

$(libdir)/%.o: $(libdir)/%.c
	$(CC) $(CFLAGS) -c $(libdir)/$*.c -o $@

clean:
	rm -f $(obj) $(srcdir)/*.h

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
	make CFLAGS='-DNDEBUG -Ofast -march=native' LDFLAGS=-flto

smalldyn:
	make CC=musl-gcc CFLAGS='-DNDEBUG -Os -fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections -march=native' LDFLAGS='-flto -s -Wl,-gc-sections'
	strip --strip-all --remove-section=.comment --remove-section=.note inutility
	if type sstrip >/dev/null 2>&1; then sstrip inutility; fi

small:
	make CC=musl-gcc CFLAGS='-DNDEBUG -Os -fno-asynchronous-unwind-tables -fdata-sections -ffunction-sections -march=native' LDFLAGS='-flto -s -Wl,-gc-sections -static'
	strip --strip-all --remove-section=.comment --remove-section=.note inutility
	if type sstrip >/dev/null 2>&1; then sstrip inutility; fi

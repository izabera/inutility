srcdir = src
objdir = obj
libdir = lib
programs = $(basename $(notdir $(wildcard $(srcdir)/*.c)))
lib = $(wildcard $(libdir)/*.h)
objlib = $(basename $(lib)).o
src = $(addprefix $(srcdir)/,$(addsuffix .c,$(programs)))
obj = $(addprefix $(objdir)/,$(addsuffix .o,$(programs))) $(objlib)

CFLAGS = -Wall -Wextra -O2 -g -march=native
LDFLAGS = -flto

all: inutility

inutility: $(obj) $(srcdir)/proto.h $(srcdir)/struct.h inutility.c
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

$(objdir)/%.o: $(srcdir)/%.c $(lib)
	$(CC) $(CFLAGS) -Dmain=main_$* -c $(srcdir)/$*.c -o $@

clean:
	rm -f $(obj) $(srcdir)/proto.h $(srcdir)/struct.h

install: inutility
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp inutility "$(DESTDIR)$(PREFIX)/bin"
	for target in $(programs) list; do				\
		ln -fs "inutility" "$(DESTDIR)$(PREFIX)/bin/$$target";	\
	done

uninstall:
	for target in inutility $(programs) list; do			\
		rm -f "$(DESTDIR)$(PREFIX)/bin/$$target";		\
	done

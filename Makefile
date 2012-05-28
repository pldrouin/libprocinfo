ifeq ($(shell uname -s),FreeBSD)
	LIBFLAGS+= -lkvm
endif

ifeq ($(CXX),icpc)
STCOM   =       xiar cru
else
STCOM   =       ar rcs
endif

CSRCS		=	$(wildcard *.c)
OBJS		=	$(CSRCS:.c=.o)
OBJSS		=	$(CSRCS:.c=.os)
CLHD		=	$(CSRCS:.c=.h)
ODEP		=	$(OBJS:.o=.d)

all: libprocinfo.so libprocinfo.as libprocinfo.a

libprocinfo.so: $(OBJS)
	$(CC) $(CFLAGS) -shared $(LIBFLAGS) -o $@ $^

libprocinfo.a: $(OBJS)
	rm -rf $@ && $(STCOM) $@ $^

libprocinfo.as: $(OBJSS)
	rm -rf $@ && $(STCOM) $@ $^

clean:
	rm -f *.d
	rm -f *.o *.os

clear: clean
	rm -f *.so *.a

$(ODEP): %.d: %.c %.h
	@echo "Generating dependency file $@"
	@set -e; rm -f $@
	@$(CC) -M $(CFLAGS) $< > $@.tmp
	@sed 's,\($*\)\.o[ :]*,\1.o \1.os $@ : ,g' < $@.tmp > $@
	@rm -f $@.tmp

include $(ODEP)

$(OBJS): %.o: %.c
	 $(CC) -c -o $@ -fPIC $(CFLAGS) $<

$(OBJSS): %.os: %.c
	 $(CC) -c -o $@ $(CFLAGS) $<

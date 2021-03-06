.SUFFIX       = .e: .i: .l: .p: .r: .s: .y:   

.SUFFIXES: .pc .c .pfo .f .F

DBGFLAG       = -g3

CFLAGS        = $(DBGFLAG) -w -I. -I../h -L../lib -I/usr/include

CPPFLAGS      = $(LCFLAGS)

TARGET_ARCH   =

FFLAGS        = -c

HOSTLIBS      = -fPIC

LDFLAGS       = $(HOSTLIBS)

#CC            = /usr/5bin/cc
CC            = gcc -fPIC
#
FC            = f77

PCC           = pcc

PCCINC        = $(ORACLE_HOME)/c/lib

PCCFLAGS      = include=$(PCCINC) ireclen=132 oreclen=132 rebind=yes \
                select_error=no ltype=none

PFC           = pcc

PFCINC        = $(ORACLE_HOME)/fortran/lib

PFCFLAGS      = include=$(PFCINC) ireclen=132 oreclen=132 rebind=yes \
                select_error=no ltype=none
GENDEST       = .

DEST          = .

INSTALLGROUP  = tpi

INSTALLOWNER  = julian

EXTHDRS       = ../h/geoscience.h \
		../h/gs.h \
		../h/gs_err.h \
		../h/gs_ptypes.h

HDRS          = gs_ptypes.h

LIBRARY	      = libgs.a
.PRECIOUS:    libgs.a

MAKEFILE      = makefile

OBJS          = gs_alloc.o \
		gs_cf_name.o \
		gs_chk_debug.o \
		gs_cm.o \
		gs_cm_do_exec.o \
		gs_config.o \
		gs_connect.o \
		gs_faccess.o \
		gs_finder_rec.o \
		gs_get_handle.o \
		gs_home.o \
		gs_init.o \
		gs_inq_ctxt.o \
		gs_inq_dest.o \
		gs_ipc_key.o \
		gs_log.o \
		gs_log_init.o \
		gs_logfile.o \
		gs_receive.o \
		gs_semlib.o \
		gs_send.o \
		gs_sendstr.o \
		gs_slot.o \
		gs_subs.o \
		gs_talk.o \
		gs_terminate.o \
		gs_test.o \
		in.fld.o

OBJLIB        = .

OBJECTS       = $(OBJLIB)/*.o

PRINT         = lpr

SRCS          = gs_alloc.c \
		gs_cf_name.c \
		gs_chk_debug.c \
		gs_cm.c \
		gs_cm_do_exec.c \
		gs_config.c \
		gs_connect.c \
		gs_faccess.c \
		gs_finder_rec.c \
		gs_get_handle.c \
		gs_home.c \
		gs_init.c \
		gs_inq_ctxt.c \
		gs_inq_dest.c \
		gs_ipc_key.c \
		gs_log.c \
		gs_log_init.c \
		gs_logfile.c \
		gs_receive.c \
		gs_semlib.c \
		gs_send.c \
		gs_sendstr.c \
		gs_slot.c \
		gs_subs.c \
		gs_talk.c \
		gs_terminate.c \
		gs_test.c \
		in.fld.c

GSLIBS        = -lgs

all:	library gs_cm gs_test gs_finder_rec gs_alloc

library:	$(LIBRARY)($(OBJS)) ranlib
		@rm -f $(OBJS)

world:	clean rmlib all

new:;	-touch $(SRCS) gs_cm.c gs_test.c gs_config.c gs_finder_rec.c

rmlib:;		rm -f $(LIBRARY)

ranlib:	$(LIBRARY)
		ranlib $(LIBRARY)
	
prod:;	make DBGFLAG=-O

__.SYMDEF:;  ranlib $(LIBRARY) 

gslock: ${LIBC}
	${CC} -o $@ ${CFLAGS} $@.c ${GSLIBS}

gs_cm:  $(SRCS) $(HDRS) gs_cm.c
	${CC} -o $@ ${CFLAGS} -DSERVER $@.c ${GSLIBS}

gs_test: $(SRCS) $(HDRS) gs_test.c
	${CC} -o $@ ${CFLAGS} -DSERVER $@.c ${GSLIBS}

gs_finder_rec: $(SRCS) $(HDRS) gs_finder_rec.c
	${CC} -o $@ ${CFLAGS} $@.c ${GSLIBS}

gs_alloc: gs_alloc.c
	${CC} -o $@ ${CFLAGS} -DTEST $@.c ${GSLIBS}

gs_config: gs_config.c
	${CC} -o $@ ${CFLAGS} -DTEST $@.c ${GSLIBS}

clean:; rm -f core $(OBJS)

cleandir: clean
	rm -f ${MAN} tags .depend

depend: ${SRCS}
	mkdep -p ${CFLAGS} ${SRCS}

id:     midb

ptypes:   proto
   
proto:;   mkptypes $(SRCS) | sed "s/va_alist/va_alist,.../" | grep -v ' main ' > gs_ptypes.h

midb:;  -@midb >/dev/null 2>&1 && echo "ID database created."

install:
	install -s -o $(INSTALLOWNER) -g $(INSTALLGROUP) -m 4755 gslock ${DESTDIR}/bin/gslock
	install -s -o $(INSTALLOWNER) -g $(INSTALLGROUP) -m 4755 gs_cm ${DESTDIR}/bin/gs_cm
	install -s -o $(INSTALLOWNER) -g $(INSTALLGROUP) -m 4755 test ${DESTDIR}/bin/cm

lint: ${SRCS}
	lint ${CFLAGS} ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}

#depend:;	mkmf -f $(MAKEFILE)

extract:;	ar xo $(DEST)/$(LIBRARY)
		@rm -f __.SYMDEF

index:;		ctags -wx $(HDRS) $(SRCS)

link:;          $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) $(LDFLAGS) \
		$(LCFLAGS) $(GENDEST)/$*.c -o $(DESTDIR)/$* $(OBJLIB)/$*.o

#install:	$(LIBRARY)
#		install $(INSTALLGROUP) $(INSTALLOWNER) $(LIBRARY) $(DEST)
#		ranlib -t $(DEST)/$(LIBRARY)

print:;		$(PRINT) $(HDRS) $(SRCS)

#tags:           $(HDRS) $(SRCS); ctags $(HDRS) $(SRCS)

#update:         $(DEST)/$(LIBRARY)

.pc.c:;		$(PCC) $(PCCFLAGS) iname=$< oname=$(GENDEST)/$*.c

.pc.a:;		$(PCC) $(PCCFLAGS) iname=$< oname=$(GENDEST)/$*.c
		$(CC)  $(CFLAGS)  $(CPPFLAGS) $(TARGET_ARCH) \
			$(LCFLAGS) -c $(GENDEST)/$*.c -o $(OBJLIB)/$*.o

.pc:;		$(PCC) $(PCCFLAGS) iname=$< oname=$(GENDEST)/$*.c
		$(CC)  $(CFLAGS)  $(CPPFLAGS) $(TARGET_ARCH) \
			$(LCFLAGS) -c $(GENDEST)/$*.c -o $(OBJLIB)/$*.o

.pfo.a:;	$(PFC) $(PFCFLAGS) iname=$< oname=$(GENDEST)/$*.f
		$(FC)  $(FFLAGS)  $(CPPFLAGS) $(TARGET_ARCH) \
			$(LCFLAGS) -c $(GENDEST)/$*.f -o $(OBJLIB)/$*.o

.c.a:;		$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c $< -o $(OBJLIB)/$*.o
		ar crul $(LIBRARY) $(OBJLIB)/$*.o

.f.a:;		$(FC)  $(FFLAGS)  $(CPPFLAGS) $(TARGET_ARCH) \
			$(LCFLAGS) -c $< -o $(OBJLIB)/$*.o

.F.a:;		$(FC)  $(CFLAGS) $(FFLAGS)  $(CPPFLAGS) $(TARGET_ARCH) \
			$(LCFLAGS) -c $< -o $(OBJLIB)/$*.o
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

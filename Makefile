RM = rm -f

CC        = gcc
COPTIONS  = -O2 -fomit-frame-pointer -Wall
CINCLUDES = -I. -I./include -I./libparse 
CLIBS     = -lhidapi-hidraw -lcurl -ljson-c

PROG =  helpdesk

COBJS = lcdout.o \
	helpdesk.o \
	dodisplay.o \
	restio.o \

all: go-for-it

ifeq (.depend,$(wildcard .depend))
include .depend
go-for-it: $(PROG)
else
go-for-it: depend
endif


$(PROG): $(COBJS)
	$(RM) $@
	$(CC) -o $@ $(COBJS) $(COPTIONS) $(CLIBS)

%.o: %.c
	$(RM) $@
	$(CC) -c $*.c $(COPTIONS) $(CINCLUDES)

depend:
	touch .depend
	makedepend $(CINCLUDES) $(COBJS:.o=.c) $(COBJS2:.o=.c) $(COBJS3:.o=.c) $(COBJS4:.o=.c) -f .depend
	$(RM) .depend.bak

clean:
	$(RM) $(PROG) $(COBJS) $(COBJS2) $(COBJS3) $(COBJS4) $(COBJS5)
	$(RM) *~

rclean: clean
	$(RM) $(PROG) $(PROG2) $(PROG3) $(PROG4) $(PROG5)
	$(RM) .depend .depend.back


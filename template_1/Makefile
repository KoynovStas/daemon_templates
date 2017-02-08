DAEMON_NAME           = daemon_tmpl
DAEMON_MAJOR_VERSION  = 0
DAEMON_MINOR_VERSION  = 0
DAEMON_PATCH_VERSION  = 0
DAEMON_LOG_FILE_NAME  = $(DAEMON_NAME).log
DAEMON_NO_CHDIR       = 1
DAEMON_NO_CLOSE_STDIO = 0



CFLAGS   = -DDAEMON_NAME='"$(DAEMON_NAME)"'
CFLAGS  += -DDAEMON_MAJOR_VERSION=$(DAEMON_MAJOR_VERSION)
CFLAGS  += -DDAEMON_MINOR_VERSION=$(DAEMON_MINOR_VERSION)
CFLAGS  += -DDAEMON_PATCH_VERSION=$(DAEMON_PATCH_VERSION)
CFLAGS  += -DDAEMON_LOG_FILE_NAME='"$(DAEMON_LOG_FILE_NAME)"'
CFLAGS  += -DDAEMON_NO_CHDIR=$(DAEMON_NO_CHDIR)
CFLAGS  += -DDAEMON_NO_CLOSE_STDIO=$(DAEMON_NO_CLOSE_STDIO)
CFLAGS  += -O2  -s  -Wall
GCC      =  gcc





.PHONY: all
all: clean  $(DAEMON_NAME)



.PHONY: clean
clean:
	-@rm -f *.o
	-@rm -f *.*~
	-@rm -f $(DAEMON_NAME)



$(DAEMON_NAME):
	$(GCC)  $@.c -o $@  $(CFLAGS)
	@echo "  ---- Compiled $@ ver $(DAEMON_MAJOR_VERSION).$(DAEMON_MINOR_VERSION).$(DAEMON_PATCH_VERSION) ----"
	@echo ""



.PHONY: debug
debug: CFLAGS  += -DDEBUG
debug: DAEMON_NO_CLOSE_STDIO = 1
debug: all



.PHONY: help
help:
	@echo "make [command]"
	@echo "command is:"
	@echo "   clean   -  remove all binary files"
	@echo "   all     -  clean and build daemon"
	@echo "   debug   -  build in debug mode (#define DEBUG 1)"
	@echo "   help    -  This help"


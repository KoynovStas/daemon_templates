DAEMON_NAME          =  daemon2
DAEMON_MAJOR_VERSION = 0
DAEMON_MINOR_VERSION = 0
DAEMON_PATCH_VERSION = 0
DAEMON_PID_FILE_NAME = $(DAEMON_NAME).pid
DAEMON_LOG_FILE_NAME = $(DAEMON_NAME).log
DAEMON_NO_CHDIR      = 0

COMMON_DIR           = ../common

CFLAGS               = -DDAEMON_NAME='"$(DAEMON_NAME)"'
CFLAGS              += -DDAEMON_MAJOR_VERSION=$(DAEMON_MAJOR_VERSION)
CFLAGS              += -DDAEMON_MINOR_VERSION=$(DAEMON_MINOR_VERSION)
CFLAGS              += -DDAEMON_PATCH_VERSION=$(DAEMON_PATCH_VERSION)
CFLAGS              += -DDAEMON_PID_FILE_NAME='"$(DAEMON_PID_FILE_NAME)"'
CFLAGS              += -DDAEMON_LOG_FILE_NAME='"$(DAEMON_LOG_FILE_NAME)"'
CFLAGS              += -DDAEMON_NO_CHDIR=$(DAEMON_NO_CHDIR)

CFLAGS              += -I$(COMMON_DIR)
CFLAGS              += -O2  -s  -Wall

GCC                  =  gcc





SOURCES  = $(DAEMON_NAME).c         \
           $(COMMON_DIR)/daemon.c





.PHONY: all
all: clean  $(DAEMON_NAME)



.PHONY: clean
clean:
	-@rm  *.o
	-@rm  *.*~
	-@rm  $(DAEMON_NAME)



$(DAEMON_NAME):
	$(GCC)  $(SOURCES) -o $@  $(CFLAGS)
	@echo "  ---- Compiled $@ ----"
	@echo ""



.PHONY: debug
debug: CFLAGS  += -DDEBUG
debug: all



.PHONY: help
help:
	@echo "make [command]"
	@echo "command is:"
	@echo "   clean   -  remove all binary files"
	@echo "   all     -  clean and build daemon"
	@echo "   debug   -  build in debug mode (#define DEBUG 1)"
	@echo "   help    -  This help"


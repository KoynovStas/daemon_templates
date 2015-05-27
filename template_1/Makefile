DAEMON_NAME  =  daemon1
CFLAGS       = -DDAEMON_NAME='"$(DAEMON_NAME)"'
CFLAGS      += -O2  -s  -Wall
GCC          =  gcc





.PHONY: all
all: clean  $(DAEMON_NAME)



.PHONY: clean
clean:
	-@rm  *.o
	-@rm  *.*~
	-@rm  $(DAEMON_NAME)



$(DAEMON_NAME):
	$(GCC)  $@.c -o $@  $(CFLAGS)
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


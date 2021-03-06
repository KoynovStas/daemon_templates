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

CFLAGS  += -O2  -Wall  -pipe

CC      ?=  gcc




# Add your source files to the list.
# Supported *.c  *.cpp  *.S files.
# For other file types write a template rule for build, see below.
SOURCES  = $(DAEMON_NAME).c





OBJECTS  := $(patsubst %.c,  %.o, $(SOURCES) )
OBJECTS  := $(patsubst %.cpp,%.o, $(OBJECTS) )
OBJECTS  := $(patsubst %.S,  %.o, $(OBJECTS) )


DEBUG_SUFFIX   = debug

DEBUG_OBJECTS := $(patsubst %.o, %_$(DEBUG_SUFFIX).o, $(OBJECTS) )




.PHONY: all
all: debug release



.PHONY: release
release: CFLAGS := -s  $(CFLAGS)
release: $(DAEMON_NAME)



.PHONY: debug
debug: DAEMON_NO_CLOSE_STDIO = 1
debug: CFLAGS := -DDEBUG  -g  $(CFLAGS)
debug: $(DAEMON_NAME)_$(DEBUG_SUFFIX)



# release
$(DAEMON_NAME): .depend $(OBJECTS)
	$(call build_bin, $(OBJECTS))


# debug
$(DAEMON_NAME)_$(DEBUG_SUFFIX): .depend $(DEBUG_OBJECTS)
	$(call build_bin, $(DEBUG_OBJECTS))



# Build release objects
%.o: %.c
	$(build_object)


%.o: %.cpp
	$(build_object)


%.o: %.S
	$(build_object)



# Build debug objects
%_$(DEBUG_SUFFIX).o: %.c
	$(build_object)


%_$(DEBUG_SUFFIX).o: %.cpp
	$(build_object)


%_$(DEBUG_SUFFIX).o: %.S
	$(build_object)



.PHONY: clean
clean:
	-@rm -f $(DAEMON_NAME)
	-@rm -f $(DAEMON_NAME)_$(DEBUG_SUFFIX)
	-@rm -f $(OBJECTS)
	-@rm -f $(DEBUG_OBJECTS)
	-@rm -f .depend
	-@rm -f *.*~




.depend:
	-@rm -f .depend
	@echo "Generating dependencies..."
	@for src in $(SOURCES) ; do  \
        echo "  [depend]  $$src" ; \
        $(CC) $(CFLAGS) -MT ".depend $${src%.*}.o $${src%.*}_$(DEBUG_SUFFIX).o" -MM $$src >> .depend ; \
    done


ifeq "$(findstring $(MAKECMDGOALS),clean distclean)"  ""
    include $(wildcard .depend)
endif




# Common commands
BUILD_ECHO = echo "\n  [build]  $@:"


define build_object
    @$(BUILD_ECHO)
    $(CC) -c $< -o $@  $(CFLAGS)
endef


define build_bin
    @$(BUILD_ECHO)
    $(CC)  $1 -o $@  $(CFLAGS)
    @echo "\n---- Compiled $@ ver $(DAEMON_MAJOR_VERSION).$(DAEMON_MINOR_VERSION).$(DAEMON_PATCH_VERSION) ----\n"
endef



.PHONY: help
help:
	@echo "make [command]"
	@echo "command is:"
	@echo "   all     -  build daemon in release and debug mode"
	@echo "   debug   -  build in debug mode (#define DEBUG 1)"
	@echo "   release -  build in release mode (strip)"
	@echo "   clean   -  remove all generated files"
	@echo "   help    -  this help"

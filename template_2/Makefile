DAEMON_NAME           = daemon_tmpl
DAEMON_MAJOR_VERSION  = 0
DAEMON_MINOR_VERSION  = 0
DAEMON_PATCH_VERSION  = 0
DAEMON_PID_FILE_NAME  = $(DAEMON_NAME).pid
DAEMON_LOG_FILE_NAME  = $(DAEMON_NAME).log
DAEMON_NO_CHDIR       = 1
DAEMON_NO_CLOSE_STDIO = 0



COMMON_DIR = ../common

CFLAGS     = -DDAEMON_NAME='"$(DAEMON_NAME)"'
CFLAGS    += -DDAEMON_MAJOR_VERSION=$(DAEMON_MAJOR_VERSION)
CFLAGS    += -DDAEMON_MINOR_VERSION=$(DAEMON_MINOR_VERSION)
CFLAGS    += -DDAEMON_PATCH_VERSION=$(DAEMON_PATCH_VERSION)
CFLAGS    += -DDAEMON_PID_FILE_NAME='"$(DAEMON_PID_FILE_NAME)"'
CFLAGS    += -DDAEMON_LOG_FILE_NAME='"$(DAEMON_LOG_FILE_NAME)"'
CFLAGS    += -DDAEMON_NO_CHDIR=$(DAEMON_NO_CHDIR)
CFLAGS    += -DDAEMON_NO_CLOSE_STDIO=$(DAEMON_NO_CLOSE_STDIO)

CFLAGS    += -I$(COMMON_DIR)
CFLAGS    += -O2  -Wall  -pipe

GCC        =  gcc




# Add your source files to the list.
# Supported *.c  *.cpp  *.S files.
# For other file types write a template rule for build, see below.
SOURCES  = $(DAEMON_NAME).c         \
           $(COMMON_DIR)/daemon.c





OBJECTS  := $(patsubst %.c,  %.o, $(SOURCES) )
OBJECTS  := $(patsubst %.cpp,%.o, $(OBJECTS) )
OBJECTS  := $(patsubst %.S,  %.o, $(OBJECTS) )


DEBUG_OBJECTS := $(patsubst %.o, %d.o, $(OBJECTS) )




.PHONY: all
all: debug release



.PHONY: release
release: CFLAGS := -s  $(CFLAGS)
release: $(DAEMON_NAME)



.PHONY: debug
debug: DAEMON_NO_CLOSE_STDIO = 1
debug: CFLAGS := -DDEBUG  -g  $(CFLAGS)
debug: $(DAEMON_NAME)d



# release
$(DAEMON_NAME): .depend $(OBJECTS)
	$(call build_bin, $(OBJECTS))


# debug
$(DAEMON_NAME)d: .depend $(DEBUG_OBJECTS)
	$(call build_bin, $(DEBUG_OBJECTS))



# Build release objects
%.o: %.c
	$(build_object)


%.o: %.cpp
	$(build_object)


%.o: %.S
	$(build_object)



# Build debug objects
%d.o: %.c
	$(build_object)


%d.o: %.cpp
	$(build_object)


%d.o: %.S
	$(build_object)



.PHONY: clean
clean:
	-@rm -f $(DAEMON_NAME)
	-@rm -f $(DAEMON_NAME)d
	-@rm -f $(OBJECTS)
	-@rm -f $(DEBUG_OBJECTS)
	-@rm -f .depend
	-@rm -f *.*~




.depend: cmd  = echo "  [depend]  $(var)" &&
.depend: cmd += $(GCC) $(CFLAGS) -MT ".depend $(basename $(var)).o $(basename $(var))d.o"  -MM $(var) >> .depend;
.depend: $(SOURCES)
	@rm -f .depend
	@echo "Generating dependencies..."
	@$(foreach var, $(SOURCES), $(cmd))


include $(wildcard .depend)




# Common commands
BUILD_ECHO = echo "\n  [build]  $@:"


define build_object
    @$(BUILD_ECHO)
    $(GCC) -c $< -o $@  $(CFLAGS)
endef


define build_bin
    @$(BUILD_ECHO)
    $(GCC)  $1 -o $@  $(CFLAGS)
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


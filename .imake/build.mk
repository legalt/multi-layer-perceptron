ifeq ($(imake_dir),)
    imake_dir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
endif

include $(imake_dir)/imake.mk

SRCDIRS   =

SOURCES   =

SOURCES_FORCE =

OBJDIR := $(builddir)/$(basename $(TARGET))

TARGETS_WITH_PATH = $(addprefix $(bindir), /$(TARGET))

STATIC_TARGET = $(filter %.a, $(TARGETS_WITH_PATH))
DYNAMIC_TARGET = $(filter %.so, $(TARGETS_WITH_PATH))
APPLICATION_TARGET = $(filter-out %.so %.a, $(TARGETS_WITH_PATH))

all: $(STATIC_TARGET) $(DYNAMIC_TARGET) $(APPLICATION_TARGET)

include $(PROJECT)

SOURCES +=

CFLAGS += -fPIC

CXXFLAGS += -fPIC


# The pre-processor and compiler options.
ifdef debug
    CFLAGS  += -DDEBUG -g
    CXXFLAGS += -DDEBUG -g
else
    CFLAGS  += -O2 -W -D_REENTRANT
endif

# The linker options.

ifeq ($(PLATFORM_LIBS),)
  LIBS  += 
else
  LIBS += $(foreach l,$(PLATFORM_LIBS), $(addprefix -L,$(l)))
endif

INCLUDES  += $(PLATFORM_INCLUDES)

# The options used in linking as well as in any direct use of ld.
LDFLAGS +=
#-WL,-rpath -WL,./lib -WL,-rpath -WL,/home/default


## Implicit Section: change the following only when necessary.
##==========================================================================

# The source file types (headers excluded).
# .c indicates C source files, and others C++ ones.
SRCEXTS = .c .C .cc .cpp .CPP .c++ .cxx .cp

# The header file types.
HDREXTS = .h .H .hh .hpp .HPP .h++ .hxx .hp

ETAGS = etags
ETAGSFLAGS =

CTAGS = ctags
CTAGSFLAGS =

## Stable Section: usually no need to be changed. But you can add more.
##==========================================================================
SHELL   = /bin/sh
EMPTY   =
SPACE   = $(EMPTY) $(EMPTY)

SOURCES_INNER = $(SOURCES)

ifneq ($(SRCDIRS),)
  SOURCES_INNER += $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
endif

SOURCES_INNER := $(sort $(SOURCES_INNER))

HEADERS = $(foreach d,$(INCLUDES),$(wildcard $(addprefix $(d)/*,$(HDREXTS))))
HEADERS_PRECOMPILED = $(foreach d,$(INCLUDES),$(wildcard $(d)/stdafx.h))
HEADERSFLAG = $(foreach dir,$(INCLUDES),$(addprefix -I,$(dir)))

# A literal space.
space :=
space +=
join-with = $(subst $(space),$1,$(strip $2))
VPATH = $(call join-with,:,$(sort $(dir $(SOURCES_INNER))))

SRC_CXX = $(filter-out %.c,$(SOURCES_INNER))
OBJS    = $(addprefix $(OBJDIR)/, $(notdir $(addsuffix .o, $(basename $(SOURCES_INNER)))))
OBJS_FORCE = $(addprefix $(OBJDIR)/, $(notdir $(addsuffix .o, $(basename $(SOURCES_FORCE)))))

DEPS    = $(OBJS:.o=.d)

## Define some useful variables.
DEP_OPT = $(shell if `$(CC) --version | grep "GCC" >/dev/null`; then \
                  echo "-MM -MP"; else echo "-M"; fi )
DEPEND.c      = $(CC)   $(DEP_OPT) $(CFLAGS) $(HEADERSFLAG) -c
DEPEND.cxx    = $(CXX)  $(DEP_OPT) $(CXXFLAGS) $(HEADERSFLAG) -c

COMPILE.c   = $(CC)  $(CFLAGS)   $(HEADERSFLAG) -c
COMPILE.cxx = $(CXX) $(CXXFLAGS) $(PRE_CXXFLAGS) $(HEADERSFLAG) -c
PRE_COMPILE.cxx = $(CXX) $(CXXFLAGS) $(HEADERSFLAG) -c
LINK.c      = $(CC)  $(CFLAGS)   $(LDFLAGS)
LINK.cxx    = $(CXX) $(CXXFLAGS) $(LDFLAGS)

ifeq ($(wildcard $(OBJS_FORCE)),)
.FORCE: ;
else

.FORCE:
	$(VERBOSE)$(RM) $(OBJS_FORCE)
endif

.PHONY: all deps objs tags ctags clean distclean help show .FORCE

# Delete the default suffixes
.SUFFIXES:

deps: $(DEPS)

# we want the directory to be created before any TARGET_PATHs are placed into it but,
# because the timestamps on directories change whenever a file is added, removed, or renamed,
# we certainly don’t want to rebuild all the TARGET_PATHs whenever the directory’s timestamp changes.
# One way to manage this is with order-only prerequisites:
# make the directory an order-only prerequisite on all the TARGET_PATHs
$(DEPS) : | $(external_libs) $(OBJDIR)

$(OBJDIR)/%.d:%.c
	@echo make dependency $< ...
	$(VERBOSE)$(DEPEND.c) $< >> $@

$(OBJDIR)/%.d: %.cpp
	@echo make dependency $< ...
	$(VERBOSE)$(DEPEND.cxx) $< >> $@

$(OBJDIR)/%.d: %.cxx
	@echo make dependency $< ...
	$(VERBOSE)$(DEPEND.cxx) $< >> $@

# Rules for generating object files (.o).
#----------------------------------------

objs: $(OBJS)

# we want the directory to be created before any TARGET_PATHs are placed into it but,
# because the timestamps on directories change whenever a file is added, removed, or renamed,
# we certainly don’t want to rebuild all the TARGET_PATHs whenever the directory’s timestamp changes.
# One way to manage this is with order-only prerequisites:
# make the directory an order-only prerequisite on all the TARGET_PATHs
$(OBJS) : | $(PRE_DST_BIN)
$(OBJS) : | $(DEPS) $(OBJDIR)

$(OBJDIR)/%.o:%.c
	@echo compile $< ...
	$(VERBOSE)$(COMPILE.c) $< -o $@

$(OBJDIR)/%.o:%.cpp
	@echo compile $< ...
	$(VERBOSE)$(COMPILE.cxx) $< -o $@

$(OBJDIR)/%.o:%.cxx
	@echo compile $< ...
	$(VERBOSE)$(COMPILE.cxx) $< -o $@

$(OBJDIR):
	$(VERBOSE)mkdir -p $@

# Rules for generating the tags.
#-------------------------------------
tags: $(HEADERS) $(SOURCES_INNER)
	$(ETAGS) $(ETAGSFLAGS) $(HEADERS) $(SOURCES_INNER)

ctags: $(HEADERS) $(SOURCES_INNER)
	$(CTAGS) $(CTAGSFLAGS) $(HEADERS) $(SOURCES_INNER)

# Rules for generating the library.
#-------------------------------------

$(STATIC_TARGET): $(OBJS) | $(bindir) .FORCE 
	@echo link $< ...
	$(VERBOSE)$(AR) rcs $@ $(OBJS) $(LIB_OBJS)

$(DYNAMIC_TARGET): $(OBJS) | $(bindir) .FORCE
	@echo link $< ...
	$(VERBOSE)$(LINK.cxx) -o $@ -shared $(OBJS) $(LIBS)

$(APPLICATION_TARGET): $(OBJS) | $(bindir) .FORCE
	@echo link $< ...
	$(VERBOSE)$(LINK.cxx) -o $@ $(OBJS) $(LIBS)

strip: $(APPLICATION_TARGET) $(DYNAMIC_TARGET) | $(stripdir)
	@echo strip $< ...
	$(VERBOSE)$(STRIP) $< -o $(stripdir)/$(notdir $<)

ifndef NODEP
ifneq ($(DEPS),)
  -include $(DEPS)
endif
endif

clean:
	$(VERBOSE)rm -rf $(OBJDIR)
	$(VERBOSE)rm -rf $(TARGETS_WITH_PATH) $(stripdir)/$(notdir $(TARGETS_WITH_PATH))
	$(VERBOSE)rm -d $(bindir) 2>&- | true
	$(VERBOSE)rm -d $(stripdir) 2>&- | true
	$(VERBOSE)rmdir -p $(builddir) 2>&- | true

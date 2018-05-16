# args:
# @param passport  - project pasport file

ifeq ($(imake_dir),)
    imake_dir := $(abspath ./.imake)
endif

export imake_dir

include $(imake_dir)/imake.mk

ifeq ($(wildcard $(passport)),)
    $(error $(passport) not found)
endif

include $(passport)

project := $(notdir $(basename $(passport)))

include $(passport)

ifneq ($(makefile),)
  makefile_flag := -f $(makefile)
endif

.PHONY: all FORCE fetch realclean report clean-report build clean install uninstall collect-passport

all: build

# fetch

fetch: $(srcdir) $(passport)
	$(VERBOSE)make -f  $(imake_dir)/fetch-project.mk passport=$(passport) all
	$(VERBOSE)cd $(srcdir)/$(project); make projectdir=$(srcdir)/$(project) $(makefile_flag) fetch-externals

# realclean

ifeq ($(wildcard $(srcdir)/$(project)),)
realclean: ;
else
realclean: clean clean-report
	$(VERBOSE)cd $(srcdir)/$(project); make projectdir=$(srcdir)/$(project) $(makefile_flag) realclean-externals
	$(VERBOSE)rm -rf $(srcdir)/$(project)
	$(VERBOSE)rmdir -p $(srcdir) 2>&- | true
endif

# report

report: fetch
	$(VERBOSE)make -f  $(imake_dir)/report-project.mk passport=$(passport)

clean-report:
	$(VERBOSE)make -f  $(imake_dir)/report-project.mk passport=$(passport) clean

# collect-passport

collect-passport: report
	$(VERBOSE)cd $(srcdir)/$(project); make projectdir=$(srcdir)/$(project) destdir=$(abspath $(destdir)) $(makefile_flag) collect-passport-externals
	$(VERBOSE)cp $(passportdir)/* $(abspath $(destdir))

# build

build: report
	$(VERBOSE)cd $(srcdir)/$(project); make projectdir=$(srcdir)/$(project) $(makefile_flag)

# clean

ifeq ($(wildcard $(srcdir)/$(project)),)
clean: ;
else
clean:
	$(VERBOSE)cd $(srcdir)/$(project);make $(makefile_flag) projectdir=$(srcdir)/$(project) clean
	$(VERBOSE)cd $(srcdir)/$(project);make $(makefile_flag) projectdir=$(srcdir)/$(project) clean-externals
endif

# install

install: build
	$(VERBOSE)cd $(srcdir)/$(project); make projectdir=$(srcdir)/$(project) destdir=$(abspath $(destdir)) $(makefile_flag) install

# uninstall

uninstall: report
	$(VERBOSE)cd $(srcdir)/$(project); make projectdir=$(srcdir)/$(project) destdir=$(abspath $(destdir)) $(makefile_flag) uninstall

ifeq ($(imake_dir),)
    imake_dir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
endif

export imake_dir

# add tools 

include $(imake_dir)/tools.mk

# include spec
ifeq ($(spec_file),)
  ifeq ($(spec),)
    ifeq ($(wildcard $(imake_dir)/../infomir.spec),)
        spec := /usr/local/src/infomir.spec
    else
        spec := $(imake_dir)/../infomir.spec
    endif
  endif
  spec_file := $(abspath $(spec))
endif

export spec_file

ifeq ($(VERBOSE),)
    $(info Use spec file: $(spec_file))
endif

-include $(spec_file)

ifneq ($(shared-libs),)
    SHARED_LIBS := $(shared-libs)
endif

export SHARED_LIBS

ifneq ($(shared-includess),)
    SHARED_INCLUDES := $(shared-includes)
endif

export SHARED_INCLUDES

# include path
ifeq ($(path_file),)
  ifeq ($(path),)
    ifeq ($(wildcard $(imake_dir)/../mk/path.mk),)
      path = /usr/local/src/stb-platform-lib-path.mk
    else
      path = $(imake_dir)/../mk/path.mk
    endif
  endif
  path_file := $(abspath $(path))
endif

export path_file

ifeq ($(VERBOSE),)
    $(info Use path file: $(path_file))
endif
-include $(path)

ifneq ($(model),)
    INFOMIR_MODEL_NAME := $(model)
endif

ifeq ($(VERBOSE),)
    $(info Model: $(INFOMIR_MODEL_NAME))
endif

# ------------------------------------------------------------
# makefile variables

.DEFAULT_GOAL = all

ifneq ($(jobs),)
JOB_FLAG := -j$(jobjs)
else
JOB_FLAG ?= -j4
endif

# ------------------------------------------------------------

# docker init SHARED_LIBS, we use it then
ifeq ($(INFOMIR_SHARED_LIBS),)
    INFOMIR_SHARED_LIBS :=$(SHARED_LIBS)
endif

ifeq ($(INFOMIR_MODEL_NAME),)
    $(error INFOMIR_MODEL_NAME is undefined)
endif

-include $(imake_dir)/models/$(INFOMIR_MODEL_NAME)

include $(imake_dir)/cross.mk

ifeq ($(builddir),)
    builddir := $(abspath $(imake_dir)/../.$(INFOMIR_MODEL_NAME)/build)
endif

ifeq ($(bindir),)
    bindir := $(abspath $(imake_dir)/../.$(INFOMIR_MODEL_NAME)/bin)
endif

imake-bin := $(bindir)

ifeq ($(stripdir),)
    stripdir := $(abspath $(imake_dir)/../.$(INFOMIR_MODEL_NAME)/strip)
endif

ifeq ($(srcdir),)
    srcdir := $(abspath $(imake_dir)/../.$(INFOMIR_MODEL_NAME)/srcdir)
endif

imake-src := $(srcdir)

ifeq ($(reportdir),)
    reportdir := $(abspath $(imake_dir)/../.$(INFOMIR_MODEL_NAME)/out/reports)
endif

ifeq ($(destdir),)
    destdir := $(abspath $(imake_dir)/../.$(INFOMIR_MODEL_NAME)/out)
endif

ifeq ($(projectdir),)
    projectdir := $(abspath $(imake_dir)/..)
endif

ifeq ($(passportdir),)
    passportdir = $(abspath $(projectdir)/external-libs/passports)
endif

$(builddir) $(stripdir) $(bindir) $(srcdir) $(reportdir) $(destdir) $(passportdir):
	$(VERBOSE)mkdir -p $@

include $(imake_dir)/externals.mk

ifeq ($(is-it-main-makefile),)

  include $(imake_dir)/common-targets.mk

  include $(imake_dir)/qtcreator.mk

  export is-it-main-makefile := 1

endif

.PHONY: help

help::
	@echo Command line variables:
	@echo '    jobs             : jobs=N       Allow N jobs at once; infinite jobs with no arg.'
	@echo '    verbose          : verbose=1    Shows details about the results of running.'
	@echo '    model            : model=mag324 Setup the model type.'
	@echo '    IFOMIR_MODEL_NAME: The same as model.'
	@echo '    spec             : spec=<file>  Setup spec file. Default: [/usr/local/src/infomir.spec].'
	@echo '    shared-includes  : unsupported yet'
	@echo '    shared-libs      : Shared libs path. You can use it to setup path to the boost, json-spirit etc ...'
	@echo '    destdir          : destdir=<dir>     The installation directory. Default: [<INFOMIR_MODEL_NAME>/out/reports].'
	@echo '    passportdir      : passportdir=<dir> The directory with *.passport files for the external projects. Default: [./external-libs/passports].'
	@echo Targets:
	@echo '    build           : Build the project. The results will be in <bindir> folder.'
	@echo '    strip           : Build the project and strip the result. The results will be in <stripdir> folder.'
	@echo '    clean           : Clean build results like libs, executables, object and dependency files.'
	@echo '    realclean       : Clean build results like libs, executables, object and dependency files. Including the external projects.'
	@echo '    install         : Install the build results and headers to the <destdir>.'
	@echo '                      make install [destdir=<dir>]'
	@echo '    uninstall       : Uinstall the build results from the <destdir>.'
	@echo '                      make uninstall [destdir=<dir>]'
	@echo '    fetch           : Fetch all external projects source to the <srcdir>. Alias for the fetch-externals.'
	@echo '    report          : Collect the reports to the <reportdir>.'
	@echo '    clean-report    : Clean all reports from the <reportdir>.'
	@echo '    qtcreator       : Create the qtcreator project files in the <destdir> folder.'
	@echo '                      make qtcreator [destdur=<dir>]'
	@echo Targets related to all external projects:
	@echo '    build-externals            : Build all external projects. The results will be in <bindir> folder.'
	@echo '    clean-externals            : Clean build results like libs, executables, object and dependency files for all external projects.'
	@echo '    fetch-externals            : Fetch all external projects source to the <srcdir>.'
	@echo '    report-externals           : Collect the reports to the <reportdir> for all external projects.'
	@echo '    clean-report-externals     : Clean all reports from the <reportdir> for all external projects.'
	@echo '    install-externals          : Install the build results and headers to the <destdir> for all external projects.'
	@echo '    uninstall-externals        : Uninstall the build results and headers from the <destdir> for all external projects.'
	@echo '    show-externals             : Show the external projects list.'
	@echo '    collect-passport           : Collect the passports for all external projects to the <destdir> folder.'
	@echo '                               make collect-passport destdir=<dir>'
	@echo targets related to external project:
	@echo '    build-<project>       :'
	@echo '    clean-<project>       :'
	@echo '    fetch-<project>       :'
	@echo '    realclean-<project>   :'
	@echo '    report-<project>      :'
	@echo '    clean-report-<project>:'
	@echo '    install-<project>     :'
	@echo '    uninstall-<project>   :'
	@echo 'Internal variables:'
	@echo '    bindir     : The executables/libraries direcrory. Value: [<INFOMIR_MODEL_NAME>/bin].'
	@echo '    imake-bin  : The same as <bindir>. You can use it in *.mk scripts to link with external libraries.'
	@echo '    builddir   : The build directory. *.o *.d files will be there. Value: [<INFOMIR_MODEL_NAME>/build].'
	@echo '    stripdir   : The stripped executables/libraries direcrory. Value: [.<INFOMIR_MODEL_NAME>/strip].'
	@echo '    srcdir     : The external projects will be there. Value: [.<INFOMIR_MODEL_NAME>/srcdir].'
	@echo '    imake-src  : The same as <srcdir>. You can use it in *.mk scripts to include headers from external projects.'
	@echo '    reportdir  : The directory for the reports. Value: [<INFOMIR_MODEL_NAME>/out/reports].'

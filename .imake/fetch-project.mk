# fetch repository
# args:
# @param passport  - project pasport file
# @param imake_dir - imake folder

ifeq ($(wildcard $(passport)),)
    $(error Passport $(passport) not found)
endif

ifeq ($(imake_dir),)
    $(error imake_dir undefined)
endif

include $(imake_dir)/imake.mk

project = $(notdir $(basename $(passport)))

fetchdir := $(srcdir)/$(project)

report := $(reportdir)/$(project)

#$(info fetch for the $(project))

include $(passport)

-include $(report)

PASSPORT_MD5 := $(call passport_md5, $(passport))
PASSPORT_ID := $(call md5_calc, $(passport))

ifeq ($($(project)-passport-md5),)
  $(if $(verbose), $(info there is no report for this project yet. project: $(project)),)
else
ifeq ($($(project)-passport-id),)
  $(if $(verbose), $(info there is broken report for this project.  we need to remove report and project folder. project: $(project)),)
  $(shell rm -rf $(fetchdir) $(report))
else
  ifeq ($($(project)-passport-id),$(PASSPORT_ID))
    $(if $(verbose), $(info this report was generated by this passport. project: $(project)),)
    ifneq ($($(project)-passport-md5),$(PASSPORT_MD5))
      $(if $(verbose), $(info the passport was changed. we need to remove report and project folder. project: $(project)),)
      $(shell rm -rf $(fetchdir) $(report))
    endif
  else
    $(if $(verbose), $(info this report was generated by different passport. project: $(project)),)
    ifneq ($($(project)-passport-md5),$(PASSPORT_MD5))
      $(error We already have the different version $(project) project)
    endif
  endif
endif
endif

ifneq ($(wildcard $(fetchdir)),)
# fetchdir already exists. do nothing
all: ;
else
ifneq ($(strip $(SVN)),)
  include $(imake_dir)/fetch-svn-project.mk
else ifneq ($(strip $(GIT)),)
  include $(imake_dir)/fetch-git-project.mk
else ifneq ($(strip $(FILE)),)
  ifeq ($(wildcard $(FILE)),)
    $(error destination not found. '$(FILE)')
all: ;
  else

all: $(srcdir)
	$(VERBOSE)$(shell ln -s $(FILE) $(fetchdir))
  endif
else
    $(error unknown protocol)
endif
endif


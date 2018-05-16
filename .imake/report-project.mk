# make repository report
# args:
# @param passport  - project pasport file
# @param imake_dir - imake folder


ifeq ($(imake_dir),)
    $(error imake_dir undefined)
endif

include $(imake_dir)/imake.mk

ifeq ($(wildcard $(passport)),)

  project = $(notdir $(basename $(projectdir)))

else 

  project = $(notdir $(basename $(passport)))

  projectdir := $(srcdir)/$(project)

endif

ifeq ($(VERBOSE),)

  $(info Make report for project $(projectdir) $(passport))

endif


report := $(reportdir)/$(project)

difffile := $(basename $(report)).diff

#$(info build report file for the $(project))

.PHONY: all report clean


all: report

report: clean $(report) $(difffile)

clean:
	$(VERBOSE)rm -f $(report)
	$(VERBOSE)rm -f $(difffile)
	$(VERBOSE)rmdir -p $(reportdir) 2>&- | true

ifeq ($(wildcard $(projectdir)),)
  $(warning Project $(projectdir) folder not found. Passport: $(passport))
else
  git-repo := $(shell cd $(projectdir);git config --get remote.origin.url)

  svn-repo := $(shell cd $(projectdir);svn info --show-item url 2>&-)
endif

include $(passport)

PASSPORT_MD5 = $(call passport_md5)
PASSPORT_ID = $(call md5_calc, $(passport))

ifneq ($(strip $(git-repo)),)

git-commit = $(shell cd $(projectdir);git rev-parse HEAD)
git-branch = $(shell cd $(projectdir);git rev-parse --abbrev-ref HEAD)
git-status = $(shell cd $(projectdir);git status)
git-diff = $(shell cd $(projectdir);git diff)

$(report): $(reportdir)
	@echo $(project)-passport-id = $(PASSPORT_ID) > $@
	@echo $(project)-passport-md5 = $(PASSPORT_MD5) >> $@
	@echo $(project)-git = $(git-repo) >> $@
	@echo $(project)-commit = $(git-commit) >> $@
	@echo $(project)-branch = $(git-branch) >> $@
	@echo "# $(subst ",',$(git-status))" >> $@

ifneq ($(strip $(git-diff)),)

$(difffile): $(reportdir)
	@$(shell cd $(projectdir);git diff > $(difffile))

else

$(difffile): ;

endif

else ifneq ($(strip $(svn-repo)),)

svn-rev = $(shell cd $(projectdir);svn info --show-item revision)
svn-status = $(shell cd $(projectdir);svn status)
svn-diff = $(shell cd $(projectdir);svn diff)

$(report): $(reportdir)
	@echo $(project)-passport-id  = $(PASSPORT_ID) > $@
	@echo $(project)-passport-md5 = $(PASSPORT_MD5) >> $@
	@echo $(project)-svn = $(svn-repo)>> $@
	@echo $(project)-rev = $(svn-rev) >> $@
	@echo "# $(subst ",',$(svn-status))" >> $@

ifneq ($(strip $(svn-diff)),)

$(difffile): $(reportdir)
	$(VERBOSE)$(shell cd $(projectdir);svn diff > $(difffile))

else

$(difffile): ;

endif

else
    $(info Project $(project). Nothing to report. Unknown repository type)
endif



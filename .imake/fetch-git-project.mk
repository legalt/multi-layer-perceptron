# fetch git repository
# args:
# @param fetchdir - destination folder
# @param GIT     - git repository
# @param BRANCH  - git branch name (optional)
# @param COMMIT  - commit

ifeq ($(strip $(GIT)),)
    $(error undefined GIT repository)
endif

.PHONY: all clean

all: $(fetchdir)

clean:
	$(VERBOSE)rm -rf $(fetchdir)

ifneq ($(strip $(BRANCH)),)

$(fetchdir):
	@+git clone $(GIT) -b $(BRANCH) $(fetchdir)

else ifneq ($(strip $(COMMIT)),)

$(fetchdir):
	@+git clone $(GIT) $(fetchdir)
	$(VERBOSE)cd $(fetchdir); git checkout $(COMMIT)

else

$(fetchdir):
	@+git clone $(GIT) $(fetchdir)

endif


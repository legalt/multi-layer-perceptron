
empty :=
space := $(empty) $(empty)

define newline

$(space)
endef

define md5_calc
$(strip $(subst -,,$(shell echo -n $(1) | md5sum)))
endef


# return the resolved path relative to 
# args:
# @ param $(1) - detect relative path to $(1) file
# @ param $(2) - files list

relative-path = $(foreach f, $(2), $(shell realpath -q --relative-to $(1) $(f)))

# generate md5 hashe
# args:
# @param $(GIT)    - git repository
# @param $(BRANCH) - git branch name
# @param $(COMMIT) - git commit 
# @param $(SVN)    - svn repository
# @param $(REV)    - svn revision
# @param $(FILE)   - folder name

define passport_md5
$(call md5_calc, $(GIT) + $(BRANCH) + $(COMMIT) + $(SVN) + +$(REV) + $(FILE))
endef

ifdef verbose
VERBOSE :=
else
VERBOSE := @
endif

# fetch svn repository
# args:
# @param fetchdir - destination folder
# @param SVN    - svn repository
# @param REV    - svb revision. HEAD if empty

ifeq ($(strip $(SVN)),)
    $(error undefined SVN repository) 
endif

LOGIN?=libs-anon-user
PASSWD?="dfj3jf*j@34"
REV?=HEAD

.PHONY: all clean

all: $(fetchdir)

clean:
	$(VERBOSE)rm -rf $(fetchdir)

$(fetchdir): clean
	$(VERBOSE)svn co $(SVN) -r $(REV) $(fetchdir) --username=$(LOGIN) --password=$(PASSWD) --no-auth-cache


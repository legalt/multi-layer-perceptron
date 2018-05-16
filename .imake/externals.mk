# externals management
# args:
# @param passportdir - passports path

project-name = $(strip $(notdir $(basename $(1))))

# supported targets
targets = build clean fetch realclean report clean-report install uninstall collect-passport

# external projects list
projects := $(call project-name,$(wildcard $(passportdir)/*.passport))

# project-target. Add target for project.
# @param - target  name
# @param - project name

define project-recepy

$(1)-$(2):
	$(VERBOSE)make -f $(abspath $(imake_dir)/project.mk) passport=$(passportdir)/$(2).passport $(1)

endef

# common target. Make one common target with all projects related target.
# @param - target name

ifeq ($(projects),)

define common-target

$(1)-externals: ;

endef 

else

define common-target

$(1)-externals: $(foreach p,$(projects),$(1)-$(p))

endef 

endif

# project. Make project related recepes.
# @param - project name
define project

.PHONY: $(1) $(foreach t,$(targets),$(t)-$(1))

$(1): build-$(1)

$(foreach t,$(targets),$(call project-recepy,$(t),$(1)))

endef

define phony_common-targets
.PHONY: $(foreach t,$(targets),$(t)-externals)
endef

projects_recipes = $(foreach p,$(projects),$(call project,$(p)))

targets_recipes = $(foreach t,$(targets),$(call common-target,$(t)))

define externals

$(projects_recipes)

$(call phony_common-targets)

$(targets_recipes)

endef

$(eval $(externals))

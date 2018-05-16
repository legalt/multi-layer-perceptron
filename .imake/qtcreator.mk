# args:
# @param destdir      - qt creator project

.PHONY: qtcreator clean-qtcreator

qtcreator-projectdir := $(abspath .) 

qtcreator-project = $(notdir $(basename $(qtcreator-projectdir)))-$(INFOMIR_MODEL_NAME)

qtcreator-project-files-absolute = $(sort $(shell find $(qtcreator-projectdir) -name \*.h) $(shell find $(qtcreator-projectdir) -name \*.cpp)) 

qtcreator-project-files = $(qtcreator-project-files-absolute)

#$(call relative-path, $(destdir), $(qtcreator-project-files-absolute))

qtcreator-project-folders := $(sort $(dir $(qtcreator-project-files)))

qtcreator: clean-qtcreator $(destdir)
	@echo [General] > $(destdir)/$(qtcreator-project).creator
	@echo "#" > $(destdir)/$(qtcreator-project).files
	@$(foreach f,$(qtcreator-project-files), $(newline)echo $(f) >> $(destdir)/$(qtcreator-project).files)
	@echo "#" > $(destdir)/$(qtcreator-project).includes
	@$(foreach f,$(qtcreator-project-folders), $(newline)echo $(f) >> $(destdir)/$(qtcreator-project).includes)

clean-qtcreator:
	$(VERBOSE)rm -f $(destdir)/$(qtcreator-project).files
	$(VERBOSE)rm -f $(destdir)/$(qtcreator-project).includes
	$(VERBOSE)rmdir -p $(destdir) 2>&- | true

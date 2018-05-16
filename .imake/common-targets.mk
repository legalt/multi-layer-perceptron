# aliases

.PHONY: fetch report clean-report realclean collect-passport show-externals

fetch: fetch-externals

report: report-externals
	$(VERBOSE)make -f $(imake_dir)/report-project.mk report

clean-report: clean-report-externals
	$(VERBOSE)make -f $(imake_dir)/report-project.mk clean

realclean: clean clean-report realclean-externals

clean-externals: clean

ifeq ($(wildcard $(passportdir)),)
collect-passport: ;
else
collect-passport: $(destdir) collect-passport-externals 
endif

show-externals:
	@echo $(projects)

debug-externals:
	$(info $(externals))

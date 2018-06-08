export INFOMIR_MODEL_NAME = pc

ifeq ($(imake_dir),)
    imake_dir := $(abspath ./.imake)
endif

include $(imake_dir)/imake.mk

.PHONY: all clean build

all: clean build

build: annLib annApp

clean: annLib_clean annApp_clean

annLib:
	$(VERBOSE)make $(JOB_FLAG) -f $(imake_dir)/build.mk TARGET=annLib.a PROJECT=./mk/ann.mk

annLib_clean:
	$(VERBOSE)rm -rf /.$(INFOMIR_MODEL_NAME)/bin/annLib.a /.$(INFOMIR_MODEL_NAME)/build/annLib

annApp:
	$(VERBOSE)make $(JOB_FLAG) -f $(imake_dir)/build.mk TARGET=annApp PROJECT=./mk/annApp.mk

annApp_clean:
	$(VERBOSE)rm -rf ./.$(INFOMIR_MODEL_NAME)/bin/annApp ./.$(INFOMIR_MODEL_NAME)/build/annApp

annAppMnist:
	$(VERBOSE)make $(JOB_FLAG) -f $(imake_dir)/build.mk TARGET=annAppMnist PROJECT=./mk/annAppMnist.mk

annAppMnist_clean:
	$(VERBOSE)rm -rf ./.$(INFOMIR_MODEL_NAME)/bin/annAppMnist ./.$(INFOMIR_MODEL_NAME)/build/annAppMnist
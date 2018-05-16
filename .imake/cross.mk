ifeq ($(CROSS),)

  ifeq ($(VERBOSE),)
    $(info CROSS is undefined. use host gcc)
  endif

    CC = gcc
    CXX = g++
    LD = ld
    AR = ar
    STRIP = strip
else
    CC = $(CROSS)-gcc
    CXX = $(CROSS)-g++
    LD = $(CROSS)-ld
    AR = $(CROSS)-ar
    STRIP = $(CROSS)-strip
endif
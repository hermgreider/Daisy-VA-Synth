# Project Name
TARGET = oscvasynth

USE_DAISYSP_LGPL = 1
LDFLAGS += -u _printf_float

# Sources
CPP_SOURCES = main.cpp synthui.cpp midihandlerreface.cpp vasynth.cpp 

# Library Locations
LIBDAISY_DIR = libDaisy
DAISYSP_DIR = DaisySP

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

